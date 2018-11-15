/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
#include <smbios.h>
#include <variant/variant.h>

uint32_t sku_id(void)
{
	return VARIANT_SKU_ID;
}

const char *smbios_mainboard_sku(void)
{
	return VARIANT_SKU_NAME;
}
