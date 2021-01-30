
// libUSB passthrough: instrumented bridge to libusb.

#ifndef LIBUSB_PASSTHROUGH
#define LIBUSB_PASSTHROUGH

#include <libusb-1.0/libusb.h>

int libUSB_init(libusb_context **context);
void libUSB_exit(struct libusb_context *ctx);

ssize_t libUSB_get_device_list(libusb_context * ctx, libusb_device *** list);
int libUSB_get_device_descriptor(libusb_device *dev, struct libusb_device_descriptor *desc);
uint8_t libUSB_get_device_address(libusb_device *dev);
uint8_t libUSB_get_bus_number(libusb_device *dev);
void libUSB_free_device_list(libusb_device **list, int unref_devices);

libusb_device_handle * libUSB_open_device_with_vid_pid (libusb_context *ctx, uint16_t vendor_id, uint16_t product_id);
int libUSB_set_configuration(libusb_device_handle *dev, int configuration);
int libUSB_claim_interface(libusb_device_handle *dev, int interface_number);

void libUSB_close(libusb_device_handle *dev_handle);

// Miscellaneous functions.

int libUSB_reset_device(libusb_device_handle *dev);

int libUSB_clear_halt(libusb_device_handle *dev, unsigned char endpoint);

int libUSB_control_transfer(libusb_device_handle *dev_handle, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout);
int libUSB_bulk_transfer(struct libusb_device_handle *dev_handle, unsigned char endpoint, unsigned char *data, int length, int *transferred, unsigned int timeout);


// new functions not in original version of this file

int libUSB_get_port_numbers(libusb_device *dev, uint8_t *port_numbers, int port_numbers_len);
void libUSB_set_debug(libusb_context *ctx, int level);
struct libusb_transfer *libUSB_alloc_transfer(int iso_packets);
int libUSB_submit_transfer(struct libusb_transfer *transfer);
int libUSB_handle_events_timeout(libusb_context *ctx, struct timeval *tv);
int libUSB_cancel_transfer(struct libusb_transfer *transfer);
void libUSB_free_transfer(struct libusb_transfer *transfer);
int libUSB_open(libusb_device *dev, libusb_device_handle **dev_handle);

#endif // LIBUSB_PASSTHROUGH
