/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SOUNDWIRE_MAX98363_CHIP_H__
#define __DRIVERS_SOUNDWIRE_MAX98363_CHIP_H__

#include <acpi/acpi.h>

struct drivers_soundwire_max98363_config {
	char acpi_name[ACPI_NAME_BUFFER_SIZE]; /* Set by the acpi_name ops */
	const char *desc;
};

#endif /* __DRIVERS_SOUNDWIRE_MAX98363_CHIP_H__ */
