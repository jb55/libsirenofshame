#include <fcgi_stdio.h>
#include <stdio.h>
#include "sirenofshame.h"

int duration_to_ms(unsigned short duration) {
  if (duration == SOS_DURATION_FOREVER)
    return -1;
  return duration * 10;
}

void print_json_packet(struct sos_packet *packet) {
  char *led = packet->leds;
  printf("{\r\n\
    \"audio_mode\": %d,\r\n\
    \"led_mode\": %d\r\n\
    \"audio_duration\": %d,\r\n\
    \"led_duration\": %d,\r\n\
    \"read_audio_ind\": %d,\r\n\
    \"read_led_ind\": %d,\r\n\
    \"leds\": [%d, %d, %d, %d, %d]\r\n}",
         packet->audio_mode,
         packet->led_mode,
         duration_to_ms(packet->audio_duration),
         duration_to_ms(packet->led_duration),
         packet->read_audio_ind,
         packet->read_led_ind,
         led[0], led[1], led[2], led[3], led[4]
         );
}

int main(void)
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

  while (FCGI_Accept() >= 0) {
    sos_read(&sos, &packet);
    printf("Content-type: application/json\r\nStatus: 200 OK\r\n\r\n");
    print_json_packet(&packet);
  }

  sos_close(&sos);

  return 0;
}
