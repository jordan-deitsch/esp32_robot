// ESP32 Robot - web-controlled steppers + servo
// (Arduino IDE users: rename this file to ESP32_Robot.ino)
#include <Arduino.h>
#include "config.h"
#include "UnipolarStepper.h"
#include "EasyDriverStepper.h"
#include "ServoMotor.h"
#include "WebApp.h"

UnipolarStepper   uniA("Unipolar stepper A", UNI_A_IN1, UNI_A_IN2, UNI_A_IN3, UNI_A_IN4, UNI_MAX_SPS);
// UnipolarStepper   uniB("Unipolar stepper B", UNI_B_IN1, UNI_B_IN2, UNI_B_IN3, UNI_B_IN4, UNI_MAX_SPS);
EasyDriverStepper easy("Bipolar stepper (Easy Driver)", EASY_STEP_PIN, EASY_DIR_PIN, EASY_ENABLE_PIN, EASY_MAX_SPS);

StepperBase* steppers[] = {&uniA, &easy};
const size_t STEPPER_COUNT = sizeof(steppers) / sizeof(steppers[0]);

ServoMotor servo;
WebApp web(steppers, STEPPER_COUNT, servo);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nESP32 Robot starting");

  for (size_t i = 0; i < STEPPER_COUNT; i++) steppers[i]->begin();
  servo.begin(SERVO_PIN, SERVO_MIN_US, SERVO_MAX_US);
  web.begin();
}

void loop() {
  for (size_t i = 0; i < STEPPER_COUNT; i++) steppers[i]->update();
  web.handle();
}
