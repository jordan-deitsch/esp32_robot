#include "UnipolarStepper.h"

// Half-step sequence for IN1..IN4, packed as bit0 = IN1 ... bit3 = IN4.
static const uint8_t SEQ[8] = {
    0b0001, 0b0011, 0b0010, 0b0110,
    0b0100, 0b1100, 0b1000, 0b1001};

UnipolarStepper::UnipolarStepper(const char* name, Sx1509& io,
                                 uint8_t firstPin, uint16_t maxStepsPerSec)
    : StepperBase(name, maxStepsPerSec), _io(io), _firstPin(firstPin) {}

void UnipolarStepper::begin() {
  // The expander itself is set up (all pins LOW outputs) in Sx1509::begin().
  _io.writeNibble(_firstPin, 0);
}

void UnipolarStepper::writePhase() {
  _io.writeNibble(_firstPin, SEQ[_phase]);
}

void UnipolarStepper::doStep(bool cw) {
  _phase = (_phase + (cw ? 1 : 7)) & 7;
  writePhase();
}

void UnipolarStepper::energize(bool on) {
  if (on) writePhase();
  else _io.writeNibble(_firstPin, 0);
}
