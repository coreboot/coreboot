/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <ec/google/chromeec/ec.h>
#include <drivers/intel/gma/opregion.h>

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 2)
		return "vbt-nightfury-qled.bin";
	else
		return "vbt.bin";
}
