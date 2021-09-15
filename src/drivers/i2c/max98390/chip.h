/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * MAXIM MAX98390 audio codec devicetree bindings
 */

struct drivers_i2c_max98390_config {
	const char *name; /* ACPI Device Name */
	const char *desc; /* Device Description */
	unsigned int uid; /* ACPI _UID */

	/* The VPD key of calibrated speaker resistance. */
	const char *r0_calib_key;
	/* The VPD key of temperature during speaker calibration. */
	const char *temperature_calib_key;
	const char *dsm_param_file_name;
	/* slot number where voltage feedback will be received */
	uint32_t vmon_slot_no;
	/* slot number where current feedback will be received */
	uint32_t imon_slot_no;
};
