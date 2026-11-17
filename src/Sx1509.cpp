#include "Sx1509.h"

// Register addresses (SX1509 datasheet, table 3). For 16-bit pairs the bank B
// register comes first, and the chip auto-increments to bank A.
namespace {
constexpr uint8_t REG_INPUT_DISABLE_B = 0x00;
constexpr uint8_t REG_PULL_UP_B       = 0x06;
constexpr uint8_t REG_PULL_DOWN_B     = 0x08;
constexpr uint8_t REG_OPEN_DRAIN_B    = 0x0A;
constexpr uint8_t REG_DIR_B           = 0x0E;   // 1 = input, 0 = output
constexpr uint8_t REG_DATA_B          = 0x10;
constexpr uint8_t REG_RESET           = 0x7D;
}

bool Sx1509::begin(TwoWire& wire) {
  _wire = &wire;

  _wire->beginTransmission(_addr);
  _present = (_wire->endTransmission() == 0);
  if (!_present) return false;

  // Software reset: write 0x12 then 0x34 to RegReset.
  writeReg(REG_RESET, 0x12);
  writeReg(REG_RESET, 0x34);
  delay(2);

  // Set the output latch LOW *before* switching pins to outputs so no coil
  // is energized for even a moment at power-up.
  _out = 0;
  writeWord(REG_DATA_B, 0x0000);
  writeWord(REG_PULL_UP_B, 0x0000);
  writeWord(REG_PULL_DOWN_B, 0x0000);
  writeWord(REG_OPEN_DRAIN_B, 0x0000);        // push-pull outputs
  writeWord(REG_INPUT_DISABLE_B, 0xFFFF);     // input buffers off (outputs only)
  _present = writeWord(REG_DIR_B, 0x0000);    // all 16 pins outputs
  _dirty = false;
  return _present;
}

void Sx1509::writePin(uint8_t pin, bool high) {
  if (pin > 15) return;
  const uint16_t mask = 1u << pin;
  const uint16_t next = high ? (_out | mask) : (_out & ~mask);
  if (next != _out) { _out = next; _dirty = true; }
}

void Sx1509::writeNibble(uint8_t firstPin, uint8_t bits) {
  if (firstPin > 12) return;
  const uint16_t mask = 0x0Fu << firstPin;
  const uint16_t next = (_out & ~mask) | ((uint16_t)(bits & 0x0F) << firstPin);
  if (next != _out) { _out = next; _dirty = true; }
}

void Sx1509::flush() {
  if (!_dirty || !_present) return;
  if (writeWord(REG_DATA_B, _out)) _dirty = false;
}

bool Sx1509::writeReg(uint8_t reg, uint8_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write(val);
  return _wire->endTransmission() == 0;
}

bool Sx1509::writeWord(uint8_t regHigh, uint16_t val) {
  _wire->beginTransmission(_addr);
  _wire->write(regHigh);
  _wire->write((uint8_t)(val >> 8));     // bank B (pins 8-15)
  _wire->write((uint8_t)(val & 0xFF));   // bank A (pins 0-7)
  return _wire->endTransmission() == 0;
}
