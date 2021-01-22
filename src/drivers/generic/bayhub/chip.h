/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

/*
 * Bayhub BG720 PCI to eMMC bridge
 */
struct drivers_generic_bayhub_config {
	/* 1 to enable power-saving mode, 0 to disable */
	int power_saving;

	/* When set, disables programming HS200 mode */
	bool disable_hs200_mode;

	/* CLK and DAT tuning values */
	uint8_t vih_tuning_value;
};
