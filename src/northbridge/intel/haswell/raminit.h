/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include <types.h>

#define SPD_MEMORY_DOWN	0xff

struct spd_info {
	uint8_t addresses[4];
	unsigned int spd_index;
};

#if CONFIG(INTEL_LYNXPOINT_LP)
#define MAX_USB2_PORTS	10
#define MAX_USB3_PORTS	4
#else
#define MAX_USB2_PORTS	14
#define MAX_USB3_PORTS	6
#endif

/* There are 8 OC pins */
#define USB_OC_PIN_SKIP	8

enum usb2_port_location {
	USB_PORT_SKIP = 0,
	USB_PORT_BACK_PANEL,
	USB_PORT_FRONT_PANEL,
	USB_PORT_DOCK,
	USB_PORT_MINI_PCIE,
	USB_PORT_FLEX,
	USB_PORT_INTERNAL,
};

/*
 * USB port length is in MRC format: binary-coded decimal length in tenths of an inch.
 *   4.2 inches -> 0x0042
 *  12.7 inches -> 0x0127
 */
struct usb2_port_config {
	uint16_t length;
	bool enable;
	unsigned short oc_pin;
	enum usb2_port_location location;
};

struct usb3_port_config {
	bool enable;
	unsigned int oc_pin;
};

/* Mainboard-specific USB configuration */
extern const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS];
extern const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS];

/* Mainboard callback to fill in the SPD addresses */
void mb_get_spd_map(struct spd_info *spdi);

void perform_raminit(const int s3resume);

#endif				/* RAMINIT_H */
