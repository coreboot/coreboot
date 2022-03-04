/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <device/device.h>
#include <variant/sku.h>

static bool eps_sku(uint32_t sku_id)
{
	/*
	 * Assume EPS SKU by default, helpful for testing on
	 * unprovisioned or development SKUs.
	 */
	if (sku_id == JINLON_SKU_01 || sku_id == JINLON_SKU_21)
		return false;
	else
		return true;
}

static void check_for_eps(uint32_t sku_id)
{
	struct device *eps_dev = DEV_PTR(eps);

	if (eps_sku(sku_id)) {
		printk(BIOS_INFO, "SKU ID %u has EPS\n", sku_id);
		return;
	}

	if (!eps_dev) {
		printk(BIOS_ERR, "Error! No EPS dev, view-angle-management won't work\n");
		return;
	}

	printk(BIOS_INFO, "SKU ID %u doesn't have EPS, disabling...\n", sku_id);
	eps_dev->enabled = 0;
}

void variant_devtree_update(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	/* Disable EPS on SKUs that do not support it */
	check_for_eps(sku_id);
}
