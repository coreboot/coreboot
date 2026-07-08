/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <soc/cdt.h>

uint32_t board_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	if (id != UNDEFINED_STRAPPING_ID)
		return id;

	if (CONFIG(SOC_QUALCOMM_CDT)) {
		id = cdt_get_hw_version();
		return id;
	}

	return id;
}

uint32_t sku_id(void)
{
	static uint32_t id = UNDEFINED_STRAPPING_ID;
	if (id != UNDEFINED_STRAPPING_ID)
		return id;

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		id = google_chromeec_get_board_sku();

	return id;
}
