/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Intel Integrated Sensor Hub (ISH)
 */
struct drivers_intel_ish_config {
	/* Firmware name used by kernel for loading ISH firmware */
	const char *firmware_name;

	/* Add `DmaProperty` in _DSD */
	bool add_acpi_dma_property;
};
