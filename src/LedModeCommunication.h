/* LedModeCommunication - LED mode communications specifications and header definition.
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

#ifndef _LEDMODECOMMUNICATION_H_
#define _LEDMODECOMMUNICATION_H_

#include "Communications_protocol.h"
#include "Communications.h"

class LedModeCommunication
{
private:
    /*
     *  Check if the mode is set to layer mode. Mode 0 is the layer mode.
     * */
    static bool layer_mode_set ()
    {
        return ::LEDControl.get_mode_index() == 0;
    }
 protected:
  static void sendLedMode(LedModeSerializable &led_mode_serializable)
  {
      // If the host is not connected, we should not update the mode and show the user layers.
      // This block is responsible for sending the LED mode to the host and preventing
      // that if you have both sides connected to neuron,
      // and you disconnect and reconnect the side, the keyboard will not show the layers.
      if(!Communications.is_host_connected() && layer_mode_set())
      {
          NRF_LOG_INFO("Host not connected, not sending LED mode");
          return;
      }

      Communications_protocol::Packet packet{};
      packet.header.command = Communications_protocol::MODE_LED;
      packet.header.size    = led_mode_serializable.serialize(packet.data);
      Communications.sendPacket(packet);
  }
};
#endif  //_LEDMODECOMMUNICATION_H_
