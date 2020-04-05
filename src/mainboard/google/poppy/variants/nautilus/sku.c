/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/sku.h>

uint32_t variant_board_sku(void)
{
#if ENV_ROMSTAGE
	uint32_t sku_id = SKU_UNKNOWN;
#else
	static uint32_t sku_id = SKU_UNKNOWN;

	if (sku_id != SKU_UNKNOWN)
		return sku_id;
#endif

	/*
	*  Nautilus uses GPP_B20 to determine SKU
	*  0 - Wifi SKU
	*  1 - LTE SKU
	*/
	gpio_input_pulldown(GPP_B20);
	if (!gpio_get(GPP_B20))
		sku_id = SKU_0_NAUTILUS;
	else
		sku_id = SKU_1_NAUTILUS_LTE;

	return sku_id;
}
