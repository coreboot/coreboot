/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	return id;
}

uint32_t ram_code(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;

	return id;
}

uint32_t sku_id(void)
{
	return google_chromeec_get_board_sku();
}
