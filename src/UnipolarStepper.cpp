#include "UnipolarStepper.h"

// Half-step sequence for IN1..IN4
static const uint8_t SEQ[8][4] = {
    {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
    {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}};

UnipolarStepper::UnipolarStepper(const char* name, uint8_t in1, uint8_t in2,
                                 uint8_t in3, uint8_t in4,
                                 uint16_t maxStepsPerSec)
    : StepperBase(name, maxStepsPerSec) {
  _pins[0] = in1; _pins[1] = in2; _pins[2] = in3; _pins[3] = in4;
}

void UnipolarStepper::begin() {
  for (uint8_t p : _pins) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
}

void UnipolarStepper::writePhase() {
  for (uint8_t i = 0; i < 4; i++) digitalWrite(_pins[i], SEQ[_phase][i]);
}

void UnipolarStepper::doStep(bool cw) {
  _phase = (_phase + (cw ? 1 : 7)) & 7;
  writePhase();
}

void UnipolarStepper::energize(bool on) {
  if (on) writePhase();
  else for (uint8_t p : _pins) digitalWrite(p, LOW);
}
