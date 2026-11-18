#pragma once
#include "StepperBase.h"

// Bipolar stepper on a SparkFun Easy Driver (A3967): STEP / DIR / ENABLE.
class EasyDriverStepper : public StepperBase {
public:
  EasyDriverStepper(const char* name, uint8_t stepPin, uint8_t dirPin,
                    uint8_t enablePin, uint16_t maxStepsPerSec);
  void begin() override;
  bool supportsHold() const override { return true; }

protected:
  void doStep(bool cw) override;
  void energize(bool on) override;

private:
  uint8_t _stepPin, _dirPin, _enPin;
};
