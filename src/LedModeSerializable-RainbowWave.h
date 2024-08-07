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
#include "LEDManagement.hpp"

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
        uint8_t hueStep = 255 / Pins::NUM_ROWS;

        // Precompute the hue values for each row
        uint8_t rowHues[Pins::NUM_ROWS];

        for (uint8_t row = 0; row < Pins::NUM_ROWS; ++row)
        {
            rowHues[row] = (baseHue + row * hueStep) % 255;
        }

        // Iterate over each row
        for (uint8_t row = 0; row < Pins::NUM_ROWS; ++row)
        {
            RGBW rainbow = calculateRGBWFromHue(rowHues[row]);
            for (uint8_t i = 0; i < Pins::NUM_COLS[row]; ++i)
            {
                setLEDColor(row, i, Pins::NUM_COLS, rainbow);
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

    RGBW calculateRGBWFromHue(uint8_t hue) {
        RGBW rainbow;
        uint8_t region = hue / 43;
        uint8_t remainder = (hue - (region * 43)) * 6;

        switch (region)
        {
            case 0:
                rainbow.r = 255;
                rainbow.g = remainder;
                rainbow.b = 0;
                break;
            case 1:
                rainbow.r = 255 - remainder;
                rainbow.g = 255;
                rainbow.b = 0;
                break;
            case 2:
                rainbow.r = 0;
                rainbow.g = 255;
                rainbow.b = remainder;
                break;
            case 3:
                rainbow.r = 0;
                rainbow.g = 255 - remainder;
                rainbow.b = 255;
                break;
            case 4:
                rainbow.r = remainder;
                rainbow.g = 0;
                rainbow.b = 255;
                break;
            default:
                rainbow.r = 255;
                rainbow.g = 0;
                rainbow.b = 255 - remainder;
                break;
        }
        rainbow.w = 0; // Turn off the white component

        return rainbow;
    }

    void setLEDColor(uint8_t row, uint8_t col, const uint8_t NUM_COLS[], RGBW color) {
        // Calculate the LED index in the LED array
        uint8_t ledIndex = col;
        for (uint8_t j = 0; j < row; ++j)
        {
            ledIndex += NUM_COLS[j];
        }
        LEDManagement::set_led_at(color, ledIndex);
    }

    void update_underglow_leds()
    {
        // Determine the base hue value for the rainbow
        uint8_t baseHue = rainbowHue % 255;

        // Number of LEDs in the strip
        const uint8_t NUM_LEDS = 53;

        // Iterate over each LED
        for (uint8_t i = 0; i < Pins::MAX_UG_LEDS; ++i)
        {
            // Calculate the rainbow color based on the LED position and the base hue
            uint8_t hue = (baseHue + ((i - Pins::MAX_BL_LEDS) * 256 / NUM_LEDS)) % 255;
            RGBW color = calculateRGBWFromHue(hue);
            LEDManagement::set_ug_at(color, i);
        }

    }
};

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};
