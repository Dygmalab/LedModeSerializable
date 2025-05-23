/* LedModeSerializable_BluetoothPairing - LED bluetooth pairing effect that is then serialized to the keyscanner when executed.
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
#include "debug_print.h"
#include <vector>
#include "LedModeSerializable-Breathe.h"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_BluetoothPairing : public LedModeSerializable
{
public:
  explicit LedModeSerializable_BluetoothPairing(uint32_t id)
      : LedModeSerializable(id)
  {
  }
  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    output[index] = paired_channels_;
    output[++index] = connected_channel_id_;
    output[++index] = advertising_id;
    output[++index] = defy_id_side;
    output[++index] = erease_done;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    paired_channels_ = input[index];
    connected_channel_id_ = input[++index];
    advertising_id = input[++index];
    defy_id_side = input[++index];
    erease_done = input[++index];
    base_settings.delay_ms = 40;
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
    for (int i = 4; i >= 0; i--)
    {                                         // Iterate through each bit
      bool bit = (paired_channels_ >> i) & 1; // Read the bit at position i using shift and AND

      if (bit)
      {
        key_color[i + 1] = white;
        is_paired[i + 1] = 1;
      }
      else
      {
        key_color[i + 1] = blue;
        is_paired[i + 1] = 0;
      }
    }
    if (KsConfig::get_side())
    { // Right side
      LEDManagement::set_led_at(yellow, 6);
      for (int i = 5; i >= 1; --i)
      {

        LEDManagement::set_led_at(key_color[6 - i], i);
        if (is_paired[6 - i] == 1)
        {
          LEDManagement::set_led_at(red, i + 7);
        }
        else
        {
          LEDManagement::set_led_at(ledOff, i + 7);
        }
      }
      if (connected_channel_id_ != NOT_CONNECTED && connected_channel_id_ < 5)
      {
        LEDManagement::set_led_at(green, 4 - connected_channel_id_ + 1);
        LEDManagement::set_led_at(red, 4 - connected_channel_id_ + 8);
      }
      if (advertising_id != NOT_ON_ADVERTISING)
      {
        breathe(4 - advertising_id);
        setUnderglowLEDS(KsConfig::UG_LEDS_RIGHT);
      }
    }
    else
    { // Left side
      for (uint8_t i = 1; i < 6; ++i)
      {
        LEDManagement::set_led_at(yellow, 0);
        LEDManagement::set_led_at(key_color[i], i);
        if (is_paired[i] == 1)
        {
          LEDManagement::set_led_at(red, i + 7);
        }
        else
        {
          LEDManagement::set_led_at(ledOff, i + 7);
        }
      }

      if (connected_channel_id_ != NOT_CONNECTED && connected_channel_id_ < 5)
      {
        LEDManagement::set_led_at(green, connected_channel_id_ + 1);
        LEDManagement::set_led_at(red, connected_channel_id_ + 8);
      }
      if (advertising_id != NOT_ON_ADVERTISING)
      {
        breathe(advertising_id);
        setUnderglowLEDS(KsConfig::UG_LEDS_LEFT);
      }
    }
    if (erease_done)
    {
      ereaseLedEffect();
    }
    LEDManagement::set_updated(true);
  }

  void ereaseLedEffect()
  {
    static uint32_t lastExecutionTime = 0;
    static uint8_t counter = 0;
    static bool ledIsOn = false;

    if (counter >= 2)
    {
      counter = 0;
      erease_done = false;
      LEDManagement::set_all_leds(ledOff, true);
      ledIsOn = false; // Reset LED state
      return;          // Terminate early if counter has reached its max
    }

    uint32_t currentTime = hal_mcu_systim_ms_get(hal_mcu_systim_counter_get());
    if (currentTime - lastExecutionTime >= 1000)
    { // Wait for 1000ms
      if (ledIsOn)
      {
        LEDManagement::set_all_leds(ledOff , true);
      }
      else
      {
        LEDManagement::set_all_leds(blue,true);
      }
      ledIsOn = !ledIsOn; // Toggle LED state
      lastExecutionTime = currentTime;
      counter++;
    }
  }

    void setUnderglowLEDS( uint8_t ug_leds)
    {
      const uint8_t MAX_UG_LEDS = ug_leds;
      const uint8_t MAX_LED_ID = MAX_UG_LEDS;
      const uint8_t WRAP_AROUND_LED_ID = 0;

      // Apagar todos los LEDs primero
      for (uint8_t i = 0; i < MAX_UG_LEDS; ++i)
      {
        LEDManagement::set_ug_at(ledOff, i);
      }

      // Encender el LED actual y el siguiente
      LEDManagement::set_ug_at(blue, underglow_led_id);

      if (underglow_led_id + 1 > MAX_LED_ID)
      {
        LEDManagement::set_ug_at(blue, WRAP_AROUND_LED_ID);
      }
      else
      {
        LEDManagement::set_ug_at(blue, underglow_led_id + 1);
      }

      // Incrementar el valor de underglow_led_id
      underglow_led_id++;
      if (underglow_led_id > MAX_LED_ID)
      {
        underglow_led_id = 0;
      }
    }

  void breathe(uint8_t channel_id)
  {
    uint8_t i = ((uint16_t)hal_mcu_systim_ms_get(hal_mcu_systim_counter_get())) >> 3;

    if (i & 0x80)
    {
      i = 255 - i;
    }

    i = i << 1;
    uint8_t ii = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

    RGBW breathe = LEDManagement::HSVtoRGB(160, 255, i);
    breathe.w = 0;

    LEDManagement::set_led_at(breathe, channel_id + 1);
  }
#endif



  uint8_t paired_channels_;
  uint8_t connected_channel_id_;
  uint8_t advertising_id;
  uint8_t erease_done;
  uint8_t defy_id_side;

private:
#ifdef KEYSCANNER
  static constexpr RGBW white = {0, 0, 0, 255};
  static constexpr RGBW green = {0, 255, 0, 0};
  static constexpr RGBW blue = {0, 0, 255, 0};
  static constexpr RGBW red = {255, 0, 0, 0};
  static constexpr RGBW yellow = {150, 150, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  enum Channels : uint8_t
  {
    NOT_CONNECTED = 5,
    NOT_ON_ADVERTISING
  };
  std::vector<RGBW> key_color{5};
  std::vector<uint8_t> is_paired{5};
  uint8_t underglow_led_id = 0;
#endif
};

static LedModeSerializable_BluetoothPairing
    ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};
