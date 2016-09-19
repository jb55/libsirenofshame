
#include <libusb-1.0/libusb.h>

#define SOS_LEDS 5
#define SOS_PACKET_SIZE (1 + 37)
#define SOS_TIMEOUT 5000

#define SOS_OK 0
#define SOS_BUFFER_TOO_SMALL 1
#define SOS_NOT_FOUND 2
#define SOS_LIBUSB_ERROR 3
#define SOS_GET_DEVICE_FAILED 4
#define SOS_DEVICE_BUSY 5

#define SOS_INTERFACE_NUMBER               0
#define SOS_AUDIO_MODE_INTERNAL_START      1
#define SOS_AUDIO_MODE_OFF                 0
#define SOS_CONTROL_CHAR1_DEBUG            0x04
#define SOS_CONTROL_CHAR1_ECHO_ON          0x02
#define SOS_CONTROL_CHAR1_FIRMWARE_UPGRADE 0x01
#define SOS_CONTROL_CHAR1_IGNORE           0xff
#define SOS_HARDWARE_TYPE_PRO              2
#define SOS_HARDWARE_TYPE_STANDARD         1
#define SOS_LED_MODE_INTERNAL_START        2
#define SOS_LED_MODE_MANUAL                1
#define SOS_LED_MODE_OFF                   0
#define SOS_USB_REPORTID_IN_INFO           1
#define SOS_USB_REPORTID_IN_READ_AUDIO     3
#define SOS_USB_REPORTID_IN_READ_LED       4
#define SOS_USB_REPORTID_OUT_CONTROL       1
#define SOS_USB_REPORTID_OUT_DATA_UPLOAD   2
#define SOS_USB_REPORTID_OUT_LED_CONTROL   3
#define SOS_USB_DATA_SIZE                  32
#define SOS_USB_NAME_SIZE                  20
#define SOS_DURATION_FOREVER               0xfeff
#define SOS_PLAY_DURATION_FOREVER          0xfffe

#define SOS_VENDOR_ID  0x16d0
#define SOS_PRODUCT_ID 0x0646

struct sos {
  libusb_context *usb;
  libusb_device_handle *usb_handle;
};

struct sos_packet {
  char  report_id;
  char  control_byte;
  char  audio_mode;
  char  led_mode;
  short audio_duration;
  short led_duration;
  char  read_audio_ind;
  char  read_led_ind;
  char  leds[SOS_LEDS];
};

int  sos_open(struct sos *);
int  sos_read(struct sos *, struct sos_packet *packet);
void sos_close(struct sos *);
int  sos_boot(struct sos *);

void
sos_blinking_siren(struct sos_packet *packet);
