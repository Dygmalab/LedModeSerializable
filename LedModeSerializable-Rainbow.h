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

    void update() override
    {
        // Incrementar el valor de matiz (hue) para el arco iris
        rainbowHue++;

        // Asegurarse de que el valor de matiz (hue) esté en el rango correcto
        if (rainbowHue >= 255)
        {
            rainbowHue -= 255;
        }

        // Convertir el valor de matiz (hue) directamente en un color RGB
        RGBW rainbow;
        switch (rainbowHue / 43)
        {
        case 0:
            rainbow.r = 255;
            rainbow.g = rainbowHue * 6;
            rainbow.b = 0;
            break;
        case 1:
            rainbow.r = 255 - (rainbowHue - 43) * 6;
            rainbow.g = 255;
            rainbow.b = 0;
            break;
        case 2:
            rainbow.r = 0;
            rainbow.g = 255;
            rainbow.b = (rainbowHue - 86) * 6;
            break;
        case 3:
            rainbow.r = 0;
            rainbow.g = 255 - (rainbowHue - 129) * 6;
            rainbow.b = 255;
            break;
        case 4:
            rainbow.r = (rainbowHue - 172) * 6;
            rainbow.g = 0;
            rainbow.b = 255;
            break;
        default:
            rainbow.r = 255;
            rainbow.g = 0;
            rainbow.b = 255 - (rainbowHue - 215) * 6;
            break;
        }

        // Establecer la luminosidad y apagar el componente blanco
        rainbow.w = 0;

        // Actualizar los LEDs con el color arco iris calculado
        LEDManagement::set_all_leds(rainbow);
        LEDManagement::set_updated(true);
    }

private:
};

static LedModeSerializable_Rainbow ledModeSerializableRainbow{CRC32_STR("LedModeSerializable_Rainbow")};