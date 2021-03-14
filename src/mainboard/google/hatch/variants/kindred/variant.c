/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>
#include <drivers/intel/gma/opregion.h>

void variant_devtree_update(void)
{
	uint32_t sku_id;
	struct device *emmc_host;
	struct device *ssd_host;
	config_t *cfg = config_of_soc();
	emmc_host = pcidev_path_on_root(PCH_DEVFN_EMMC);
	ssd_host = pcidev_path_on_root(PCH_DEVFN_SATA);

	/* SKU ID 1/3/23/24 doesn't have a eMMC device, hence disable it. */
	sku_id = google_chromeec_get_board_sku();
	if (sku_id == 1 || sku_id == 3 || sku_id == 23 || sku_id == 24) {
		if (emmc_host == NULL)
			return;
		emmc_host->enabled = 0;
		cfg->ScsEmmcHs400Enabled = 0;
	}
	/* SKU ID 2/4/21/22 doesn't have a SSD device, hence disable it. */
	if (sku_id == 2 || sku_id == 4 || sku_id == 21 || sku_id == 22) {
		if (ssd_host == NULL)
			return;
		ssd_host->enabled = 0;
		cfg->SataSalpSupport = 0;
		cfg->SataMode = 0;
		cfg->SataPortsEnable[1] = 0;
		cfg->SataPortsDevSlp[1] = 0;
		cfg->satapwroptimize = 0;
	}
}

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 1 || sku_id == 2 || sku_id == 3 || sku_id == 4)
		return "wifi_sar-kled.hex";
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 1 || sku_id == 2 || sku_id == 3 || sku_id == 4)
		return "vbt-kled.bin";
	else
		return "vbt.bin";
}
