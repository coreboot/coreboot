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
#include <device/i2c_simple.h>

struct drivers_i2c_tpm_config {
	const char *hid;	/* ACPI _HID (required) */
	const char *desc;	/* Device Description */
	unsigned int uid;	/* ACPI _UID */
	enum i2c_speed speed;	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	struct acpi_irq irq;	/* Interrupt */
	struct acpi_gpio irq_gpio;	/* GPIO interrupt */
};
