
#include "sirenofshame.h"
#include <stdio.h>

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
  libusb_set_debug(sos.usb, LIBUSB_LOG_LEVEL_DEBUG);

  if ((ret = sos_boot(&sos)) != SOS_OK) {
    printf("boot error: %d\n", ret);
    return 1;
  }

  /* if ((ret = sos_read(&sos, &packet)) != SOS_OK) { */
  /*   return 1; */
  /* } */

  sos_close(&sos);
  return 0;
}
