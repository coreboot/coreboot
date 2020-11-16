/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_SX9324_CHIP_H__
#define __DRIVERS_I2C_SX9324_CHIP_H__

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>

#define REGISTER(NAME) uint8_t NAME

struct drivers_i2c_sx9324_config {
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

#endif /* __DRIVERS_I2C_SX9324_CHIP_H__ */
