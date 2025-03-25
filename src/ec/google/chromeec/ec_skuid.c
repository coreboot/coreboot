/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>

uint32_t sku_id(void)
{
	return google_chromeec_get_board_sku();
}
