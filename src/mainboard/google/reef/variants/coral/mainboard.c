/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016, 2017 Google Inc.
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

#include <stdint.h>
#include <ec/google/chromeec/ec.h>
#include "baseboard/variants.h"
#include <soc/cpu.h>
#include <soc/intel/apollolake/chip.h>

enum {
	SKU_2_SANTA = 2,
	SKU_3_SANTA = 3
};

uint8_t variant_board_sku(void)
{
	// we know that the values are in 0..255
	return google_chromeec_get_sku_id();
}

void variant_nhlt_oem_overrides(const char **oem_id,
				const char **oem_table_id,
				uint32_t *oem_revision)
{
	*oem_id = "coral";
	*oem_table_id = CONFIG_VARIANT_DIR;
	*oem_revision = variant_board_sku();
}

void mainboard_devtree_update(struct device *dev)
{
       /* Override dev tree settings per board */
	struct soc_intel_apollolake_config *cfg = dev->chip_info;
	uint8_t sku_id;

	sku_id = variant_board_sku();

	switch (sku_id) {
	case SKU_2_SANTA:
	case SKU_3_SANTA:
		cfg->usb2eye[1].Usb20PerPortPeTxiSet = 7;
		cfg->usb2eye[1].Usb20PerPortTxiSet = 2;
		break;
	default:
		break;
	}
}
