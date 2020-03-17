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

#include <boardid.h>
#include <ec/google/wilco/commands.h>
#include <smbios.h>
#include <variant/variant.h>

uint32_t sku_id(void)
{
	if (wilco_ec_signed_fw())
		return VARIANT_SKU_ID_SIGNED_EC;
	else
		return VARIANT_SKU_ID;
}

const char *smbios_system_sku(void)
{
	if (wilco_ec_signed_fw())
		return VARIANT_SKU_NAME_SIGNED_EC;
	else
		return VARIANT_SKU_NAME;
}
