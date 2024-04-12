/* LedModeSerializable_Breathe - LED Breathe effect that is then serialized to the keyscanner when executed.
 * Copyright (C) 2023, 2024  DygmaLabs, S. L.
 *
 * The MIT License (MIT)
 * Copyright © 2024 <copyright holders>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in the
 * Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#include <LEDManagement.hpp>

class LedModeSerializable_Breathe : public LedModeSerializable {
 public:
  explicit LedModeSerializable_Breathe(uint32_t id)
    : LedModeSerializable(id) {
  }

  uint8_t serialize(uint8_t *output) const override {
    uint8_t index   = LedModeSerializable::serialize(output);
    output[index]   = breatheSaturation;
    output[++index] = breatheHue;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index     = LedModeSerializable::deSerialize(input);
    breatheSaturation = input[index];
    breatheHue        = input[++index];
    return ++index;
  }

  void update() override {
    // This code is adapted from FastLED lib8tion.h as of dd5d96c6b289cb6b4b891748a4aeef3ddceaf0e6
    uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 4;

    if (i & 0x80) {
      i = 255 - i;
    }

    i           = i << 1;
    uint8_t ii  = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

    RGBW breathe = LEDManagement::HSVtoRGB(breatheHue, breatheSaturation, i);
    breathe.w    = 0;
    LEDManagement::set_all_leds(breathe);
  }

  uint16_t breatheSaturation;

  uint16_t breatheHue;

 private:
  uint8_t breatheLastUpdate = 0;
};

static LedModeSerializable_Breathe ledModeSerializableBreathe{CRC32_STR("LedModeSerializable_Breathe")};