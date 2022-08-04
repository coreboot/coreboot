/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __NXP_UWB_CHIP_H__
#define __NXP_UWB_CHIP_H__

#include <acpi/acpi_device.h>

struct drivers_nxp_uwb_config {
	/* ACPI Device Name */
	const char *name;

	/* Device Description */
	const char *desc;

	/* ACPI _UID */
	unsigned int uid;

	/* Bus speed in Hz (default 1MHz) */
	unsigned int speed;

	/* Use GPIO based interrupt instead of PIRQ */
	struct acpi_gpio irq_gpio;
	struct acpi_gpio ce_gpio;
	struct acpi_gpio ri_gpio;
};

#endif /* __NXP_UWB_CHIP_H__ */
