#include "EasyDriverStepper.h"

EasyDriverStepper::EasyDriverStepper(const char* name, uint8_t stepPin,
                                     uint8_t dirPin, uint8_t enablePin,
                                     uint16_t maxStepsPerSec)
    : StepperBase(name, maxStepsPerSec),
      _stepPin(stepPin), _dirPin(dirPin), _enPin(enablePin) {}

void EasyDriverStepper::begin() {
  pinMode(_stepPin, OUTPUT);
  pinMode(_dirPin, OUTPUT);
  pinMode(_enPin, OUTPUT);
  digitalWrite(_stepPin, LOW);
  digitalWrite(_dirPin, LOW);
  digitalWrite(_enPin, HIGH);          // HIGH = outputs disabled
}

void EasyDriverStepper::doStep(bool cw) {
  digitalWrite(_dirPin, cw ? HIGH : LOW);
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(3);                // A3967 needs >= 1 us
  digitalWrite(_stepPin, LOW);
}

void EasyDriverStepper::energize(bool on) {
  digitalWrite(_enPin, on ? LOW : HIGH);   // ENABLE is active LOW
}
