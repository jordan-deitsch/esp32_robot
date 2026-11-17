#pragma once
#include <Arduino.h>
#include <Wire.h>

// Minimal SX1509 16-channel I2C GPIO expander driver, outputs only.
//
// All 16 pins are used as push-pull outputs. Writes go into a 16-bit shadow
// copy; flush() sends the whole word in a single I2C transaction, and only
// when something changed. That lets several motors step in the same loop()
// pass for the cost of one bus write.
//
// Pin numbering follows the SX1509 datasheet: 0-7 = bank A (IO0-IO7),
// 8-15 = bank B (IO8-IO15).
class Sx1509 {
public:
  explicit Sx1509(uint8_t i2cAddr) : _addr(i2cAddr) {}

  // Resets the chip and makes all 16 pins LOW outputs.
  // Returns false if nothing answers at the I2C address.
  bool begin(TwoWire& wire);

  bool present() const { return _present; }

  // Update the shadow copy (no bus traffic).
  void writePin(uint8_t pin, bool high);
  // Replace 4 consecutive pins (firstPin..firstPin+3) with the low nibble of bits.
  void writeNibble(uint8_t firstPin, uint8_t bits);

  // Push the shadow copy to the chip if it changed.
  void flush();

private:
  bool writeReg(uint8_t reg, uint8_t val);
  bool writeWord(uint8_t regHigh, uint16_t val);   // regHigh = bank B, regHigh+1 = bank A

  TwoWire* _wire = nullptr;
  uint8_t _addr;
  bool _present = false;
  uint16_t _out = 0;
  bool _dirty = false;
};
