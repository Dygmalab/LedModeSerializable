/* LedModeSerializable_BatteryStatus - LED battery status highlight that serializes its content.
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
#include "BatteryManagement.hpp"
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
    const uint8_t batteryLevel = BatteryManagement::getBatteryPercentage();
    const BatteryManagement::BatteryStatus batteryStatus = BatteryManagement::getBatteryStatus();

    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    static uint32_t last_execution_time = 0;

    switch (batteryStatus)
    {
    case BatteryManagement::CHARGING_DONE:
    {
      setLedState(green, green, green);
    }
    break;

    case BatteryManagement::CHARGING:
    {
        static enum {
            FIRST_CELL,
            SECOND_CELL,
            THIRD_CELL,
            NO_CELL,
        } currentCell = NO_CELL;

        if (current_time - last_execution_time > charging_time_led_effect)
        {
            last_execution_time = current_time;
            switch (currentCell)
            {
                case NO_CELL:
                    setLedState(ledOff, ledOff, ledOff);
                    currentCell = THIRD_CELL;
                    break;
                case THIRD_CELL:
                    setLedState(ledOff, ledOff, green);
                    currentCell = SECOND_CELL;
                    break;
                case SECOND_CELL:
                    setLedState(ledOff, green, green);
                    currentCell = FIRST_CELL;
                    break;
                case FIRST_CELL:
                    setLedState(green, green, green);
                    currentCell = NO_CELL;
                    break;
            }
        }
    }
    break;

    case BatteryManagement::NOT_CHARGHING:
    {
      if (batteryLevel > 70)
      {
        setLedState(green, green, green);
      }
      else if (batteryLevel > 40)
      {
        setLedState(ledOff, green, green);
      }
      else if (batteryLevel > 20)
      {
        setLedState(ledOff, ledOff, green);
      }
      else
      {
          if(KsConfig::get_side() == KsConfig::Side::RIGHT)
          {
              breathe_at(thirdCellPosition,Pins::THIRD_CELL_POS_RIGHT);
          }
          else
          {
            breathe_at(thirdCellPosition,Pins::THIRD_CELL_POS_LEFT);
          }
      }
    }
    break;

    default:
    {
        //TODO: discuss with the team, if this is the correct way to handle the default case.
        //For now let's just comment it out.
/*        if(KsConfig::get_side() == KsConfig::Side::RIGHT)
        {
            //Right side
            breathe_at(thirdCellPosition,Pins::FIRST_CELL_POS_RIGHT);
            breathe_at(secondCellPosition,Pins::SECOND_CELL_POS_RIGHT);
            breathe_at(thirdCellPosition,Pins::THIRD_CELL_POS_RIGHT);
        }
        else
        {
            //Left side
            breathe_at(thirdCellPosition,Pins::FIRST_CELL_POS_LEFT);
            breathe_at(secondCellPosition,Pins::SECOND_CELL_POS_LEFT);
            breathe_at(thirdCellPosition,Pins::THIRD_CELL_POS_LEFT);
        }*/
    }
    break;
    }
  }

private:
  static inline RGBW firstCell = {0, 0, 0, 0};
  static inline RGBW secondCell = {0, 0, 0, 0};
  static inline RGBW thirdCell = {0, 0, 0, 0};

  static inline uint8_t firstCellPosition = 0;
  static inline uint8_t secondCellPosition = 0;
  static inline uint8_t thirdCellPosition = 0;

  static constexpr RGBW green = {0, 255, 0, 0};
  static constexpr RGBW red = {255, 0, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};

  static constexpr uint8_t charging_time_led_effect = 160;
  static void setLedState(const RGBW &first, const RGBW &second, const RGBW &third)
  {
    firstCell = first;
    secondCell = second;
    thirdCell = third;
    updateLedEffect();
  }

  static void updateLedEffect()
  {
      if(KsConfig::get_side() == KsConfig::Side::RIGHT)
      {
          //Right side
          LEDManagement::set_led_at(firstCell, Pins::FIRST_CELL_POS_RIGHT);
          LEDManagement::set_led_at(secondCell, Pins::SECOND_CELL_POS_RIGHT);
          LEDManagement::set_led_at(thirdCell, Pins::THIRD_CELL_POS_RIGHT);
      }
      else
      {
          //Lefts side
          LEDManagement::set_led_at(firstCell, Pins::FIRST_CELL_POS_LEFT);
          LEDManagement::set_led_at(secondCell, Pins::SECOND_CELL_POS_LEFT);
          LEDManagement::set_led_at(thirdCell, Pins::THIRD_CELL_POS_LEFT);
      }
    LEDManagement::set_updated(true, true);
  }

  static void breathe_at(uint8_t cellPosition, uint8_t cell)
  {
      if(KsConfig::get_side() == KsConfig::Side::RIGHT)
      {
          //Right side
          cellPosition = cell;
      }
      else
      {
          //Lefts side
          cellPosition = cell;
      }

      uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 4;

      if (i & 0x80)
      {
          i = 255 - i;
      }

      i = i << 1;
      uint8_t ii = (i * i) >> 8;
      uint8_t iii = (ii * i) >> 8;

      i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

      RGBW breathe = LEDManagement::HSVtoRGB(0, 255 , i);
      breathe.w = 0;
      printf("Breathe: %d %d %d %d\n", breathe.r, breathe.g, breathe.b, breathe.w);
      LEDManagement::set_led_at(breathe, cellPosition);
      LEDManagement::set_updated(true);
  }
#endif
};

static LedModeSerializable_BatteryStatus ledModeSerializableBatteryStatus{CRC32_STR("LedModeSerializable_BatteryStatus")};
