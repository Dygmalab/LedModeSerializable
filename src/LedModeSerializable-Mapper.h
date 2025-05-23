#ifndef KEYSCANNER_LEDMODESERIALIZABLE_MAPPER_H
#define KEYSCANNER_LEDMODESERIALIZABLE_MAPPER_H
/* LedModeSerializable_Mapper - LED Breathe effect that is then serialized to the keyscanner when executed.
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
#include "debug_print.h"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#endif

#define TIME_INTERVAL 500

class LedModeSerializable_Mapper : public LedModeSerializable
{
public:
    uint8_t pos = {0}; // Variable para almacenar la posición

    explicit LedModeSerializable_Mapper(uint32_t id)
            : LedModeSerializable(id) {}

    uint8_t serialize(uint8_t *output) const override
    {
        uint8_t index = LedModeSerializable::serialize(output);
        return ++index;
    }

#ifdef KEYSCANNER

    uint8_t deSerialize(const uint8_t *input) override
    {
        uint8_t index = LedModeSerializable::deSerialize(input);
        return ++index;
    }

    void update() override
    {
        static unsigned long previousMillis = 0;

        RGBW color = {0,0,0,255};
        if (pos <= KsConfig::BL_LEDS_RIGHT)
        {
            uint32_t currentMillis = hal_mcu_systim_ms_get(hal_mcu_systim_counter_get());

            if (currentMillis - previousMillis >= TIME_INTERVAL)
            {
                previousMillis = currentMillis;
                LEDManagement::set_led_at(color, pos);
                DBG_PRINTF_TRACE("pos: %i", pos);
                if (LedModeSerializable_Mapper::key_pressed)
                {
                    uint8_t aux_pos = pos;
                    if (pos!=0)
                    {
                        aux_pos = pos - 1;
                    }
                    LedModeSerializable_Mapper::positions[counter] = aux_pos;
                    LedModeSerializable_Mapper::counter++;
                    for (int i = 0; i < counter; ++i)
                    {
                        DBG_PRINTF_TRACE("led mapping POS: %i VALUE = [ %i ]", i, LedModeSerializable_Mapper::positions[i]);
                    }
                    LedModeSerializable_Mapper::key_pressed = false;
                }
                pos++; // Avanzar a la siguiente posición
                LEDManagement::set_updated(true);
            }

        }
        else
        {
            pos = 0;
            LEDManagement::set_all_leds({},true);
            LEDManagement::set_updated(true);
        }
    }
#else
    void update() override
  {
  }
#endif

    inline  static uint8_t key_pressed{false};

    uint8_t counter = 0;
    uint8_t positions[KsConfig::MAX_BL_LEDS] = {0};

private:
};

static LedModeSerializable_Mapper ledModeSerializableMapper{CRC32_STR("LedModeSerializable_Mapper")};

#endif //KEYSCANNER_LEDMODESERIALIZABLE_MAPPER_H
