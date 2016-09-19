
#include "sirenofshame.h"
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifdef linux
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

static void
sos_sleep(int ms)
{
#ifdef linux
  usleep(ms * 1000);   // usleep takes sleep time in us (1 millionth of a second)
#endif
#ifdef WINDOWS
  Sleep(ms);
#endif
}

void
sos_init_packet(struct sos_packet *packet) {
  packet->report_id      = 1;
  packet->control_byte   = 0xFF;
  packet->audio_mode     = 0xFF;
  packet->led_mode       = 0xFF;
  packet->audio_duration = 0xFFFF;
  packet->led_duration   = 0xFFFF;
  packet->read_audio_ind = 0xFF;
  packet->read_led_ind   = 0xFF;
  memset(packet->leds, 0xFF, SOS_LEDS);
}

static int
sos_serialize(struct sos_packet *packet, unsigned char *buffer, int buf_len) {
  int i = 0;
  unsigned char *buf;

  if (buf_len < SOS_PACKET_SIZE) return SOS_BUFFER_TOO_SMALL;

  buffer[0] = packet->report_id;
  buffer[1] = packet->control_byte;
  buffer[2] = packet->audio_mode;
  buffer[3] = packet->led_mode;
  buffer[4] = packet->audio_duration & 0xFF;
  buffer[5] = (packet->audio_duration >> 8) & 0xFF;
  buffer[6] = packet->led_duration & 0xFF;
  buffer[7] = (packet->led_duration >> 8) & 0xFF;
  buffer[8] = packet->read_audio_ind;
  buffer[9] = packet->read_led_ind;

  buf = &buffer[10];

  for (i = 0; i < SOS_LEDS; ++i)
    buf[i] = packet->leds[i];

  return SOS_OK;
}

static int
sos_deserialize(struct sos_packet *packet, unsigned char *buffer, int buf_len) {
  unsigned char *buf;
  int i = 0;

  if (buf_len < SOS_PACKET_SIZE) return SOS_BUFFER_TOO_SMALL;

  packet->report_id = buffer[0];

  packet->control_byte   = buffer[1];
  packet->audio_mode     = buffer[2];
  packet->led_mode       = buffer[3];
  packet->audio_duration = (buffer[4] << 8 | (buffer[5] & 0xFF)) & 0xFFFF;
  packet->led_duration   = (buffer[6] << 8 | (buffer[7] & 0xFF)) & 0xFFFF;
  packet->read_audio_ind = buffer[8];
  packet->read_led_ind   = buffer[9];

  buf = &buffer[10];

  for (i = 0; i < SOS_LEDS; ++i)
    packet->leds[i] = buf[i];

  return SOS_OK;
}

int
sos_open(struct sos *sos) {
  int ret;
  int config;

  libusb_init(&sos->usb);

  libusb_device_handle *handle =
    libusb_open_device_with_vid_pid(sos->usb,
                                    SOS_VENDOR_ID,
                                    SOS_PRODUCT_ID);
  libusb_device *device =
    libusb_get_device(handle);

  if (!handle)
    return SOS_NOT_FOUND;

  if (!device)
    return SOS_GET_DEVICE_FAILED;

  if (libusb_kernel_driver_active(handle, SOS_INTERFACE_NUMBER)) {
    ret = libusb_detach_kernel_driver(handle, SOS_INTERFACE_NUMBER);
    if (ret != LIBUSB_SUCCESS) {
      printf("libusb_detach_kernel_driver err: %s\n", libusb_strerror(ret));
      return SOS_DEVICE_BUSY;
    }
  }

  ret = libusb_set_configuration(handle, 1);
  if (ret != LIBUSB_SUCCESS) {
    printf("libusb_set_configuration err: %s\n", libusb_strerror(ret));
    return SOS_DEVICE_BUSY;
  }

  ret = libusb_claim_interface(handle, SOS_INTERFACE_NUMBER);

  if (ret != LIBUSB_SUCCESS) {
    printf("libusb_claim_interface err: %s\n", libusb_strerror(ret));
    return SOS_DEVICE_BUSY;
  }


  sos->usb_handle = handle;

  return SOS_OK;
}

static int
sos_send_message(struct sos *sos, unsigned char *message, int len) {
  int transferred;

  static const enum libusb_request_type request_type =
    LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_REQUEST_CLEAR_FEATURE;

  static const enum libusb_standard_request request =
    LIBUSB_REQUEST_SET_CONFIGURATION;

  transferred = 
    libusb_control_transfer(sos->usb_handle, request_type, request,
                            0x0201, 0, message, len, SOS_TIMEOUT);

  if (transferred <= 0) {
    printf("libusb control_transfer err: %s\n", libusb_strerror(transferred));
    return SOS_LIBUSB_ERROR;
  }

  return SOS_OK;
}

int
sos_boot(struct sos *sos) {
  unsigned char buffer[SOS_PACKET_SIZE];
  int ret;
  struct sos_packet packet;
  sos_blinking_siren(&packet);
  packet.audio_mode = SOS_AUDIO_MODE_INTERNAL_START;

  sos_serialize(&packet, buffer, ARRAY_SIZE(buffer));

  if ((ret = sos_send_message(sos, buffer, ARRAY_SIZE(buffer))) != SOS_OK)
    return ret;

  sos_sleep(2000);

  packet.led_mode   = SOS_LED_MODE_OFF;
  packet.audio_mode = SOS_AUDIO_MODE_OFF;

  sos_serialize(&packet, buffer, ARRAY_SIZE(buffer));
  if ((ret = sos_send_message(sos, buffer, ARRAY_SIZE(buffer))) != SOS_OK)
    return ret;

  return SOS_OK;
}


int
sos_get_input_report(struct sos *sos, unsigned char *message, int len, char control) {
  int transferred;

  static const int request_type =
    LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN;

  static const enum libusb_standard_request request =
    LIBUSB_REQUEST_CLEAR_FEATURE;

  message[0] = 1;

  transferred =
    libusb_control_transfer(sos->usb_handle, request_type, request,
                            (1 << 8) | control, 0, message,
                            len, SOS_TIMEOUT);

  if (transferred <= 0) {
    printf("libusb control_transfer err: %s\n", libusb_strerror(transferred));
    return SOS_LIBUSB_ERROR;
  }

  return SOS_OK;
}

int
sos_read(struct sos *sos, struct sos_packet *packet) {
  unsigned char buffer[SOS_PACKET_SIZE];
  int ret;
  if ((ret = sos_get_input_report(sos,
                                  buffer, ARRAY_SIZE(buffer), 0x1)) != SOS_OK) {
    return ret;
  }
  sos_deserialize(packet, buffer, ARRAY_SIZE(buffer));
  return SOS_OK;
}


void
sos_blinking_siren(struct sos_packet *packet) {
  sos_init_packet(packet);
  packet->report_id    = SOS_USB_REPORTID_OUT_CONTROL;
  packet->led_mode     = SOS_LED_MODE_INTERNAL_START;
  packet->led_duration = SOS_DURATION_FOREVER;
}

void
sos_close(struct sos *sos) {
  if (sos->usb_handle) {
    libusb_release_interface(sos->usb_handle, SOS_INTERFACE_NUMBER);
    libusb_close(sos->usb_handle);
  }

  if (sos->usb)
    libusb_exit(sos->usb);
}
