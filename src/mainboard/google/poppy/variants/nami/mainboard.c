/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google Inc.
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

#include <arch/cpu.h>
#include <assert.h>
#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <smbios.h>
#include <soc/ramstage.h>
#include <string.h>
#include <variant/sku.h>

uint32_t variant_board_sku(void)
{
	static uint32_t sku_id = SKU_UNKNOWN;
	uint32_t id;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;
	if (google_chromeec_cbi_get_sku_id(&id))
		return SKU_UNKNOWN;
	sku_id = id;

	return sku_id;
}

void variant_devtree_update(void)
{
	/* Override dev tree settings per board */
	uint32_t sku_id = variant_board_sku();
	device_t root = SA_DEV_ROOT;
	config_t *cfg = root->chip_info;
	switch (sku_id) {
	case SKU_1_VAYNE:
		cfg->usb2_ports[5].enable = 0;//rear camera
		break;
	default:
		break;
	}
}

const char *smbios_mainboard_sku(void)
{
	static char sku_str[14]; /* sku{0..4294967295} */

	snprintf(sku_str, sizeof(sku_str), "sku%u", variant_board_sku());

	return sku_str;
}
