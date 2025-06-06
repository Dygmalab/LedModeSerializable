/* LedModeSerializable_Rainbow - LED Rainbow effect that is then serialized to the keyscanner when executed.
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

class LedModeSerializable_Rainbow : public LedModeSerializable
{
public:
    explicit LedModeSerializable_Rainbow(uint32_t id)
        : LedModeSerializable(id)
    {
    }

#ifdef KEYSCANNER

    void update() override
    {

        rainbowHue = (rainbowHue + 1) % 255;

        RGBW rainbow = calculateRGBWFromHue(rainbowHue);
        rainbow.w = 0;

        LEDManagement::set_all_leds(rainbow);
        LEDManagement::set_updated(true);
    }
#endif

private:
#ifdef KEYSCANNER
    static RGBW calculateRGBWFromHue(uint8_t hue)
    {
        return LEDManagement::hueTable[hue];
    }
#endif
    uint8_t rainbowHue = 0;
};

static LedModeSerializable_Rainbow ledModeSerializableRainbow{CRC32_STR("LedModeSerializable_Rainbow")};