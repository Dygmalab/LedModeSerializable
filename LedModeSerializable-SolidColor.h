/* LedModeSerializable_SolidColor - LED Solid Color effect that is then serialized to the keyscanner when executed.
 * Copyright (C) 2023, 2024  DygmaLabs, S. L.
 *
 * The MIT License (MIT)
 * Copyright © 2024 DygmaLab S.L.
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

#ifdef KEYSCANNER
#include <LEDManagement.hpp>
#endif
class LedModeSerializable_SolidColor : public LedModeSerializable
{
public:
  explicit LedModeSerializable_SolidColor(uint32_t id)
      : LedModeSerializable(id)
  {
  }
  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    output[index] = r_;
    output[++index] = g_;
    output[++index] = b_;
    output[++index] = w_;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    r_ = input[index];
    g_ = input[++index];
    b_ = input[++index];
    w_ = input[++index];
    base_settings.delay_ms = 100;
    return ++index;
  }

#ifdef KEYSCANNER

  void update() override
  {
    LEDManagement::set_all_leds({r_, g_, b_, w_});
    LEDManagement::set_updated(true);
  }

#endif

  uint8_t r_, g_, b_, w_;
};

static LedModeSerializable_SolidColor
    ledModeSerializableSolidColor{CRC32_STR("LedModeSerializable_SolidColor")};