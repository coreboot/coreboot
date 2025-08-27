/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_SOUNDWIRE_CS35L56_FAMILY_CHIP_H__
#define __DRIVERS_SOUNDWIRE_CS35L56_FAMILY_CHIP_H__

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <mipi/ids.h>

enum cs35l56_part_ids {
	CS35L56_PART_ID_CS35L56 = MIPI_DEV_ID_CIRRUS_CS35L56,
	CS35L56_PART_ID_CS35L57 = MIPI_DEV_ID_CIRRUS_CS35L57,
	CS35L63_PART_ID_CS35L63 = MIPI_DEV_ID_CIRRUS_CS35L63
};

struct drivers_soundwire_cs35l56_config {
	char acpi_name[ACPI_NAME_BUFFER_SIZE]; /* Set by the acpi_name ops */
	const char *desc;
	enum cs35l56_part_ids part_id;
	const char *sub; /* SUB ID to uniquely identify system */

	/* Use GPIO based spkid gpio */
	struct acpi_gpio spkid_gpio;
};

#endif /* __DRIVERS_SOUNDWIRE_CS35L56_FAMILY_CHIP_H__ */
