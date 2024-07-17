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
class LedModeSerializable_RainbowWave : public LedModeSerializable
{
public:
    explicit LedModeSerializable_RainbowWave(uint32_t id)
            : LedModeSerializable(id)
    {
    }

#ifdef KEYSCANNER

    void update() override
    {
        // Determinar el valor base de matiz (hue) para el arco iris
        uint8_t baseHue = rainbowHue % 255;

        // Calcular el paso entre filas basado en la cantidad de filas
        uint8_t hueStep = 255 / 6; // Asegúrate de definir NUMBER_OF_ROWS

        // Número de LEDs por fila
        uint8_t ledsPerRow = 8; // Asegúrate de definir LEDS_PER_ROW

        // Iterar sobre cada fila
        for (uint8_t row = 0; row < 6; row++)
        {
            // Calcular el valor de matiz (hue) para la fila actual
            uint8_t rowHue = (baseHue + (row * hueStep)) % 255;

            // Iterar sobre cada LED en la fila y establecer su color correspondiente
            for (uint8_t i = 0; i < ledsPerRow; i++)
            {
                // Convertir el valor de matiz (hue) en un color RGB
                RGBW rainbow;
                switch (rowHue / 43)
                {
                    case 0:
                        rainbow.r = 255;
                        rainbow.g = rowHue * 6;
                        rainbow.b = 0;
                        break;
                    case 1:
                        rainbow.r = 255 - (rowHue - 43) * 6;
                        rainbow.g = 255;
                        rainbow.b = 0;
                        break;
                    case 2:
                        rainbow.r = 0;
                        rainbow.g = 255;
                        rainbow.b = (rowHue - 86) * 6;
                        break;
                    case 3:
                        rainbow.r = 0;
                        rainbow.g = 255 - (rowHue - 129) * 6;
                        rainbow.b = 255;
                        break;
                    case 4:
                        rainbow.r = (rowHue - 172) * 6;
                        rainbow.g = 0;
                        rainbow.b = 255;
                        break;
                    default:
                        rainbow.r = 255;
                        rainbow.g = 0;
                        rainbow.b = 255 - (rowHue - 215) * 6;
                        break;
                }

                // Apagar el componente blanco
                rainbow.w = 0;

                // Calcular el índice del LED en la matriz
                uint16_t ledIndex = row * ledsPerRow + i;

                // Establecer el color del LED actual
                LEDManagement::set_led_at(rainbow, ledIndex);
            }
        }

        // Incrementar el valor de matiz (hue) base para el próximo ciclo
        rainbowHue++;

        // Ajustar el valor de matiz (hue) base para mantenerlo dentro del rango [0, 255]
        rainbowHue %= 255;

        // Indicar que los LEDs han sido actualizados
        LEDManagement::set_updated(true);
    }

#endif

private:
    uint16_t rainbowHue = 0;
};

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};

