/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code == BOARD_ID_INIT) {
		cached_sku_code = google_chromeec_get_board_sku();

		if (cached_sku_code == CROS_SKU_UNKNOWN ||
		    cached_sku_code == CROS_SKU_UNPROVISIONED) {
			printk(BIOS_WARNING, "Failed to get SKU code from EC\n");
			cached_sku_code = CROS_SKU_UNPROVISIONED;
		}
		printk(BIOS_DEBUG, "SKU Code: %#02x\n", cached_sku_code);
	}

	return cached_sku_code;
}
