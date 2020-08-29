/*
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __USB_H
#define __USB_H
#include <libpayload.h>
#include <pci/pci.h>
#include <stdint.h>

typedef enum { host_to_device = 0, device_to_host = 1 } dev_req_dir;
typedef enum { standard_type = 0, class_type = 1, vendor_type =
		2, reserved_type = 3
} dev_req_type;
typedef enum { dev_recp = 0, iface_recp = 1, endp_recp = 2, other_recp = 3
} dev_req_recp;

enum {
	DT_DEV = 1,
	DT_CFG = 2,
	DT_STR = 3,
	DT_INTF = 4,
	DT_ENDP = 5,
};

typedef enum {
	GET_STATUS = 0,
	CLEAR_FEATURE = 1,
	SET_FEATURE = 3,
	SET_ADDRESS = 5,
	GET_DESCRIPTOR = 6,
	SET_DESCRIPTOR = 7,
	GET_CONFIGURATION = 8,
	SET_CONFIGURATION = 9,
	GET_INTERFACE = 10,
	SET_INTERFACE = 11,
	SYNCH_FRAME = 12
} bRequest_Codes;

typedef enum {
	ENDPOINT_HALT = 0,
	DEVICE_REMOTE_WAKEUP = 1,
	TEST_MODE = 2
} feature_selectors;

/* SetAddress() recovery interval (USB 2.0 specification 9.2.6.3 */
#define SET_ADDRESS_MDELAY 2

/*
 * USB sets an upper limit of 5 seconds for any transfer to be completed.
 *
 * Data originally from EHCI driver:
 *	Tested with some USB2.0 flash sticks:
 *	TUR turn around took  about 2.2s for the slowest (13fe:3800), maximum
 *	of 250ms for the others.
 *
 * SET ADDRESS on xHCI controllers.
 *	The USB specification indicates that devices must complete processing
 *	of a SET ADDRESS request within 50 ms.  However, some hubs were found
 *	to take more than 100 ms to complete a SET ADDRESS request on a
 *	downstream port.
 */
#define USB_MAX_PROCESSING_TIME_US (5 * 1000 * 1000)

#define USB_FULL_LOW_SPEED_FRAME_US 1000

typedef struct {
	unsigned char bDescLength;
	unsigned char bDescriptorType;
	unsigned char bNbrPorts;
	union {
		struct {
			unsigned long logicalPowerSwitchingMode:2;
			unsigned long isCompoundDevice:1;
			unsigned long overcurrentProtectionMode:2;
			unsigned long ttThinkTime:2;
			unsigned long arePortIndicatorsSupported:1;
			unsigned long:8;
		} __packed;
		unsigned short wHubCharacteristics;
	} __packed;
	unsigned char bPowerOn2PwrGood;
	unsigned char bHubContrCurrent;
	char DeviceRemovable[];
} __packed hub_descriptor_t;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned short bcdUSB;
	unsigned char bDeviceClass;
	unsigned char bDeviceSubClass;
	unsigned char bDeviceProtocol;
	unsigned char bMaxPacketSize0;
	unsigned short idVendor;
	unsigned short idProduct;
	unsigned short bcdDevice;
	unsigned char iManufacturer;
	unsigned char iProduct;
	unsigned char iSerialNumber;
	unsigned char bNumConfigurations;
} __packed device_descriptor_t;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned short wTotalLength;
	unsigned char bNumInterfaces;
	unsigned char bConfigurationValue;
	unsigned char iConfiguration;
	unsigned char bmAttributes;
	unsigned char bMaxPower;
} __packed configuration_descriptor_t;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bInterfaceNumber;
	unsigned char bAlternateSetting;
	unsigned char bNumEndpoints;
	unsigned char bInterfaceClass;
	unsigned char bInterfaceSubClass;
	unsigned char bInterfaceProtocol;
	unsigned char iInterface;
} __packed interface_descriptor_t;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned char bEndpointAddress;
	unsigned char bmAttributes;
	unsigned short wMaxPacketSize;
	unsigned char bInterval;
} __packed endpoint_descriptor_t;

typedef struct {
	unsigned char bLength;
	unsigned char bDescriptorType;
	unsigned short bcdHID;
	unsigned char bCountryCode;
	unsigned char bNumDescriptors;
	unsigned char bReportDescriptorType;
	unsigned short wReportDescriptorLength;
} __packed hid_descriptor_t;

typedef struct {
	union {
		struct {
			dev_req_recp req_recp:5;
			dev_req_type req_type:2;
			dev_req_dir data_dir:1;
		} __packed;
		unsigned char bmRequestType;
	} __packed;
	unsigned char bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
} __packed dev_req_t;

struct usbdev_hc;
typedef struct usbdev_hc hci_t;

struct usbdev;
typedef struct usbdev usbdev_t;

typedef enum { SETUP, IN, OUT } direction_t;
typedef enum { CONTROL = 0, ISOCHRONOUS = 1, BULK = 2, INTERRUPT = 3
} endpoint_type;

typedef struct {
	usbdev_t *dev;
	int endpoint;
	direction_t direction;
	int toggle;
	int maxpacketsize;
	endpoint_type type;
	int interval; /* expressed as binary logarithm of the number
			 of microframes (i.e. t = 125us * 2^interval) */
} endpoint_t;

typedef enum {
	UNKNOWN_SPEED = -1,
	FULL_SPEED = 0,
	LOW_SPEED = 1,
	HIGH_SPEED = 2,
	SUPER_SPEED = 3,
	SUPER_SPEED_PLUS = 4,
} usb_speed;

struct usbdev {
	hci_t *controller;
	endpoint_t endpoints[32];
	int num_endp;
	int address;		// USB address
	int hub;		// hub, device is attached to
	int port;		// port where device is attached
	usb_speed speed;
	u32 quirks;		// quirks field. got to love usb
	void *data;
	device_descriptor_t *descriptor;
	configuration_descriptor_t *configuration;
	void (*init) (usbdev_t *dev);
	void (*destroy) (usbdev_t *dev);
	void (*poll) (usbdev_t *dev);
};

typedef enum { OHCI = 0, UHCI = 1, EHCI = 2, XHCI = 3, DWC2 = 4} hc_type;

struct usbdev_hc {
	hci_t *next;
	uintptr_t reg_base;
	pcidev_t pcidev; // 0 if not used (eg on ARM)
	hc_type type;
	int latest_address;
	usbdev_t *devices[128];	// dev 0 is root hub, 127 is last addressable

	/* start():     Resume operation. */
	void (*start) (hci_t *controller);
	/* stop():      Stop operation but keep controller initialized. */
	void (*stop) (hci_t *controller);
	/* reset():     Perform a controller reset. The controller needs to
	                be (re)initialized afterwards to work (again). */
	void (*reset) (hci_t *controller);
	/* init():      Initialize a (previously reset) controller
	                to a working state. */
	void (*init) (hci_t *controller);
	/* shutdown():  Stop operation, detach host controller and shutdown
	                this driver instance. After calling shutdown() any
			other usage of this hci_t* is invalid. */
	void (*shutdown) (hci_t *controller);

	int (*bulk) (endpoint_t *ep, int size, u8 *data, int finalize);
	int (*control) (usbdev_t *dev, direction_t pid, int dr_length,
			void *devreq, int data_length, u8 *data);
	void* (*create_intr_queue) (endpoint_t *ep, int reqsize, int reqcount, int reqtiming);
	void (*destroy_intr_queue) (endpoint_t *ep, void *queue);
	u8* (*poll_intr_queue) (void *queue);
	void *instance;

	/* set_address():		Tell the USB device its address (xHCI
					controllers want to do this by
					themselves). Also, allocate the usbdev
					structure, initialize enpoint 0
					(including MPS) and return it. */
	usbdev_t *(*set_address) (hci_t *controller, usb_speed speed,
				  int hubport, int hubaddr);
	/* finish_device_config():	Another hook for xHCI,
					returns 0 on success. */
	int (*finish_device_config) (usbdev_t *dev);
	/* destroy_device():		Finally, destroy all structures that
					were allocated during set_address()
					and finish_device_config(). */
	void (*destroy_device) (hci_t *controller, int devaddr);
};

hci_t *usb_add_mmio_hc(hc_type type, void *bar);
hci_t *new_controller (void);
void detach_controller (hci_t *controller);
void usb_poll (void);
usbdev_t *init_device_entry (hci_t *controller, int num);

int usb_decode_mps0 (usb_speed speed, u8 bMaxPacketSize0);
int speed_to_default_mps(usb_speed speed);
int set_feature (usbdev_t *dev, int endp, int feature, int rtype);
int get_status (usbdev_t *dev, int endp, int rtype, int len, void *data);
int get_descriptor (usbdev_t *dev, int rtype, int descType, int descIdx,
		    void *data, size_t len);
int set_configuration (usbdev_t *dev);
int clear_feature (usbdev_t *dev, int endp, int feature, int rtype);
int clear_stall (endpoint_t *ep);
_Bool is_usb_speed_ss(usb_speed speed);

void usb_nop_init (usbdev_t *dev);
void usb_hub_init (usbdev_t *dev);
void usb_hid_init (usbdev_t *dev);
void usb_msc_init (usbdev_t *dev);
void usb_generic_init (usbdev_t *dev);

int closest_usb2_hub(const usbdev_t *dev, int *const addr, int *const port);

static inline unsigned char
gen_bmRequestType (dev_req_dir dir, dev_req_type type, dev_req_recp recp)
{
	return (dir << 7) | (type << 5) | recp;
}

/* default "set address" handler */
usbdev_t *generic_set_address (hci_t *controller, usb_speed speed,
			       int hubport, int hubaddr);

void usb_detach_device(hci_t *controller, int devno);
int usb_attach_device(hci_t *controller, int hubaddress, int port,
		      usb_speed speed);

u32 pci_quirk_check(pcidev_t controller);
u32 usb_quirk_check(u16 vendor, u16 device);
int usb_interface_check(u16 vendor, u16 device);

#define USB_QUIRK_MSC_FORCE_PROTO_SCSI		(1 <<  0)
#define USB_QUIRK_MSC_FORCE_PROTO_ATAPI		(1 <<  1)
#define USB_QUIRK_MSC_FORCE_PROTO_UFI		(1 <<  2)
#define USB_QUIRK_MSC_FORCE_PROTO_RBC		(1 <<  3)
#define USB_QUIRK_MSC_FORCE_TRANS_BBB		(1 <<  4)
#define USB_QUIRK_MSC_FORCE_TRANS_CBI		(1 <<  5)
#define USB_QUIRK_MSC_FORCE_TRANS_CBI_I		(1 <<  6)
#define USB_QUIRK_MSC_NO_TEST_UNIT_READY	(1 <<  7)
#define USB_QUIRK_MSC_SHORT_INQUIRY		(1 <<  8)
#define USB_QUIRK_HUB_NO_USBSTS_PCD		(1 <<  9)
#define USB_QUIRK_TEST				(1 << 31)
#define USB_QUIRK_NONE				 0

static inline void __attribute__((format(printf, 1, 2))) usb_debug(const char *fmt, ...)
{
#ifdef USB_DEBUG
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
#endif
}

/**
 * To be implemented by libpayload-client. It's called by the USB
 * stack just before iterating over known devices to poll them for
 * status change.
 */
void __attribute__((weak)) usb_poll_prepare (void);

/**
 * To be implemented by libpayload-client. It's called by the USB stack
 * when a new USB device is found which isn't claimed by a built in driver,
 * so the client has the chance to know about it.
 *
 * @param dev descriptor for the USB device
 */
void __attribute__((weak)) usb_generic_create (usbdev_t *dev);

/**
 * To be implemented by libpayload-client. It's called by the USB stack
 * when it finds out that a USB device is removed which wasn't claimed by a
 * built in driver.
 *
 * @param dev descriptor for the USB device
 */
void __attribute__((weak)) usb_generic_remove (usbdev_t *dev);

#endif
