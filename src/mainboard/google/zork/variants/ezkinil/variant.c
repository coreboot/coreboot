/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>

static int sku_has_emmc(void)
{
	uint32_t board_sku = sku_id();

	/* Factory flow requires all OS boot media to be enabled. */
	if (boot_is_factory_unprovisioned())
		return 1;

	if ((board_sku == 0x5A020000) ||
	    (board_sku == 0x5A020001) || (board_sku == 0x5A020002) ||
	    (board_sku == 0x5A020005) || (board_sku == 0x5A020006) ||
	    (board_sku == 0x5A020009) || (board_sku == 0x5A02000A) ||
	    (board_sku == 0x5A02000D) || (board_sku == 0x5A02000E))
		return 1;

	return 0;
}

void variant_devtree_update(void)
{
	struct soc_amd_picasso_config *cfg;

	cfg = config_of_soc();

	if (!sku_has_emmc())
		cfg->sd_emmc_config = SD_EMMC_DISABLE;
}
