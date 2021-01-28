/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_SX9310_CHIP_H__
#define __DRIVERS_I2C_SX9310_CHIP_H__

#include <acpi/acpi_device.h>
#include <device/i2c_simple.h>

#define MAX_COMBINED_SENSORS_ENTRIES 4

enum sx9310_resolution {
	SX9310_COARSEST = 1,
	SX9310_VERY_COARSE,
	SX9310_COARSE,
	SX9310_MEDIUM_COARSE,
	SX9310_MEDIUM,
	SX9310_FINE,
	SX9310_VERY_FINE,
	SX9310_FINEST,
};

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

	/*
	 * Registers definition in the kernel source tree at:
	 * Documentation/devicetree/bindings/iio/proximity/semtech,sx9310.yaml
	 */

	/* When true, cs0 is the ground. 0 [default] is false. */
	uint32_t cs0_ground;

	/* Sensor used for start-up proximity detection: Default 0. */
	uint32_t startup_sensor;

	/* Raw Proximity filter strength: When not set, disabled. */
	uint32_t proxraw_strength;

	/* Average Proximity filter strength: When not set, disabled. */
	uint32_t avg_pos_strength;

	/*
	 * List of which sensors are combined and represented by CS3.
	 * Could be standalone (3) or combination of 0, 1, 2, 3.
	 * Driver default: CS0 + CS1.
	 */
	uint32_t combined_sensors_count;
	uint32_t combined_sensors[MAX_COMBINED_SENSORS_ENTRIES];

	/* Capacitance measure resolution. Driver default: "finest". */
	enum sx9310_resolution resolution;
};

#endif /* __DRIVERS_I2C_SX9310_CHIP_H__ */
