#pragma once
#include "StepperBase.h"
#include "Sx1509.h"

// Unipolar stepper on a 4-input driver board (ULN2003 / 28BYJ-48 style),
// with IN1..IN4 driven from four consecutive SX1509 pins.
// Half-step drive: 8 phases per cycle.
//
// Coil changes are written to the expander's shadow register; the caller
// must call Sx1509::flush() (once per loop) to send them over I2C.
class UnipolarStepper : public StepperBase {
public:
  // firstPin: SX1509 pin wired to IN1; IN2..IN4 are firstPin+1..firstPin+3.
  UnipolarStepper(const char* name, Sx1509& io, uint8_t firstPin,
                  uint16_t maxStepsPerSec);
  void begin() override;

protected:
  void doStep(bool cw) override;
  void energize(bool on) override;

private:
  void writePhase();
  Sx1509& _io;
  uint8_t _firstPin;
  uint8_t _phase = 0;
};
