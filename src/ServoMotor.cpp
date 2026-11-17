#include "ServoMotor.h"
#include "config.h"

void ServoMotor::begin(uint8_t pin, int minUs, int maxUs) {
  _pin = pin;
  _minUs = minUs;
  _maxUs = maxUs;
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  _servo.setPeriodHertz(50);
}

void ServoMotor::setEnabled(bool on) {
  if (on == _enabled) return;
  _enabled = on;
  if (on) {
    _servo.attach(_pin, _minUs, _maxUs);
    _servo.write(_angle);
  } else {
    _servo.detach();
  }
}

void ServoMotor::setAngle(int degrees) {
  _angle = constrain(degrees, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  if (_enabled) _servo.write(_angle);
}
