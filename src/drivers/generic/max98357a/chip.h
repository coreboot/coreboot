/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi_device.h>

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
