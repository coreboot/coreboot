/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_SX9360_CHIP_H__
#define __DRIVERS_I2C_SX9360_CHIP_H__

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>

struct drivers_i2c_sx9360_config {
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

	/*
	 * Registers definition in the kernel source tree at:
	 * Documentation/devicetree/bindings/iio/proximity/semtech,sx9360.yaml
	 */

	/* Raw Proximity filter strength: When not set, disabled. */
	uint32_t proxraw_strength;

	/* Average Proximity filter strength: When not set, disabled. */
	uint32_t avg_pos_strength;

	/* Capacitance measure resolution. Driver default: 128. */
	uint32_t resolution;
};

#endif /* __DRIVERS_I2C_SX9360_CHIP_H__ */
