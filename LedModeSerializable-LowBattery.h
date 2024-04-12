/* LedModeSerializable_Layer - LED low battery visualization effect that serializes its content.
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
#include "LEDManagement.hpp"
#include "RFGW_communications.h"
#endif

class LedModeSerializable_LowBattery : public LedModeSerializable
{
public:
  explicit LedModeSerializable_LowBattery(uint32_t id)
      : LedModeSerializable(id)
  {
  }
  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    return ++index;
  }
#ifdef NEURON_WIRED
  void update() override
  {
  }
#endif

#ifdef KEYSCANNER

  void update() override
  {
    RGBW first_cell, second_cell, third_cell = {0, 0, 0, 0};
    /*Column effect*/
    switch (currentCell)
    {
    case CurrentCell::NO_CELL:
      first_cell = red;
      second_cell = red;
      third_cell = red;
      currentCell = CurrentCell::THIRD_CELL;
      break;
    case CurrentCell::THIRD_CELL:
      first_cell = ledOff;
      second_cell = red;
      third_cell = red;
      currentCell = CurrentCell::SECOND_CELL;
      break;
    case CurrentCell::SECOND_CELL:
      first_cell = ledOff;
      second_cell = ledOff;
      third_cell = red;
      currentCell = CurrentCell::FIRST_CELL;
      break;
    case CurrentCell::FIRST_CELL:
      first_cell = ledOff;
      second_cell = ledOff;
      third_cell = ledOff;
      currentCell = CurrentCell::NO_CELL;
      break;
    }
    counter--;
    LEDManagement::set_led_at(first_cell, 6);
    LEDManagement::set_led_at(second_cell, 13);
    LEDManagement::set_led_at(third_cell, 20);
    LEDManagement::set_updated(true);
  }

  void resetCounter()
  {
    counter = 9;
  }

public:
  uint8_t counter = 3;
  enum class CurrentCell
  {
    FIRST_CELL,
    SECOND_CELL,
    THIRD_CELL,
    NO_CELL,
  };
  CurrentCell currentCell = CurrentCell::NO_CELL;

private:
  static constexpr RGBW red = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};

#endif
};

static LedModeSerializable_LowBattery ledModeSerializableLowBattery{CRC32_STR("LedModeSerializable_LowBattery")};
