/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
