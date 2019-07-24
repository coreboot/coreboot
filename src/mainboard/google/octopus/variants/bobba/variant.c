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

#include <sar.h>
#include <baseboard/variants.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = get_board_sku();

	if (sku_id == 33 || sku_id == 34 || sku_id == 35 || sku_id == 36 || sku_id == 41 ||
		sku_id == 42 || sku_id == 43 || sku_id == 44)
		filename = "wifi_sar-droid.hex";

	return filename;
}
