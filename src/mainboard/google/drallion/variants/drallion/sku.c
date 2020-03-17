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
#include <smbios.h>
#include <ec/google/wilco/commands.h>
#include <variant/variant.h>

static const uint32_t get_sku_index(void)
{
	return ((!has_360_sensor_board()) | (wilco_ec_signed_fw() << 1));
}

uint32_t sku_id(void)
{
	return skus[get_sku_index()].id;
}

const char *smbios_system_sku(void)
{
	return skus[get_sku_index()].name;
}
