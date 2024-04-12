/* LedModeSerializable_SolidColor - LED Solid Color effect that is then serialized to the keyscanner when executed.
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
#define FADE_DURATION 9000
#include <LEDManagement.hpp>
#include <Keyscanner.hpp>
#include <array>
#include <functional>
#endif

#ifdef KEYSCANNER
const int NUM_LEDS = NUMBER_OF_LEDS;
uint8_t ledState[NUMBER_OF_LEDS] = {0}; // Estado de cada LED (0 = apagado, 1 = encendido)
uint64_t keypress_leds[NUMBER_OF_LEDS] = {0};
uint16_t rainbowHue[NUMBER_OF_LEDS] = {0};
float fade_factor[NUMBER_OF_LEDS] = {0};
#endif

class LedModeSerializable_Stalker : public LedModeSerializable
{
public:
  explicit LedModeSerializable_Stalker(uint32_t id)
      : LedModeSerializable(id)
  {
  }

  uint8_t serialize(uint8_t *output) const override
  {
    uint8_t index = LedModeSerializable::serialize(output);
    output[++index] = r_;
    output[++index] = g_;
    output[++index] = b_;
    output[++index] = w_;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override
  {
    uint8_t index = LedModeSerializable::deSerialize(input);
    r_ = input[++index + 1];
    g_ = input[++index + 2];
    b_ = input[++index + 3];
    w_ = input[++index + 4];

    base_settings.delay_ms = 50;
    return index;
  }

#ifdef KEYSCANNER

  inline static std::array<std::array<uint8_t, Pins::COLS>, Pins::ROWS> map{};

  // Función para encender un LED
  void turnOnLED(int index)
  {
    ledState[index] = 1;
  }

  void resetLEDState()
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      ledState[i] = 0;
    }
  }

  RGBW calculateRainbowColor(uint8_t index)
  {
    // Determinar el valor base de matiz (hue) para el arco iris
    uint8_t baseHue = rainbowHue[index] % 255;

    // Calcular el paso entre LEDs basado en la cantidad de LEDs y el multiplicador
    uint8_t hueStep = 255 / NUMBER_OF_LEDS;
    uint8_t hueOffset = (gpio_get(25)) ? hueStep : 0;

    // Iterar sobre cada LED y establecer su color correspondiente
    // Calcular el valor de matiz (hue) para el LED actual con un desplazamiento
    uint8_t ledHue = baseHue + (index * hueStep) + hueOffset;

    // Ajustar el valor de matiz (hue) para mantenerlo dentro del rango [0, 255]
    ledHue %= 255;

    // Convertir el valor de matiz (hue) en un color RGB
    // Asegurarse de que el valor de matiz (hue) esté en el rango correcto
    if (rainbowHue[index] >= 255)
    {
      rainbowHue[index] -= 255;
    }

    // Convertir el valor de matriz (hue) directamente en un color RGB
    RGBW rainbow;
    switch (rainbowHue[index] / 43)
    {
    case 0:
      rainbow.r = 255;
      rainbow.g = rainbowHue[index] * 6;
      rainbow.b = 0;
      break;
    case 1:
      rainbow.r = 255 - (rainbowHue[index] - 43) * 6;
      rainbow.g = 255;
      rainbow.b = 0;
      break;
    case 2:
      rainbow.r = 0;
      rainbow.g = 255;
      rainbow.b = (rainbowHue[index] - 86) * 6;
      break;
    case 3:
      rainbow.r = 0;
      rainbow.g = 255 - (rainbowHue[index] - 129) * 6;
      rainbow.b = 255;
      break;
    case 4:
      rainbow.r = (rainbowHue[index] - 172) * 6;
      rainbow.g = 0;
      rainbow.b = 255;
      break;
    default:
      rainbow.r = 255;
      rainbow.g = 0;
      rainbow.b = 255 - (rainbowHue[index] - 215) * 6;
      break;
    }
    // Apagar el componente blanco
    rainbow.w = 0;

    // Incrementar el valor de matiz (hue) base para el próximo ciclo
    rainbowHue[index]++;

    // Ajustar el valor de matiz (hue) base para mantenerlo dentro del rango [0, 255]
    rainbowHue[index] %= 255;

    return rainbow;
  }

  void fadeOffEffect(RGBW &color, uint64_t &time, uint8_t index)
  {

    auto elapsedTime = static_cast<float>(time);

    fade_factor[index] = 1.0f - (elapsedTime / FADE_DURATION);

    // Asegura que el factor de atenuación esté dentro del rango [0, 1]
    fade_factor[index] = std::max(0.0f, std::min(fade_factor[index], 1.0f));

    // Atenúa el color de forma lineal usando el factor de atenuación
    color.r = static_cast<uint8_t>(color.r * fade_factor[index]);
    color.g = static_cast<uint8_t>(color.g * fade_factor[index]);
    color.b = static_cast<uint8_t>(color.b * fade_factor[index]);
    color.w = static_cast<uint8_t>(color.w * fade_factor[index]);
  }

  RGBW calculateRainbowColorWithFade(uint8_t index, uint64_t &time)
  {
    // Calcula el color del arco iris para el índice dado
    RGBW rainbowColor = calculateRainbowColor(index);

    // Aplica el efecto de desvanecimiento gradual al color del arco iris
    fadeOffEffect(rainbowColor, time, index);

    return rainbowColor;
  }

  // Función para actualizar el estado de los LEDs
  void updateLEDs()
  {
    // Obtiene el tiempo actualk,nkm,l.hjñ{oiy
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());

    // k Itera sobre cada LED
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
      // Si la tecla correspondiente ha sido presionada y su tiempo no ha expirado
      if (ledState[i] == 1 && (currentTime - keypress_leds[i]) < FADE_DURATION)
      {
        // Calcula el tiempo transcurrido desde que se presionó la tecla
        uint64_t elapsedTime = currentTime - keypress_leds[i];

        // Calcula el color del arcoíris atenuado
        RGBW color = calculateRainbowColorWithFade(i, elapsedTime);

        // Actualiza el LED con el color calculado
        LEDManagement::set_led_at(color, i);
        // Indicar que los LEDs han sido actualizadosjkm
        LEDManagement::set_updated(true);
      }
      else if ((currentTime - keypress_leds[i]) >= FADE_DURATION)
      {
        RGBW off = {0, 0, 0, 0};
        keypress_leds[i] = 0;
        fade_factor[i] = 0;
        rainbowHue[i] = 0;
        ledState[i] = 0;
        LEDManagement::set_led_at(off, i);
        LEDManagement::set_updated(true);
      }
    }
  }

  void handleKeyPress(int keyIndex)
  {
    // Enciende el LED correspondiente al índice de tecla presionada
    turnOnLED(keyIndex);

    // Guarda el tiempo en que se presionó la tecla
    keypress_leds[keyIndex] = to_ms_since_boot(get_absolute_time());

    // Establece la bandera de tecla presionada en verdadero
  }

  void update() override
  {
    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    static uint32_t last_time = currentTime;

    uint8_t data[Pins::ROWS]{};
    KeyScanner.readMatrix(data);
    for (int i = 0; i < Pins::ROWS; ++i)
    {
      uint8_t rows = data[i];
      for (int j = 0; j < Pins::COLS; ++j)
      {
        if (rows >> j & 1)
        {
          map[i][j] = 0xFF;
          uint8_t led_position;
          if (gpio_get(Pins::SIDE_ID))
          {
            led_position = pos_right[i][j];
            DBG_PRINTF_TRACE("Key pressed");
          }
          else
          {
            led_position = pos_left[i][j];
          }
          handleKeyPress(led_position);
        }
      }
    }

    // Actualiza el estado de los LEDs
    if (currentTime - last_time > 10)
    {
      updateLEDs();
      last_time = currentTime;
    }
  }
#else

  void update() override
  {
  }

#endif
  uint8_t r_;
  uint8_t g_;
  uint8_t b_;
  uint8_t w_;

private:
#ifdef KEYSCANNER
  uint8_t pos_left[Pins::ROWS][Pins::COLS] = {
      {0, 1, 2, 3, 4, 5, 6},
      {7, 8, 9, 10, 11, 12},
      {13, 14, 15, 16, 17, 18},
      {19, 20, 21, 22, 23, 24, 25},
      {26, 27, 28, 29, 30, 31, 32, 33},
  };

  uint8_t pos_right[Pins::ROWS][Pins::COLS] = {
      {0, 1, 2, 3, 4, 5, 6},
      {15, 8, 9, 10, 11, 12, 13, 14},
      {7, 16, 17, 18, 19, 20, 21},
      {22, 23, 24, 25, 26, 27},
      {28, 29, 30, 31, 32, 33, 34, 35},
  };
#endif
};

static LedModeSerializable_Stalker ledModeSerializableStalker{CRC32_STR("LedModeSerializable_Stalker")};
