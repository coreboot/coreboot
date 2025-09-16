/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__
#define __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <mipi/ids.h>

struct drivers_soundwire_cs42l43_config {
	char acpi_name[ACPI_NAME_BUFFER_SIZE]; /* Set by the acpi_name ops */
	const char *desc;

	const char *sub; /* SUB ID to uniquely identify system */
};

#endif /* __DRIVERS_SOUNDWIRE_CS42L43_CHIP_H__ */
