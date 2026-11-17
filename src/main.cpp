// ESP32 Robot - web-controlled steppers + servo
// (Arduino IDE users: rename this file to ESP32_Robot.ino)
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "Sx1509.h"
#include "UnipolarStepper.h"
#include "EasyDriverStepper.h"
#include "ServoMotor.h"
#include "WebApp.h"

static_assert(UNI_COUNT >= 1 && UNI_COUNT <= 4, "UNI_COUNT must be 1-4 (SX1509 has 16 pins)");

Sx1509 expander(SX1509_I2C_ADDR);

// Motor N uses SX1509 pins 4*(N-1) .. 4*(N-1)+3 for IN1..IN4.
UnipolarStepper uni1("Unipolar stepper 1", expander, 0,  UNI_MAX_SPS);
UnipolarStepper uni2("Unipolar stepper 2", expander, 4,  UNI_MAX_SPS);
UnipolarStepper uni3("Unipolar stepper 3", expander, 8,  UNI_MAX_SPS);
UnipolarStepper uni4("Unipolar stepper 4", expander, 12, UNI_MAX_SPS);
UnipolarStepper* const uniAll[4] = {&uni1, &uni2, &uni3, &uni4};

EasyDriverStepper easy("Bipolar stepper (Easy Driver)", EASY_STEP_PIN, EASY_DIR_PIN, EASY_ENABLE_PIN, EASY_MAX_SPS);

// Web UI cards appear in this order: unipolar 1..UNI_COUNT, then the Easy Driver.
StepperBase* steppers[UNI_COUNT + 1];
const size_t STEPPER_COUNT = UNI_COUNT + 1;

ServoMotor servo;
WebApp web(steppers, STEPPER_COUNT, servo);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nESP32 Robot starting");

  for (size_t i = 0; i < UNI_COUNT; i++) steppers[i] = uniAll[i];
  steppers[UNI_COUNT] = &easy;

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_CLOCK_HZ);
  if (expander.begin(Wire)) {
    Serial.printf("SX1509 found at 0x%02X\n", SX1509_I2C_ADDR);
  } else {
    Serial.printf("SX1509 NOT found at 0x%02X - unipolar steppers will not move. "
                  "Check SDA=%u, SCL=%u, 3.3V, GND and the ADDR jumpers.\n",
                  SX1509_I2C_ADDR, I2C_SDA_PIN, I2C_SCL_PIN);
  }

  for (size_t i = 0; i < STEPPER_COUNT; i++) steppers[i]->begin();
  expander.flush();
  servo.begin(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  web.begin();
}

void loop() {
  for (size_t i = 0; i < STEPPER_COUNT; i++) steppers[i]->update();
  expander.flush();   // one I2C write covers every unipolar step this pass
  web.handle();
  expander.flush();   // send any start/stop changes made by a web request
}
