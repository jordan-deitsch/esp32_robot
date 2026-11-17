#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Wi-Fi settings
// ---------------------------------------------------------------------------
// WIFI_SSID / WIFI_PASSWORD live in secrets.h (git-ignored).
// Copy secrets.example.h to secrets.h and edit it.
#include "secrets.h"
#define WIFI_CONNECT_TIMEOUT_MS 15000     // then fall back to access-point mode

// Fallback access point (used if the router can't be reached).
// Password must be at least 8 characters.
#define AP_SSID          "ESP32-Robot"
#define AP_PASSWORD      "robot12345"

// Reachable as http://esp32-robot.local on most networks
#define MDNS_HOSTNAME    "esp32-robot"

// ---------------------------------------------------------------------------
// Pin assignments (avoid ESP32 strapping pins 0, 2, 12, 15 and flash pins 6-11)
// ---------------------------------------------------------------------------
// I2C bus to the SX1509 GPIO expander (ESP32 default I2C pins).
constexpr uint8_t  I2C_SDA_PIN  = 21;
constexpr uint8_t  I2C_SCL_PIN  = 22;
constexpr uint32_t I2C_CLOCK_HZ = 400000;   // SX1509 supports 400 kHz fast mode

// SX1509 address: 0x3E with ADDR0/ADDR1 jumpers both open (default).
// 0x3F, 0x70, 0x71 for the other jumper combinations.
constexpr uint8_t SX1509_I2C_ADDR = 0x3E;

// Unipolar steppers -> ULN2003-style driver boards, driven from the SX1509.
// Each motor uses 4 consecutive expander pins for IN1..IN4:
//   motor 1: SX1509 pins 0-3    motor 2: pins 4-7
//   motor 3: pins 8-11          motor 4: pins 12-15
constexpr uint8_t UNI_COUNT = 4;   // 1-4; motors above this are not created

// Bipolar stepper -> SparkFun Easy Driver
constexpr uint8_t EASY_STEP_PIN   = 14;
constexpr uint8_t EASY_DIR_PIN    = 27;
constexpr uint8_t EASY_ENABLE_PIN = 33;   // Easy Driver ENABLE is active LOW

// Hobby servo signal
constexpr uint8_t SERVO_PIN = 32;

// ---------------------------------------------------------------------------
// Motor limits
// ---------------------------------------------------------------------------
// Unipolar boards are driven in HALF-step mode (8 steps per cycle).
// A 28BYJ-48 needs 4096 half-steps per output-shaft revolution and
// is reliable up to roughly 500-700 steps/s. Lower this if it stalls.
constexpr uint16_t UNI_MAX_SPS  = 600;

// Easy Driver defaults to 1/8 microstepping (MS1/MS2 not connected).
// 200-step motor => 1600 pulses per revolution.
constexpr uint16_t EASY_MAX_SPS = 2000;

// Servo pulse range in microseconds (SG90-class servos).
constexpr int SERVO_MIN_US = 600;
constexpr int SERVO_MAX_US = 2300;

// Software travel limits. Driving a servo into its mechanical end stop makes
// it buzz and draw stall current, so keep a little margin at each end.
// Widen these (toward 0 / 180) one degree at a time until just before it buzzes.
constexpr int SERVO_MIN_ANGLE = 10;
constexpr int SERVO_MAX_ANGLE = 170;
