/*
 * This file is part of the coreboot project.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <boardid.h>
#include <ec/google/wilco/commands.h>
#include <smbios.h>
#include <variant/variant.h>

static const uint32_t get_sku_index(void)
{
	return ((!has_360_sensor_board()) | (wilco_ec_signed_fw() << 1));
}

const uint32_t sku_id(void)
{
	return skus[get_sku_index()].id;
}

const char *smbios_system_sku(void)
{
	return skus[get_sku_index()].name;
}
