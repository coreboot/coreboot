/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * Realtek RT5663 audio codec devicetree bindings
 */

#include <arch/acpi_device.h>
#include <stdint.h>

struct drivers_i2c_rt5663_config {
	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned int bus_speed;
	/* Identifier for multiple chips */
	unsigned int uid;

	/* Allow GPIO based interrupt or PIRQ */
	struct acpi_gpio irq_gpio;
	struct acpi_irq irq;

	/* Manual offset value to compensate DC offset for L/R channels */
	uint32_t dc_offset_l_manual;
	uint32_t dc_offset_r_manual;
	uint32_t dc_offset_l_manual_mic;
	uint32_t dc_offset_r_manual_mic;
};
