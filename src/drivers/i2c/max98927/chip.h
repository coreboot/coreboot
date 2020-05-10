/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Maxim MAX98927 audio codec devicetree bindings
 */
struct drivers_i2c_max98927_config {
	/* I2C Bus Frequency in Hertz (default 400kHz) */
	unsigned int bus_speed;
	/* Set '1' if I2S channel size is not 32bit. */
	bool interleave_mode;
	/* Identifier for chips */
	unsigned int uid;
	/* Device Description */
	const char *desc;
	/* ACPI Device Name */
	const char *name;
	/* slot number where voltage feedback will be received */
	unsigned int vmon_slot_no;
	/* slot number where current feedback will be received */
	unsigned int imon_slot_no;
};
