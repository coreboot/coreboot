/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
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

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>
#include <baseboard/variants.h>

#define SKU_UNKNOWN     0xFFFFFFFF

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id;
	sku_id = get_board_sku();

	if (sku_id == SKU_UNKNOWN)
		return NULL;

	if (sku_id == 49 || sku_id == 50 || sku_id == 51 || sku_id == 52)
		filename = "wifi_sar-blooguard.hex";

	return filename;
}
