#pragma once
#include <Arduino.h>

// Common, non-blocking stepper behaviour: run/stop, direction, speed.
// Hardware-specific subclasses only implement doStep() and energize().
class StepperBase {
public:
  StepperBase(const char* name, uint16_t maxStepsPerSec);
  virtual ~StepperBase() {}

  virtual void begin() = 0;

  // Call as often as possible from loop().
  void update();

  void setRunning(bool run);
  void setClockwise(bool cw) { _cw = cw; }
  void setSpeed(uint16_t stepsPerSec);

  // Hold position: keep the coils powered while stopped (holding torque)
  // instead of releasing them to free-wheel. The setting is remembered while
  // running and applies whenever the motor is stopped. Ignored by motors
  // whose supportsHold() is false.
  void setHold(bool hold);
  virtual bool supportsHold() const { return false; }

  const char* name() const     { return _name; }
  bool running() const         { return _running; }
  bool hold() const            { return _hold; }
  bool clockwise() const       { return _cw; }
  uint16_t speed() const       { return _sps; }
  uint16_t maxSpeed() const    { return _maxSps; }
  long position() const        { return _position; }

protected:
  virtual void doStep(bool cw) = 0;      // take exactly one step
  virtual void energize(bool on) = 0;    // power coils / enable driver

private:
  const char* _name;
  uint16_t _maxSps;
  uint16_t _sps;
  bool _running = false;
  bool _hold = false;   // motors always start released after a reset
  bool _cw = true;
  uint32_t _lastStepUs = 0;
  long _position = 0;
};
