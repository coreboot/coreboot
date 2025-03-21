/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	/* Placeholder */
	return id;
}

uint32_t sku_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	/* Placeholder */
	return id;
}
