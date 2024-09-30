/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>
#include <static.h>

void variant_devtree_update(void)
{
	uint32_t sku_id;
	struct device *emmc_host;
	struct device *ssd_host;
	config_t *cfg = config_of_soc();
	emmc_host = pcidev_path_on_root(PCH_DEVFN_EMMC);
	ssd_host = pcidev_path_on_root(PCH_DEVFN_SATA);

	/* SKU ID 2 and 4 do not have eMMC, hence disable it. */
	sku_id = google_chromeec_get_board_sku();
	if ((sku_id == 2) || (sku_id == 4)) {
		if (emmc_host == NULL)
			return;
		emmc_host->enabled = 0;
		cfg->ScsEmmcHs400Enabled = 0;
	}

	/* SKU ID 1 and 3 do not have SSD, hence disable it. */
	if ((sku_id == 1) || (sku_id == 3)) {
		if (ssd_host == NULL)
			return;
		ssd_host->enabled = 0;
		cfg->SataSalpSupport = 0;
		cfg->SataPortsEnable[1] = 0;
		cfg->SataPortsDevSlp[1] = 0;
		cfg->satapwroptimize = 0;
	}
}
