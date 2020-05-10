/* SPDX-License-Identifier: GPL-2.0-only */

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
