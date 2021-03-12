/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_H
#define RAMINIT_H

#include <stdint.h>
#include "pei_data.h"

#define SPD_MEMORY_DOWN	0xff

struct spd_info {
	uint8_t addresses[4];
	unsigned int spd_index;
};

/* Mainboard-specific USB configuration */
extern const struct usb2_port_setting mainboard_usb2_ports[MAX_USB2_PORTS];
extern const struct usb3_port_setting mainboard_usb3_ports[MAX_USB3_PORTS];

/* Mainboard callback to fill in the SPD addresses */
void mb_get_spd_map(struct spd_info *spdi);

void perform_raminit(const int s3resume);

#endif				/* RAMINIT_H */
