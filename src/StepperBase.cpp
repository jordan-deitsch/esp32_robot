#include "StepperBase.h"

StepperBase::StepperBase(const char* name, uint16_t maxStepsPerSec)
    : _name(name), _maxSps(maxStepsPerSec) {
  _sps = (200 < maxStepsPerSec) ? 200 : maxStepsPerSec;
}

void StepperBase::setSpeed(uint16_t stepsPerSec) {
  if (stepsPerSec < 1) stepsPerSec = 1;
  if (stepsPerSec > _maxSps) stepsPerSec = _maxSps;
  _sps = stepsPerSec;
}

void StepperBase::setRunning(bool run) {
  if (run == _running) return;
  _running = run;
  if (run) {
    energize(true);
    _lastStepUs = micros();
  } else {
    energize(false);   // de-energize so the motor/driver doesn't sit hot
  }
}

void StepperBase::update() {
  if (!_running) return;

  const uint32_t interval = 1000000UL / _sps;
  const uint32_t now = micros();
  uint8_t burst = 0;

  // Catch up on missed steps (e.g. while the web server was busy), but cap it.
  while ((uint32_t)(now - _lastStepUs) >= interval && burst < 4) {
    doStep(_cw);
    _position += _cw ? 1 : -1;
    _lastStepUs += interval;
    burst++;
  }
  // Still behind? Drop the backlog rather than running in a burst.
  if ((uint32_t)(now - _lastStepUs) >= interval) _lastStepUs = now;
}
