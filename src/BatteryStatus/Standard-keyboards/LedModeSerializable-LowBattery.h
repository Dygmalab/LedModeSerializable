/* LedModeSerializable_Layer - LED low battery visualization effect that serializes its content.
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

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "RFGW_communications.h"
#include "config_ks_app.h"
#include "hal_mcu_systim.h"
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
    // Low battery effect: all LEDs start red and change sequentially to background color from key 0 to key 1
    uint32_t current_time = hal_mcu_systim_ms_get(hal_mcu_systim_counter_get());
    
    // Set all keys to red initially
    for (uint8_t i = 1; i <= 10; i++)
    {
      LEDManagement::set_led_at(red, KsConfig::STALKER_POS_LEFT[0][i]);
    }
    
    // Animation: change keys sequentially to background color from key 10 (0) to key 1
    if (current_time - last_animation_time > animation_delay)
    {
      last_animation_time = current_time;
      animationStep++;
      
      // Reset animation when it reaches the end (all 10 keys + 1 for reset)
      if (animationStep > 10)
      {
        animationStep = 0;
      }
    }
    
    // Change keys to background color from key 10 (index 10) backwards
    // animationStep 0: all red
    // animationStep 1: key 10 background, rest red
    // animationStep 2: keys 10-9 background, rest red
    // ...
    // animationStep 10: all background
    for (uint8_t i = 0; i < animationStep && i < 10; i++)
    {
      uint8_t keyIndex = 10 - i; // Start from key 10 (0) and go backwards
      LEDManagement::set_led_at(background_color, KsConfig::STALKER_POS_LEFT[0][keyIndex]);
    }
    
    LEDManagement::set_updated(true, true);
  }

  void resetCounter()
  {
    // Not used in new implementation but kept for compatibility
  }

  void reset()
  {
    // Reset animation state
    animationStep = 0;
    last_animation_time = 0;
  }

private:
  static uint32_t last_animation_time;
  static uint8_t animationStep;
  static constexpr RGBW red = {255, 0, 0, 0};
  static constexpr RGBW yellow = {255, 255, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  
  // Configurable background color - change this to ledOff for black background
  static constexpr RGBW background_color = yellow;
  
  static constexpr uint16_t animation_delay = 30; // ms between animation steps (11 steps x 30ms = 330ms per cycle, ~3 cycles in 1 second)

#endif
};

// Initialize static members
uint32_t LedModeSerializable_LowBattery::last_animation_time = 0;
uint8_t LedModeSerializable_LowBattery::animationStep = 0;

static LedModeSerializable_LowBattery ledModeSerializableLowBattery{CRC32_STR("LedModeSerializable_LowBattery")};
