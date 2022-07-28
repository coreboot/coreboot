/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Maxim MAX98396 audio codec devicetree bindings
 */

#include <stdint.h>

struct drivers_i2c_max98396_config {
	/* I2C Bus Frequency in Hertz (default 400kHz) */
	uint32_t bus_speed;
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
	/* slot number of speaker DSP monitor  */
	uint32_t spkfb_slot_no;
	/* Use GPIO based reset gpio */
	struct acpi_gpio reset_gpio;
};
