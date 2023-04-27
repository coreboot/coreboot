/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_GENERIC_NAU8315_CHIP_H__
#define __DRIVERS_GENERIC_NAU8315_CHIP_H__

#include <acpi/acpi_device.h>

struct drivers_generic_nau8315_config {
	/* ACPI _HID */
	enum {
		NAU8315,
		NAU8318,
		MAX_HID,
	} hid;
	/* ENABLE GPIO */
	struct acpi_gpio enable_gpio;
};

#endif /* __DRIVERS_GENERIC_NAU8315_CHIP_H__ */
