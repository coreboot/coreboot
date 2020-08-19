/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Realtek RT1011 audio codec devicetree bindings
 */

struct drivers_i2c_rt1011_config {
	const char *name; /* ACPI Device Name */
	const char *desc; /* Device Description */
	unsigned int uid; /* ACPI _UID */

	/* The VPD key of calibrated speaker resistance. */
	const char *r0_calib_key;
	/* The VPD key of temperature during speaker calibration. */
	const char *temperature_calib_key;
};
