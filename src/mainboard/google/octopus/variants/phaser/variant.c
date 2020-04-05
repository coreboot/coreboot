/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>

#define SKU_UNKNOWN     0xFFFFFFFF

void variant_update_devtree(struct device *dev)
{
	uint32_t sku_id = SKU_UNKNOWN;
	struct device *touchscreen_i2c_host;

	touchscreen_i2c_host = pcidev_path_on_root(PCH_DEVFN_I2C7);

	if (touchscreen_i2c_host == NULL)
		return;

	/* SKU ID 1, 6 does not have a touchscreen device, hence disable it. */
	google_chromeec_cbi_get_sku_id(&sku_id);
	if (no_touchscreen_sku(sku_id))
		touchscreen_i2c_host->enabled = 0;
}
