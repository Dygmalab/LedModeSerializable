/* LedModeSerializable_BatteryStatus - LED battery status for Sonshi keyboard.
 * Copyright (C) 2023, 2024  DygmaLabs, S. L.
 *
 * The MIT License (MIT)
 * Copyright © 2024 DygmaLab S.L.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
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
#include "BatteryManagement.hpp"
#include "BatteryInterface.h"
#endif

class LedModeSerializable_BatteryStatus : public LedModeSerializable
{
public:
  explicit LedModeSerializable_BatteryStatus(uint32_t id)
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
    base_settings.delay_ms = 10;
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
    uint8_t batteryLevel = BatteryManagement::getBatteryPercentage();
    
    const BatteryStatus batteryStatus = BatteryManagement::getBatteryStatus();

    switch (batteryStatus)
    {
    case BatteryStatus::CHARGING_DONE:
    {
      // Show all 10 keys in green when fully charged
      for (uint8_t i = 1; i <= 10; i++)
      {
        LEDManagement::set_led_at(green, KsConfig::STALKER_POS_LEFT[0][i]);
      }
      LEDManagement::set_updated(true, true);
    }
    break;

    case BatteryStatus::CHARGING:
    {
      // Charging effect: completed keys in green, red background, sequential animation from current level to key 0
      static uint32_t last_animation_time = 0;
      static uint8_t animationStep = 0;
      uint32_t current_time = hal_mcu_systim_ms_get(hal_mcu_systim_counter_get());
      
      // Calculate which key represents current battery level
      uint8_t currentKey = (batteryLevel + 9) / 10; // Round up to nearest 10
      if (currentKey > 10) currentKey = 10;
      if (currentKey < 1) currentKey = 1;
      
      // Set completed keys (before current level) to green
      for (uint8_t i = 1; i < currentKey; i++)
      {
        LEDManagement::set_led_at(green, KsConfig::STALKER_POS_LEFT[0][i]);
      }
      
      // Set remaining keys background color.
      for (uint8_t i = currentKey; i <= 10; i++)
      {
        LEDManagement::set_led_at(ledOff, KsConfig::STALKER_POS_LEFT[0][i]);
      }
      
      // Animation: light up keys sequentially from currentKey to 10 (key 0)
      if (current_time - last_animation_time > charging_animation_delay)
      {
        last_animation_time = current_time;
        animationStep++;
        
        // Reset animation when it reaches the end
        if (animationStep > (10 - currentKey + 1))
        {
          animationStep = 0;
        }
      }
      
      // Light up keys in green from currentKey to currentKey + animationStep
      for (uint8_t i = 0; i < animationStep && (currentKey + i) <= 10; i++)
      {
        LEDManagement::set_led_at(green, KsConfig::STALKER_POS_LEFT[0][currentKey + i]);
      }
      
      LEDManagement::set_updated(true, true);
    }
    break;

    case BatteryStatus::NOT_CHARGHING:
    {
      // Show battery level in 10% increments
      // Key 1 (index 1): 1-10%    -> green if >= 6%,  yellow if 1-5%
      // Key 2 (index 2): 11-20%   -> green if >= 16%, yellow if 11-15%
      // ...
      // Key 0 (index 10): 91-100% -> green if >= 96%, yellow if 91-95%
      
      // Normal battery display with red background for empty keys
      for (uint8_t i = 1; i <= 10; i++)
      {
        // Calculate the range for this key
        uint8_t rangeStart = (i - 1) * 10 + 1;  // e.g., key 10: 91
        uint8_t rangeEnd = i * 10;              // e.g., key 10: 100
        uint8_t greenThreshold = rangeStart + 5; // e.g., key 10: 96
        
        if (batteryLevel >= greenThreshold)
        {
          // Battery is at 6% or more into this range: green
          LEDManagement::set_led_at(green, KsConfig::STALKER_POS_LEFT[0][i]);
        }
        else if (batteryLevel >= rangeStart)
        {
          // Battery is in the first 5% of this range: yellow
          LEDManagement::set_led_at(yellow, KsConfig::STALKER_POS_LEFT[0][i]);
        }
        else
        {
          // Battery hasn't reached this range yet: red
          LEDManagement::set_led_at(gray, KsConfig::STALKER_POS_LEFT[0][i]);
        }
      }
      
      LEDManagement::set_updated(true, true);
    }
    break;

    default:
    {
      // Unknown state: turn off all number key LEDs
      for (uint8_t i = 1; i <= 10; i++)
      {
        LEDManagement::set_led_at(ledOff, KsConfig::STALKER_POS_LEFT[0][i]);
      }
      LEDManagement::set_updated(true, true);
    }
    break;
    }
  }

private:
  // LED positions for number keys 1-0 on Sonshi keyboard
  // Based on led_mapping array from config_ks_app.h:
  // Row 0: 0, 6, 12, 18, 24, 5, 35, 54, 48, 42, 36, 30
  // Number keys 1-0 correspond to physical LEDs at these positions


  static constexpr RGBW green = {0, 255, 0, 0};
  static constexpr RGBW yellow = {255, 255, 0, 0};
  static constexpr RGBW gray = {0, 0, 0, 255};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  
  static constexpr uint16_t charging_animation_delay = 120; // ms between animation steps

#endif
};

static LedModeSerializable_BatteryStatus ledModeSerializableBatteryStatus{CRC32_STR("LedModeSerializable_BatteryStatus")};
