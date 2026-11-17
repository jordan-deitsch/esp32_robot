#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>

// Hobby servo. The signal is only generated while "enabled", so the servo
// stays limp (and quiet) when switched off.
class ServoMotor {
public:
  void begin(uint8_t pin, int minUs, int maxUs);
  void setEnabled(bool on);
  void setAngle(int degrees);          // 0..180

  bool enabled() const { return _enabled; }
  int angle() const    { return _angle; }

private:
  Servo _servo;
  uint8_t _pin = 0;
  int _minUs = 544, _maxUs = 2400;
  int _angle = 90;
  bool _enabled = false;
};
