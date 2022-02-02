/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

struct drivers_generic_max98357a_config {
	/* ACPI _HID */
	const char *hid;

	/* SDMODE GPIO */
	struct acpi_gpio sdmode_gpio;

	/* SDMODE Delay */
	unsigned int sdmode_delay;

	/* GPIO used to indicate if this device is present */
	unsigned int device_present_gpio;
	unsigned int device_present_gpio_invert;
};
