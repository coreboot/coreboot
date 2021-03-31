/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

struct drivers_generic_nau8315_config {
	/* ENABLE GPIO */
	struct acpi_gpio enable_gpio;
};
