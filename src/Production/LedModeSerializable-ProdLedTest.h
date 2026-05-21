/* LedModeSerializable-ProdLedTest
 * Copyright (C) 2023, 2024  DygmaLabs, S. L.
 *
 * The MIT License (MIT)
 * Copyright 2024 DygmaLab S.L.
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

#include "../LedModeSerializable.h"
#include <cstdio>
#include <cstdint>

#ifdef KEYSCANNER
#include <LEDManagement.hpp>
#endif

class LedModeSerializable_ProdLedTest : public LedModeSerializable
{
public:
  explicit LedModeSerializable_ProdLedTest(uint32_t id)
      : LedModeSerializable(id)
  {
    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      test_states_[i] = TestState::OFF;
    }
  }

  enum class TestState : uint8_t
  {
    OFF = 0,
    SUCCESS = 1,
    WAIT = 2,
    FAIL = 3
  };

  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      output[index++] = static_cast<uint8_t>(test_states_[i]);
    }
    output[index++] = blink_counter_;
    output[index++] = static_cast<uint8_t>(all_color_mode_);
    output[index++] = static_cast<uint8_t>(force_solid_color_);
    return index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      test_states_[i] = static_cast<TestState>(input[index++]);
    }
    blink_counter_ = input[index++];
    all_color_mode_ = static_cast<Color>(input[index++]);
    force_solid_color_ = static_cast<bool>(input[index++]);
    base_settings.delay_ms = 50;
    return index;
  }

  void increase_test_success()
  {
    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      if (test_states_[i] != TestState::SUCCESS)
      {
        test_states_[i] = TestState::SUCCESS;
        break;
      }
    }
  }

  void set_success_at(uint8_t test_index)
  {
    if (test_index < MAX_TEST)
    {
      test_states_[test_index] = TestState::SUCCESS;
    }
  }

  void set_wait(uint8_t test_index)
  {
    if (test_index < MAX_TEST)
    {
      test_states_[test_index] = TestState::WAIT;
    }
  }

  void set_fail(uint8_t test_index)
  {
    if (test_index < MAX_TEST)
    {
      test_states_[test_index] = TestState::FAIL;
    }
  }

  void set_all_red()
  {
    all_color_mode_ = Color::RED;
    force_solid_color_ = true;
  }

  void set_all_green()
  {
    all_color_mode_ = Color::GREEN;
    force_solid_color_ = true;
  }

  void set_all_blue()
  {
    all_color_mode_ = Color::BLUE;
    force_solid_color_ = true;
  }

  void set_all_white()
  {
    all_color_mode_ = Color::WHITE;
    force_solid_color_ = true;
  }

  void set_all_off()
  {
    all_color_mode_ = Color::OFF;
    force_solid_color_ = false;
    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      test_states_[i] = TestState::OFF;
    }
  }

  void resume()
  {
    all_color_mode_ = Color::OFF;
  }

  void clear_force_solid_color()
  {
    force_solid_color_ = false;
  }

  void update() override
  {
    LEDManagement::set_all_leds({0, 0, 0, 0}, false);

    if (force_solid_color_)
    {
      RGBW color = {0, 0, 0, 0};
      switch (all_color_mode_)
      {
        case Color::RED:
          color = {255, 0, 0, 0};
          break;
        case Color::GREEN:
          color = {0, 255, 0, 0};
          break;
        case Color::BLUE:
          color = {0, 0, 255, 0};
          break;
        case Color::WHITE:
          color = {0, 0, 0, 255};
          break;
        default:
          break;
      }
      LEDManagement::set_all_leds(color);
      LEDManagement::set_updated(true);

      return;
    }

    for (uint8_t i = 0; i < MAX_TEST; i++)
    {
      RGBW color = {0, 0, 0, 0};

      switch (test_states_[i])
      {
        case TestState::SUCCESS:
          color = {0, 255, 0, 0};
          break;

        case TestState::WAIT:
          if ((blink_counter_ / 10) % 2 == 0)
          {
            color = {255, 150, 0, 0};
          }
          else
          {
            color = {0, 0, 0, 0};
          }
          break;

        case TestState::FAIL:
          color = {255, 0, 0, 0};
          break;

        case TestState::OFF:
        default:
          color = {0, 0, 0, 0};
          break;
      }

      LEDManagement::set_led_at(color, i);
    }

    blink_counter_++;
    if (blink_counter_ >= BLINK_TIMER_MS)
    {
      blink_counter_ = 0;
    }

    LEDManagement::set_updated(true);
  }

private:

  enum class Color : uint8_t
  {
    RED = 0,
    GREEN = 1,
    BLUE = 2,
    YELLOW = 3,
    WHITE = 4,
    OFF = 5
  };

  constexpr static uint8_t MAX_TEST = 10;
  constexpr static uint8_t BLINK_TIMER_MS = 200;
  TestState test_states_[MAX_TEST];
  uint8_t blink_counter_ = 0;
  Color all_color_mode_ = Color::OFF;
  bool force_solid_color_ = false;
};

static LedModeSerializable_ProdLedTest
    ledModeSerializableProdLedTest{CRC32_STR("LedModeSerializable_ProdLedTest")};