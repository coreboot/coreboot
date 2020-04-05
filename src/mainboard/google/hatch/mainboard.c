/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <smbios.h>
#include <string.h>
#include <stdint.h>

const char *smbios_mainboard_manufacturer(void)
{
	static char oem_name[32];
	static const char *manuf;

	if (manuf)
		return manuf;

	if (google_chromeec_cbi_get_oem_name(&oem_name[0],
				ARRAY_SIZE(oem_name)) < 0) {
		printk(BIOS_INFO, "Couldn't obtain OEM name from CBI\n");
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
	} else {
		manuf = &oem_name[0];
	}

	return manuf;
}
