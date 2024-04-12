#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_BatteryStatus : public LedModeSerializable {
 public:
  explicit LedModeSerializable_BatteryStatus(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index = LedModeSerializable::serialize(output);
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index = LedModeSerializable::deSerialize(input);
    base_settings.delay_ms = 10;
    return ++index;
  }
#ifdef NEURON_WIRED
  void update() override {
  }
#endif

#ifdef KEYSCANNER

  void update() override {
    const uint8_t batteryLevel = BatteryManagement::getBatteryLevel();
    const BatteryManagement::BatteryStatus batteryStatus = BatteryManagement::getBatteryStatus();

    uint16_t current_time = (uint16_t)to_ms_since_boot(get_absolute_time());
    static uint16_t last_execution_time = 0;

    switch (batteryStatus) {
    case BatteryManagement::CHARGING_DONE:
      setLedState(green, green, green);
      break;
    case BatteryManagement::CHARGING:
      static enum {
        FIRST_CELL,
        SECOND_CELL,
        THIRD_CELL,
        NO_CELL,
      } currentCell = NO_CELL;
      if (current_time - last_execution_time > charging_time_led_effect){
        last_execution_time = current_time;
        switch (currentCell) {
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
      break;
    case BatteryManagement::NOT_CHARGHING:
      if (batteryLevel > 70) {
        setLedState(green, green, green);
      } else if (batteryLevel > 40) {
        setLedState(ledOff, green, green);
      } else if (batteryLevel > 10) {
        setLedState(ledOff, ledOff, green);
      } else {
        breathe(thirdCellPosition);
      }
      break;
    case BatteryManagement::FAULT:
    case BatteryManagement::ERROR:
      break;
    }
  }

 private:
  static inline RGBW firstCell = { 0, 0, 0, 0 };
  static inline RGBW secondCell = { 0, 0, 0, 0 };
  static inline RGBW thirdCell = { 0, 0, 0, 0 };

  static constexpr RGBW green = { 0, 255, 0, 0 };
  static constexpr RGBW red = { 255, 0, 0, 0 };
  static constexpr RGBW ledOff = { 0, 0, 0, 0 };

  static constexpr uint8_t thirdCellPosition = 20;
  static constexpr uint8_t charging_time_led_effect = 160;
  static void setLedState(const RGBW& first, const RGBW& second, const RGBW& third) {
    firstCell = first;
    secondCell = second;
    thirdCell = third;
    updateLedEffect();
  }

  static void updateLedEffect() {
    LEDManagement::set_led_at(firstCell, 6);
    LEDManagement::set_led_at(secondCell, 13);
    LEDManagement::set_led_at(thirdCell, thirdCellPosition);
    LEDManagement::set_updated(true);
  }

  static void breathe(uint8_t cellPosition) {
      uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 3;

      if (i & 0x80) {
        i = 255 - i;
      }

      i           = i << 1;
      uint8_t ii  = (i * i) >> 8;
      uint8_t iii = (ii * i) >> 8;

      i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

      RGBW breathe = LEDManagement::HSVtoRGB(0, 255, i);
      breathe.w    = 0;

      LEDManagement::set_led_at(breathe, cellPosition);
    }
#endif
};

static LedModeSerializable_BatteryStatus ledModeSerializableBatteryStatus{CRC32_STR("LedModeSerializable_BatteryStatus")};
