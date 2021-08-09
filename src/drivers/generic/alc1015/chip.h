/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

struct drivers_generic_alc1015_config {
	const char *hid;	/* ACPI _HID */
	struct acpi_gpio sdb;	/* SDMODE GPIO */
};
