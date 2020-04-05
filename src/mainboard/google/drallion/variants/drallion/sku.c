/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
