/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <ec/google/chromeec/ec.h>

uint32_t google_chromeec_get_board_sku(void)
{
	static uint32_t sku_id = CROS_SKU_UNKNOWN;

	if (sku_id != CROS_SKU_UNKNOWN)
		return sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		sku_id = CROS_SKU_UNKNOWN;

	return sku_id;
}
