#pragma once

#include "LedModeSerializable.h"
#include "cstdio"

#ifdef KEYSCANNER
#include "LEDManagement.hpp"
#include "debug_print.h"
#include <vector>
#include "LedModeSerializable-Breathe.h"
#include "BatteryManagement.hpp"
#endif

class LedModeSerializable_BluetoothPairing : public LedModeSerializable {
 public:
  explicit LedModeSerializable_BluetoothPairing(uint32_t id)
    : LedModeSerializable(id) {
  }
  uint8_t serialize(uint8_t *output) const override {
    uint8_t index   = LedModeSerializable::serialize(output);
    output[index]   = paired_channels_;
    output[++index] = connected_channel_id_;
    output[++index] = advertising_id;
    output[++index] = defy_id_side;
    output[++index] = erease_done;
    return ++index;
  }

  uint8_t deSerialize(const uint8_t *input) override {
    uint8_t index          = LedModeSerializable::deSerialize(input);
    paired_channels_                     = input[index];
    connected_channel_id_                     = input[++index];
    advertising_id                     = input[++index];
    defy_id_side                     = input[++index];
    erease_done                     = input[++index];
    base_settings.delay_ms = 40;
    return ++index;
  }

#ifdef NEURON_WIRED
  void update() override {
  }
#endif

#ifdef KEYSCANNER
  void update() override {
    for (int i = 4; i >= 0; i--) { //Iterate through each bit
      bool bit = ( paired_channels_>> i) & 1; // Read the bit at position i using shift and AND

      if (bit) {
        key_color[i+1] = white;
        is_paired[i+1] = 1;
      } else {
        key_color[i+1] = blue;
        is_paired[i+1] = 0;
      }
    }
    if(gpio_get(25)){ //Right side
      LEDManagement::set_led_at(yellow, 6);
      for (int i = 5; i >= 1 ; --i) {

        LEDManagement::set_led_at(key_color[6-i], i);
        if (is_paired[6-i] == 1){
          LEDManagement::set_led_at(red, i+7);
        } else{
          LEDManagement::set_led_at(ledOff, i+7);
        }
      }
      if (connected_channel_id_ != NOT_CONNECTED && connected_channel_id_ < 5){
        LEDManagement::set_led_at(green, 4 - connected_channel_id_ + 1);
        LEDManagement::set_led_at(red, 4 - connected_channel_id_ + 8);
      }
      if(advertising_id != NOT_ON_ADVERTISING){
        breathe(4 - advertising_id);
        setUnderglowLEDS();
      }
    } else { //Left side
      for (uint8_t i = 1; i < 6 ; ++i) {
        LEDManagement::set_led_at(yellow, 0);
        LEDManagement::set_led_at(key_color[i], i);
        if (is_paired[i] == 1){
          LEDManagement::set_led_at(red, i+7);
        } else{
          LEDManagement::set_led_at(ledOff, i+7);
        }
      }

      if (connected_channel_id_ != NOT_CONNECTED && connected_channel_id_ < 5){
        LEDManagement::set_led_at(green, connected_channel_id_ + 1);
        LEDManagement::set_led_at(red, connected_channel_id_ + 8);
      }
      if(advertising_id != NOT_ON_ADVERTISING){
        breathe(advertising_id);
        setUnderglowLEDS();
      }
    }
    if (erease_done){
      ereaseLedEffect();
    }
    LEDManagement::set_updated(true);
  }

  void ereaseLedEffect() {
    static uint32_t lastExecutionTime = 0;
    static uint8_t counter = 0;
    static bool ledIsOn = false;

    if (counter >= 2) {
      counter = 0;
      erease_done = false;
      LEDManagement::set_all_leds(ledOff);
      ledIsOn = false; // Reset LED state
      return; // Terminate early if counter has reached its max
    }

    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
    if (currentTime - lastExecutionTime >= 1000) { // Wait for 1000ms
      if (ledIsOn) {
        LEDManagement::set_all_leds(ledOff);
      } else {
        LEDManagement::set_all_leds(blue);
      }
      ledIsOn = !ledIsOn; // Toggle LED state
      lastExecutionTime = currentTime;
      counter++;
    }
  }
#ifdef RAISE2
    void setUnderglowLEDS(){
      if(underglow_led_id > NUMBER_OF_LEDS){
        underglow_led_id = Pins::BL_LEDS_RIGHT;
      }
      LEDManagement::set_led_at(blue, underglow_led_id);

      if(underglow_led_id - 1 != Pins::BL_LEDS_RIGHT - 1){
        LEDManagement::set_led_at(ledOff, underglow_led_id - 1);
      }
      if (underglow_led_id + 1 > NUMBER_OF_LEDS){
        LEDManagement::set_led_at(blue, Pins::BL_LEDS_RIGHT);
      } else {
        LEDManagement::set_led_at(blue, underglow_led_id + 1);
      }
      underglow_led_id++;
    }
#else
  void setUnderglowLEDS(){
    if(underglow_led_id > 88){
      underglow_led_id = 35;
    }
    LEDManagement::set_led_at(blue, underglow_led_id);
    if(underglow_led_id - 1 != 34){
      LEDManagement::set_led_at(ledOff, underglow_led_id - 1);
    }
    if (underglow_led_id + 1 > 88){
      LEDManagement::set_led_at(blue, 35);
    } else {
      LEDManagement::set_led_at(blue, underglow_led_id + 1);
    }
    underglow_led_id++;
  }
#endif


  void breathe(uint8_t channel_id){
    static bool led_on = true;
    static uint32_t lastExecutionTime  = 0;
    uint8_t i = ((uint16_t)to_ms_since_boot(get_absolute_time())) >> 3;

    if (i & 0x80) {
      i = 255 - i;
    }

    i           = i << 1;
    uint8_t ii  = (i * i) >> 8;
    uint8_t iii = (ii * i) >> 8;

    i = (((3 * (uint16_t)(ii)) - (2 * (uint16_t)(iii))) / 2) + 80;

    RGBW breathe = LEDManagement::HSVtoRGB(160, 255, i);
    breathe.w    = 0;

    LEDManagement::set_led_at(breathe, channel_id + 1);
  }
#endif
  uint8_t paired_channels_;
  uint8_t connected_channel_id_;
  uint8_t advertising_id;
  uint8_t erease_done;
  uint8_t defy_id_side;

 private:
#ifdef KEYSCANNER
  static constexpr RGBW white  = {0, 0, 0, 255};
  static constexpr RGBW green  = {0, 255, 0, 0};
  static constexpr RGBW blue  = {0, 0, 255, 0};
  static constexpr RGBW red    = {255, 0, 0, 0};
  static constexpr RGBW yellow    = {150, 150, 0, 0};
  static constexpr RGBW ledOff = {0, 0, 0, 0};
  enum Channels : uint8_t {
    NOT_CONNECTED = 5,
    NOT_ON_ADVERTISING
  };
  std::vector<RGBW> key_color{5};
  std::vector<uint8_t> is_paired{5};
  uint8_t underglow_led_id = 36;
#endif
};

static LedModeSerializable_BluetoothPairing
  ledModeSerializableBluetoothPairing{CRC32_STR("LedModeSerializable_BluetoothPairing")};