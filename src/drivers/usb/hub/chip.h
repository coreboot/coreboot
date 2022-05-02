/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_USB_HUB_CHIP_H__
#define __DRIVERS_USB_HUB_CHIP_H__

struct drivers_usb_hub_config {
	const char *name;
	const char *desc;
	unsigned int port_count; /* Number of Super-speed or High-speed ports */
};

#endif /* __DRIVERS_USB_HUB_CHIP_H__ */
