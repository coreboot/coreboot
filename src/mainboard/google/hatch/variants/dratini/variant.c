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
#include <variant/sku.h>
#include <sar.h>
#include <ec/google/chromeec/ec.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_21_DRAGONAIR:
	case SKU_22_DRAGONAIR:
	case SKU_23_DRAGONAIR:
	case SKU_24_DRAGONAIR:
		filename = "wifi_sar-dragonair.hex";
		break;
	}
	return filename;
}
