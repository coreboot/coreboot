/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_USB_HUB_CHIP_H__
#define __DRIVERS_USB_HUB_CHIP_H__

#include <acpi/acpi.h>

struct drivers_usb_hub_config {
	const char *name;
	const char *desc;
	unsigned int port_count; /* Number of Super-speed or High-speed ports */
	char acpi_name[ACPI_NAME_BUFFER_SIZE]; /* Set by the acpi_name ops */
};

#endif /* __DRIVERS_USB_HUB_CHIP_H__ */
