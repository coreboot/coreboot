/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_GENERIC_ALC1015_CHIP_H__
#define __DRIVERS_GENERIC_ALC1015_CHIP_H__

#include <acpi/acpi_device.h>

struct drivers_generic_alc1015_config {
	const char *hid;	/* ACPI _HID */
	struct acpi_gpio sdb;	/* SDMODE GPIO */
};

#endif /* __DRIVERS_GENERIC_ALC1015_CHIP_H__ */
