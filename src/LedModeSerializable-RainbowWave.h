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
        // Asegurarse de que el valor de matiz (hue) esté en el rango correcto
        if (rainbowHue >= 255)
        {
            rainbowHue -= 255;
        }

        // Determinar el valor base de matiz (hue) para el arco iris
        uint8_t baseHue = rainbowHue % 255;

        // Número de LEDs en cada fila
        const uint8_t NUM_ROWS = 6;
        const uint8_t NUM_COLS[NUM_ROWS] =  {7, 7, 7, 6, 4, 4};

        // Calcular el paso entre filas basado en la cantidad de filas
        uint8_t hueStep = 255 / NUM_ROWS;

        // Precomputar los valores de matiz (hue) para cada fila
        uint8_t rowHues[NUM_ROWS];

        for (uint8_t row = 0; row < NUM_ROWS; ++row)
        {
            rowHues[row] = (baseHue + row * hueStep) % 255;
        }

        // Iterar sobre cada fila
        for (uint8_t row = 0; row < NUM_ROWS; ++row)
        {
            RGBW rainbow = calculateRGBWFromHue(rowHues[row]);
            for (uint8_t i = 0; i < NUM_COLS[row]; ++i)
            {
                setLEDColor(row, i, NUM_COLS, rainbow);
            }
        }

        // Incrementar el valor de matiz (hue) base para el próximo ciclo
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
        rainbow.w = 0; // Apagar el componente blanco

        return rainbow;
    }

    void setLEDColor(uint8_t row, uint8_t col, const uint8_t NUM_COLS[], RGBW color) {
        // Calcular el índice del LED en la matriz de LEDs
        uint8_t ledIndex = col;
        for (uint8_t j = 0; j < row; ++j)
        {
            ledIndex += NUM_COLS[j];
        }
       LEDManagement::set_led_at(color, ledIndex);
    }

    void update_underglow_leds()
    {
        // Determinar el valor base de matiz (hue) para el arco iris
        uint8_t baseHue = rainbowHue % 255;

        // Número de LEDs en la tira
        const uint8_t NUM_LEDS = 53;

        // Iterar sobre cada LED
        for (uint8_t i = 0; i < Pins::MAX_UG_LEDS; ++i)
        {
            // Calcular el color del arco iris basado en la posición del LED y el matiz base
            uint8_t hue = (baseHue + ((i - Pins::MAX_BL_LEDS) * 256 / NUM_LEDS)) % 255;
            RGBW color = calculateRGBWFromHue(hue);
            LEDManagement::set_ug_at(color, i);
        }

    }
};

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};
