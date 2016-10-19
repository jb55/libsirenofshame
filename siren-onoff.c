
#include "sirenofshame.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  struct sos sos;
  struct sos_packet packet;

  sos.usb_handle = NULL;
  sos.usb = NULL;
  int on = 0;
  int ret;

  if ((ret = sos_open(&sos)) != SOS_OK) {
    printf("error: %d\n", ret);
    return 1;
  }

  if (argc < 2) {
    return 1;
  }
  
  on = argv[1][0] == '1';

  /* packet.led_mode = SOS_LED_MODE_MANUAL; */
  /* packet.leds[0] = 1; */
  /* packet.leds[1] = 0; */
  /* packet.leds[2] = 0; */
  /* packet.leds[3] = 255; */
  /* packet.leds[4] = 0; */
  packet.led_mode = on ? SOS_LED_MODE_INTERNAL_START : 0;
  packet.audio_mode = on ? 3 : SOS_AUDIO_MODE_OFF;

  packet.led_duration = on ? SOS_DURATION_FOREVER : 0;
  packet.audio_duration = on ? SOS_DURATION_FOREVER : 0;

  sos_write(&sos, &packet);

  /* packet.led_mode = SOS_LED_MODE_OFF; */
  /* sos_write(&sos, &packet); */

  sos_close(&sos);
  return 0;
}
