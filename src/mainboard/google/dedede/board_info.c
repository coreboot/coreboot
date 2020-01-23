/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <smbios.h>
#include <string.h>

#define SKU_UNKNOWN	0xffffffff
#define SKU_MAX		0x7fffffff

static uint32_t board_info_get_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		sku_id = SKU_UNKNOWN;

	return sku_id;
}

const char *smbios_system_sku(void)
{
	/* sku{0..2147483647} */
	static char sku_str[14];
	uint32_t sku_id = board_info_get_sku();

	if (sku_id == SKU_UNKNOWN || sku_id > SKU_MAX) {
		printk(BIOS_ERR, "%s: Unexpected SKU ID %u\n",
			__func__, sku_id);
		return "";
	}

	snprintf(sku_str, sizeof(sku_str), "sku%u", sku_id);

	return sku_str;
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

int board_info_get_fw_config(uint32_t *fw_config)
{
	uint32_t sku_id = board_info_get_sku();

	/*
	 * FW_CONFIG can potentially have all the bits set. So check the
	 * sku_id to ensure that the CBI is provisioned before reading the
	 * FW_CONFIG.
	 */
	if (sku_id == SKU_UNKNOWN || sku_id > SKU_MAX)
		return -1;

	return google_chromeec_cbi_get_fw_config(fw_config);
}
