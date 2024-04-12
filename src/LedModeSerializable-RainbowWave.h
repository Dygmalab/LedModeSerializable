#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

class LedModeSerializable_RainbowWave : public LedModeSerializable {
 public:
  explicit LedModeSerializable_RainbowWave(uint32_t id)
    : LedModeSerializable(id) {
  }

  void update() override {
    // Determinar el valor base de matiz (hue) para el arco iris
    uint8_t baseHue = rainbowHue % 255;

    // Calcular el paso entre LEDs basado en la cantidad de LEDs y el multiplicador
    uint8_t hueStep = 255 / NUMBER_OF_LEDS;
    uint8_t hueOffset = (gpio_get(25)) ? hueStep : 0;

    // Iterar sobre cada LED y establecer su color correspondiente
    for (uint8_t i = 0; i < NUMBER_OF_LEDS; i++) {
      // Calcular el valor de matiz (hue) para el LED actual con un desplazamiento
      uint8_t ledHue = baseHue + (i * hueStep) + hueOffset;

      // Ajustar el valor de matiz (hue) para mantenerlo dentro del rango [0, 255]
      ledHue %= 255;

      // Convertir el valor de matiz (hue) en un color RGB
      // Asegurarse de que el valor de matiz (hue) esté en el rango correcto
      if (rainbowHue >= 255) {
        rainbowHue -= 255;
      }

      // Convertir el valor de matriz (hue) directamente en un color RGB
      RGBW rainbow;
      switch (rainbowHue / 43) {
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

      // Apagar el componente blanco
      rainbow.w = 0;

      // Establecer el color del LED actual
      LEDManagement::set_led_at(rainbow, i);
    }

    // Incrementar el valor de matiz (hue) base para el próximo ciclo
    rainbowHue++;

    // Ajustar el valor de matiz (hue) base para mantenerlo dentro del rango [0, 255]
    rainbowHue %= 255;

    // Indicar que los LEDs han sido actualizados
    LEDManagement::set_updated(true);
  }

 private:
  uint16_t rainbowHue        = 0;
};

static LedModeSerializable_RainbowWave ledModeSerializableRainbowWave{CRC32_STR("LedModeSerializable_RainbowWave")};