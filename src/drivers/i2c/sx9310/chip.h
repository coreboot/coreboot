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

#ifndef __DRIVERS_I2C_SX9310_CHIP_H__
#define __DRIVERS_I2C_SX9310_CHIP_H__

#include <arch/acpi_device.h>
#include <device/i2c_simple.h>

#define REGISTER(NAME) uint8_t NAME

struct drivers_i2c_sx9310_config {
	/* Device Description */
	const char *desc;

	/* ACPI _UID */
	unsigned int uid;

	/* Bus speed in Hz, default is I2C_SPEED_FAST */
	enum i2c_speed speed;

	/* Use GPIO-based interrupt instead of IO-APIC */
	struct acpi_gpio irq_gpio;

	/* IO-APIC interrupt */
	struct acpi_irq irq;
#include "registers.h"
};

#undef REGISTER

#endif /* __DRIVERS_I2C_SX9310_CHIP_H__ */
