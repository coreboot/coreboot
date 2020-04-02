/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/*
 * Intel Integrated Sensor Hub (ISH)
 */
struct drivers_intel_ish_config {
	/* Firmware name used by kernel for loading ISH firmware */
	const char *firmware_name;
};
