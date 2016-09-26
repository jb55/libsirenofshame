/* #include <fcgi_stdio.h> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sirenofshame.h"
#include "querystring/querystring.h"

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

void
handle_query(void *data, char *fst, char *snd) {
  char *leds;
  int ledc = 0;
  int buf;
  struct sos_packet *packet = (struct sos_packet*) data;
  if (strcmp(fst, "leds") == 0) {
    leds = strtok(snd, ",");
    for (ledc = 0; ledc < SOS_LEDS && leds != NULL; ledc++) {
      buf = atoi(leds);
      packet->leds[ledc] = buf;
      leds = strtok(NULL, ",");
      printf("writing led%d %d\r\n", ledc, buf);
    }
  }
  else if (strcmp(fst, "led_mode") == 0) {
    buf = atoi(snd);
    packet->led_mode = buf;
    printf("writing led_mode %d\r\n", buf);
  }
  else if (strcmp(fst, "audio_mode") == 0) {
    buf = atoi(snd);
    packet->audio_mode = buf;
    printf("writing audio_mode %d\r\n", buf);
  }
  else if (strcmp(fst, "audio_duration") == 0) {
    buf = atoi(snd);
    packet->audio_duration = sos_duration(buf);
    printf("writing audio_duration %d ms\r\n", buf);
  }
  else if (strcmp(fst, "led_duration") == 0) {
    buf = atoi(snd);
    packet->led_duration = sos_duration(buf);
    printf("writing led_duration %d ms\r\n", buf);
  }
  else if (strcmp(fst, "read_audio_ind") == 0) {
    buf = atoi(snd);
    packet->read_audio_ind = buf;
    printf("writing read_audio_ind %d\r\n", buf);
  }
  else if (strcmp(fst, "read_led_ind") == 0) {
    buf = atoi(snd);
    packet->read_led_ind = buf;
    printf("writing read_led_ind %d\r\n", buf);
  }
}

int main(void)
{
  struct sos sos;
  struct sos_packet packet;
  char *qs;

  sos.usb_handle = NULL;
  sos.usb = NULL;
  int ret;

  if ((ret = sos_open(&sos)) != SOS_OK) {
    printf("error: %d\n", ret);
    return 1;
  }

  /* FILE *logfile = fopen("/tmp/siren-fcgi.log", "a"); */

  printf("Content-type: application/json\r\nStatus: 200 OK\r\n\r\n");

  qs = getenv("QUERY_STRING");
  sos_read(&sos, &packet);

  if (qs != NULL && *qs != 0) {
    printf("{\"log\": \"\r\n");
    parse_querystring(qs, (void*)&packet, handle_query);
    printf("\",\r\n\"packet\":");
    sos_write(&sos, &packet);
    print_json_packet(&packet);
    printf("}");
  }
  else {
    print_json_packet(&packet);
  }

  /* fprintf(logfile, "closing...\n"); */
  /* fclose(logfile); */

  sos_close(&sos);

  return 0;
}
