#pragma once

#include "LedModeSerializable.h"
#include "cstdio"
#include "LEDManagement.hpp"


class LedModeSerializable_SolidColor : public LedModeSerializable {
 public:
  explicit LedModeSerializable_SolidColor(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index   = LedModeSerializable::serialize(output);
    output[index]   = r_;
    output[++index] = g_;
    output[++index] = b_;
    output[++index] = w_;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index          = LedModeSerializable::deSerialize(input);
    r_                     = input[index];
    g_                     = input[++index];
    b_                     = input[++index];
    w_                     = input[++index];
    base_settings.delay_ms = 100;
    return ++index;
  }

  void update() override {
    LEDManagement::set_all_leds({r_, g_, b_, w_});
    LEDManagement::set_updated(true);
  }

  uint8_t r_, g_, b_, w_;
};

static LedModeSerializable_SolidColor
  ledModeSerializableSolidColor{CRC32_STR("LedModeSerializable_SolidColor")};