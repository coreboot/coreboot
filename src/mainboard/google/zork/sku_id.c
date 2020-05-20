/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boardid.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

uint32_t sku_id(void)
{
	return google_chromeec_get_board_sku();
}

/* An unprovisioned SKU ID indicates we're in the factory booting prior to
   proper SKU ID provisioning. */
int boot_is_factory_unprovisioned(void)
{
	return sku_id() == CROS_SKU_UNPROVISIONED;
}
