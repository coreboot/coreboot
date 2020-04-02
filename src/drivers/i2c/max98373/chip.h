/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * Maxim MAX98373 audio codec devicetree bindings
 */

#include <stdint.h>

struct drivers_i2c_max98373_config {
	/* I2C Bus Frequency in Hertz (default 400kHz) */
	uint32_t bus_speed;
	/* Set '1' if I2S channel size is not 32bit. */
	bool interleave_mode;
	/* Identifier for chips */
	uint32_t uid;
	/* Device Description */
	const char *desc;
	/* ACPI Device Name */
	const char *name;
	/* slot number where voltage feedback will be received */
	uint32_t vmon_slot_no;
	/* slot number where current feedback will be received */
	uint32_t imon_slot_no;
};
