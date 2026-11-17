#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "StepperBase.h"
#include "ServoMotor.h"

class WebApp {
public:
  WebApp(StepperBase** steppers, size_t count, ServoMotor& servo);
  void begin();     // connect Wi-Fi (or start AP) and start HTTP server
  void handle();    // call from loop()

private:
  void connectWifi();
  void sendStatus();
  void handleStepper();
  void handleServo();
  void handleStopAll();

  WebServer _server{80};
  StepperBase** _steppers;
  size_t _count;
  ServoMotor& _servo;
};
