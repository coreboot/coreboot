/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>

#define SKU_UNKNOWN     0xFFFFFFFF

void variant_update_devtree(struct device *dev)
{
	uint32_t sku_id = SKU_UNKNOWN;
	struct device *touchscreen_i2c_host;

	touchscreen_i2c_host = pcidev_path_on_root(PCH_DEVFN_I2C7);

	if (touchscreen_i2c_host == NULL)
		return;

	/* SKU ID 1 does not have a touchscreen device, hence disable it. */
	sku_id = google_chromeec_get_board_sku();
	if (no_touchscreen_sku(sku_id))
		touchscreen_i2c_host->enabled = 0;
}

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = SKU_UNKNOWN;

	sku_id = google_chromeec_get_board_sku();
	if (sku_id == 9)
		return "wifi_sar-foob360.hex";
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}
