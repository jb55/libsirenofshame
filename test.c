
#include "sirenofshame.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  struct sos sos;
  struct sos_packet packet;

  sos.usb_handle = NULL;
  sos.usb = NULL;
  int ret;

  if ((ret = sos_open(&sos)) != SOS_OK) {
    printf("error: %d\n", ret);
    return 1;
  }

  /* if ((ret = sos_boot(&sos)) != SOS_OK) { */
  /*   printf("boot error: %d\n", ret); */
  /*   return 1; */
  /* } */

  /* if ((ret = sos_read(&sos, &packet)) != SOS_OK) { */
  /*   return 1; */
  /* } */

  /* sos_blinking_siren(&packet); */
  packet.led_mode = SOS_LED_MODE_MANUAL;
  packet.leds[0] = 1;
  packet.leds[1] = 0;
  packet.leds[2] = 0;
  packet.leds[3] = 255;
  packet.leds[4] = 0;

  packet.led_duration = sos_duration(5000);

  sos_write(&sos, &packet);

  /* packet.led_mode = SOS_LED_MODE_OFF; */
  /* sos_write(&sos, &packet); */

  sos_close(&sos);
  return 0;
}
