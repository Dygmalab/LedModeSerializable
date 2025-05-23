/* LedModeSerializable_RainbowWave - LED Rainbow wave effect that is then serialized to the keyscanner when executed.
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
#endif

class LedModeSerializable_RainbowWave : public LedModeSerializable
{
public:
    explicit LedModeSerializable_RainbowWave(uint32_t id)
            : LedModeSerializable(id), rainbowHue(0)
    {
    }

    uint8_t deSerialize(const uint8_t *input) override
    {
        uint8_t index = LedModeSerializable::deSerialize(input);
        base_settings.delay_ms = 51;

        return ++index;
    }

#ifdef KEYSCANNER

    void update() override
    {
        // Ensure the hue value is within the correct range
        if (rainbowHue >= 255)
        {
            rainbowHue -= 255;
        }

        // Determine the base hue value for the rainbow
        uint8_t baseHue = rainbowHue % 255;

        // Calculate the step between rows based on the number of rows
        uint8_t hueStep = 255 / KsConfig::NUM_ROWS;

        // Precompute the hue values for each row
        uint8_t rowHues[KsConfig::NUM_ROWS];
        for (uint8_t row = 0; row < KsConfig::NUM_ROWS; ++row)
        {
            rowHues[row] = (baseHue + row * hueStep) % 255;
        }

        if (KsConfig::get_side() == KsConfig::Side::RIGHT)
        {
            p_rainbow_col = &KsConfig::NUM_COLS_RIGHT[0];
        }
        else
        {
            p_rainbow_col = &KsConfig::NUM_COLS_LEFT[0];
        }

        // Iterate over each row
        for (uint8_t row = 0; row < KsConfig::NUM_ROWS; ++row)
        {
            RGBW rainbow = calculateRGBWFromHue(rowHues[row]);
            for (uint8_t i = 0; i < p_rainbow_col[row]; ++i)
            {

                setLEDColor(row, i, p_rainbow_col, rainbow);
            }
        }

        // Increment the base hue value for the next cycle
        rainbowHue++;

        update_underglow_leds();

        LEDManagement::set_updated(true);
    }

#endif

private:
    uint16_t rainbowHue = 0;
#ifdef KEYSCANNER
    uint8_t rowHues[KsConfig::NUM_ROWS];

    const uint8_t *p_rainbow_col;

    static RGBW calculateRGBWFromHue(uint8_t hue)
    {
        return LEDManagement::hueTable[hue];
    }

    void setLEDColor(uint8_t row, uint8_t col, const uint8_t NUM_COLS[], RGBW color) const
    {
        // Calculate the LED index in the LED array
        uint8_t ledIndex = col;
        for (uint8_t j = 0; j < row; ++j)
        {
            ledIndex += p_rainbow_col[j];
        }
        LEDManagement::set_led_at(color, ledIndex);
    }

    void update_underglow_leds() const
    {
        uint8_t baseHue = rainbowHue % 255;
        const uint8_t NUM_LEDS = 53;

        for (uint8_t i = 0; i < KsConfig::MAX_UG_LEDS; ++i)
        {
            uint8_t hue = (baseHue + ((i - KsConfig::MAX_BL_LEDS) * 256 / NUM_LEDS)) % 255;
            RGBW color = calculateRGBWFromHue(hue);
            LEDManagement::set_ug_at(color, i);
        }
    }
#endif
};

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};
