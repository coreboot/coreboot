/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stddef.h>
#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <console/console.h>
#include <string.h>
#include <smbios.h>

uint32_t google_chromeec_get_board_sku(void)
{
	MAYBE_STATIC_NONZERO uint32_t sku_id = CROS_SKU_UNKNOWN;

	if (sku_id != CROS_SKU_UNKNOWN)
		return sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		sku_id = CROS_SKU_UNKNOWN;

	return sku_id;
}

const char *google_chromeec_smbios_system_sku(void)
{
	static char sku_str[14]; /* sku{0..2147483647} */
	uint32_t sku_id = google_chromeec_get_board_sku();
	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);
	return sku_str;
}

const char *smbios_system_sku(void)
{
	return google_chromeec_smbios_system_sku();
}

const char *smbios_mainboard_manufacturer(void)
{
	static char oem_name[32];
	static const char *manuf;

	if (manuf)
		return manuf;

	if (google_chromeec_cbi_get_oem_name(&oem_name[0],
			ARRAY_SIZE(oem_name)) < 0) {
		printk(BIOS_ERR, "Couldn't obtain OEM name from CBI\n");
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
	} else {
		manuf = &oem_name[0];
	}

	return manuf;
}
