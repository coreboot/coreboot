/*
 * This file is part of the coreboot project.
 *
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
 * Realtek RT1011 audio codec devicetree bindings
 */

#include <stdint.h>

struct drivers_i2c_rt1011_config {
	const char *name; /* ACPI Device Name */
	const char *desc; /* Device Description */
	unsigned int uid; /* ACPI _UID */

	/* The VPD key of calibrated speaker resistance. */
	const char *r0_calib_key;
	/* The VPD key of temperature during speaker calibration. */
	const char *temperature_calib_key;
};
