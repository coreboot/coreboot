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
