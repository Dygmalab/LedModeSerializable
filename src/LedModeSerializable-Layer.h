/* LedModeSerializable_Layer - LED Layer highlight with user customized colors that serializes its content.
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
#include "debug_print.h"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_Layer : public LedModeSerializable
{
public:
  explicit LedModeSerializable_Layer(uint32_t id)
      : LedModeSerializable(id)
  {
  }
  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    output[index] = layer;
    output[++index] = fade_is_on;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    base_settings.delay_ms = 10;
    layer = input[index];
    fade_is_on = input[++index];
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
    static float led_driver_brightness = LEDManagement::get_max_ledDriver_brightness();
    static float underglow_brightness = LEDManagement::get_max_underglow_brightness();
    static bool max_reached = false;
    constexpr float top_brightness_level = 0.7f;
    constexpr float top_ug_brightness_level = 0.29f;
    static bool reached_ug_brightness = false;
    static bool reached_bl_brightness = false;
    static float min_led_driver_brightness = LEDManagement::get_ledDriver_brightness();
    static float min_underglow_brightness = LEDManagement::get_underglow_brightness();
    static bool flag = true;

    LEDManagement::Layer actualLayer{};
    if (this->layer < LEDManagement::layers.size())
    {
      actualLayer = LEDManagement::layers.at(this->layer);
    }

    for (uint8_t i = 0; i < KsConfig::MAX_BL_LEDS; i++)
    {
      RGBW &color = LEDManagement::palette[actualLayer.keyMap_leds[i]];
      LEDManagement::set_led_at(color, i);
    }

    for (uint8_t  i  = 0;  i < KsConfig::MAX_UG_LEDS ; i++)
    {
        RGBW &color = LEDManagement::palette[actualLayer.underGlow_leds[i]];
        LEDManagement::set_ug_at(color, i);
    }
    LEDManagement::set_updated(true);
    if (fade_is_on)
    {
      if (layer != 0)
      {
        if (flag)
        {
          LEDManagement::onMount(LEDManagement::LedBrightnessControlEffect::FADE_EFFECT);
          min_led_driver_brightness = LEDManagement::get_ledDriver_brightness();
          min_underglow_brightness = LEDManagement::get_underglow_brightness();
          flag = false;
        }

        if (!max_reached)
        {
          underglow_brightness = std::min(top_ug_brightness_level, underglow_brightness + 0.014f);
          led_driver_brightness = std::min(top_brightness_level, led_driver_brightness + 0.06f);

          if (led_driver_brightness >= top_brightness_level && underglow_brightness >= top_ug_brightness_level)
          {
            max_reached = true;
          }
          LEDManagement::set_ledDriver_brightness(led_driver_brightness);
          LEDManagement::set_underglow_brightness(underglow_brightness);
          LEDManagement::set_updated(true);
        }
        else
        {
          if (led_driver_brightness <= min_led_driver_brightness)
          {
            reached_bl_brightness = true;
          }
          if (underglow_brightness <= min_underglow_brightness)
          {
            reached_ug_brightness = true;
          }
          if (reached_ug_brightness && reached_bl_brightness)
          {
            base_settings.delay_ms = 0;
            reached_ug_brightness = reached_bl_brightness = false;
            LEDManagement::onDismount(LEDManagement::LedBrightnessControlEffect::FADE_EFFECT);
          }
          if (!(reached_ug_brightness && reached_bl_brightness))
          {
            led_driver_brightness = std::max(min_led_driver_brightness, led_driver_brightness - 0.01002f);
            underglow_brightness = std::max(min_underglow_brightness, underglow_brightness - 0.00505f);
          }
          LEDManagement::set_ledDriver_brightness(led_driver_brightness);
          LEDManagement::set_underglow_brightness(underglow_brightness);
          LEDManagement::set_updated(true);
        }
      }
      else if (layer == 0)
      {
        DBG_PRINTF_TRACE("LAYER == 0**************************");
        flag = true;
        max_reached = false;
        reached_ug_brightness = reached_bl_brightness = false;
        base_settings.delay_ms = 0;
        LEDManagement::onDismount(LEDManagement::LedBrightnessControlEffect::FADE_EFFECT);
      }
    }
    else
    {
      base_settings.delay_ms = 0;
    }
  }

#endif
  uint8_t layer;
  uint8_t fade_is_on;

private:
};

static LedModeSerializable_Layer ledModeSerializableLayer{CRC32_STR("LedModeSerializable_Layer")};