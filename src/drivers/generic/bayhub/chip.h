/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_GENERIC_BAYHUB_CHIP_H__
#define __DRIVERS_GENERIC_BAYHUB_CHIP_H__

#include <acpi/acpi_device.h>

/*
 * Bayhub BG720 PCI to eMMC bridge
 */
struct drivers_generic_bayhub_config {
	/* enable/disable power-saving mode */
	bool power_saving;

	/* When set, disables programming HS200 mode */
	bool disable_hs200_mode;

	/* CLK and DAT tuning values */
	uint8_t vih_tuning_value;
};

#endif /* __DRIVERS_GENERIC_BAYHUB_CHIP_H__ */
