#pragma once
#include "StepperBase.h"

// Unipolar stepper on a 4-input driver board (ULN2003 / 28BYJ-48 style).
// Half-step drive: 8 phases per cycle.
class UnipolarStepper : public StepperBase {
public:
  UnipolarStepper(const char* name, uint8_t in1, uint8_t in2, uint8_t in3,
                  uint8_t in4, uint16_t maxStepsPerSec);
  void begin() override;

protected:
  void doStep(bool cw) override;
  void energize(bool on) override;

private:
  void writePhase();
  uint8_t _pins[4];
  uint8_t _phase = 0;
};
