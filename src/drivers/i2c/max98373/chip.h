/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corp.
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

/*
 * Maxim MAX98373 audio codec devicetree bindings
 */
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
