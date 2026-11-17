#include "WebApp.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include "config.h"
#include "WebUI.h"

WebApp::WebApp(StepperBase** steppers, size_t count, ServoMotor& servo)
    : _steppers(steppers), _count(count), _servo(servo) {}

void WebApp::connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to \"%s\"", WIFI_SSID);

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < WIFI_CONNECT_TIMEOUT_MS) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected. Open http://%s/\n", WiFi.localIP().toString().c_str());
    if (MDNS.begin(MDNS_HOSTNAME)) {
      Serial.printf("Also try http://%s.local/\n", MDNS_HOSTNAME);
    }
  } else {
    Serial.println("Wi-Fi failed - starting access point instead.");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.printf("Join Wi-Fi \"%s\" then open http://%s/\n", AP_SSID,
                  WiFi.softAPIP().toString().c_str());
  }
}

void WebApp::begin() {
  connectWifi();

  _server.on("/", HTTP_GET, [this]() {
    _server.send_P(200, "text/html", INDEX_HTML);
  });
  _server.on("/api/status", HTTP_GET, [this]() { sendStatus(); });
  _server.on("/api/stepper", HTTP_GET, [this]() { handleStepper(); });
  _server.on("/api/servo",   HTTP_GET, [this]() { handleServo(); });
  _server.on("/api/stopall", HTTP_GET, [this]() { handleStopAll(); });
  _server.onNotFound([this]() { _server.send(404, "text/plain", "Not found"); });
  _server.begin();
}

void WebApp::handle() { _server.handleClient(); }

void WebApp::sendStatus() {
  String j;
  j.reserve(512);
  j += "{\"steppers\":[";
  for (size_t i = 0; i < _count; i++) {
    StepperBase* s = _steppers[i];
    if (i) j += ',';
    j += "{\"id\":";      j += i;
    j += ",\"name\":\"";  j += s->name();
    j += "\",\"running\":"; j += s->running() ? "true" : "false";
    j += ",\"cw\":";      j += s->clockwise() ? "true" : "false";
    j += ",\"speed\":";   j += s->speed();
    j += ",\"max\":";     j += s->maxSpeed();
    j += ",\"pos\":";     j += s->position();
    j += '}';
  }
  j += "],\"servo\":{\"enabled\":";
  j += _servo.enabled() ? "true" : "false";
  j += ",\"angle\":";
  j += _servo.angle();
  j += ",\"min\":";
  j += SERVO_MIN_ANGLE;
  j += ",\"max\":";
  j += SERVO_MAX_ANGLE;
  j += "}}";
  _server.send(200, "application/json", j);
}

void WebApp::handleStepper() {
  if (!_server.hasArg("id")) { _server.send(400, "text/plain", "missing id"); return; }
  int id = _server.arg("id").toInt();
  if (id < 0 || (size_t)id >= _count) { _server.send(404, "text/plain", "bad id"); return; }

  StepperBase* s = _steppers[id];
  if (_server.hasArg("speed")) s->setSpeed((uint16_t)constrain(_server.arg("speed").toInt(), 1, 65535));
  if (_server.hasArg("dir"))   s->setClockwise(_server.arg("dir") == "cw");
  if (_server.hasArg("run"))   s->setRunning(_server.arg("run") == "1");
  sendStatus();
}

void WebApp::handleServo() {
  if (_server.hasArg("angle"))   _servo.setAngle(_server.arg("angle").toInt());
  if (_server.hasArg("enabled")) _servo.setEnabled(_server.arg("enabled") == "1");
  sendStatus();
}

void WebApp::handleStopAll() {
  for (size_t i = 0; i < _count; i++) _steppers[i]->setRunning(false);
  _servo.setEnabled(false);
  sendStatus();
}
