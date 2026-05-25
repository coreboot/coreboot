/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/storage.h>
#include <boardid.h>
#include <console/console.h>
#include <device/mmio.h>
#include <ec/google/chromeec/ec.h>

DEFINE_BITFIELD(STORAGE_TYPE, 11, 9);
DEFINE_BITFIELD(CPU_TYPE, 8, 8);
DEFINE_BITFIELD(PANEL_TYPE, 7, 0);

uint32_t sku_id(void)
{
	static uint32_t cached_sku_code = BOARD_ID_INIT;

	if (cached_sku_code == BOARD_ID_INIT) {
		cached_sku_code = google_chromeec_get_board_sku();

		if (cached_sku_code == CROS_SKU_UNKNOWN ||
		    cached_sku_code == CROS_SKU_UNPROVISIONED) {
			printk(BIOS_WARNING, "Failed to get SKU code from EC\n");
			cached_sku_code = CROS_SKU_UNPROVISIONED;
			SET32_BITFIELDS(&cached_sku_code, STORAGE_TYPE, storage_id());
		}
		printk(BIOS_DEBUG, "SKU Code: %#02x\n", cached_sku_code);
	}

	return cached_sku_code;
}
