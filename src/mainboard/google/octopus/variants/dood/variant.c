/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <boardid.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>
#include <soc/intel/apollolake/chip.h>

enum {
	SKU_1_LTE  = 1, /* Wifi + LTE */
	SKU_2_WIFI = 2, /* Wifi */
	SKU_3_LTE_2CAM = 3, /* Wifi + LTE + dual camera */
	SKU_4_WIFI_2CAM = 4, /* Wifi + dual camera */
};

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	switch (google_chromeec_get_board_sku()) {
	case SKU_1_LTE:
	case SKU_3_LTE_2CAM:
		power_off_lte_module();
		return;
	default:
		return;
	}
}

void variant_update_devtree(struct device *dev)
{
	struct soc_intel_apollolake_config *cfg = NULL;

	cfg = (struct soc_intel_apollolake_config *)dev->chip_info;

	if (cfg != NULL && (cfg->disable_xhci_lfps_pm != 1)) {
		switch (google_chromeec_get_board_sku()) {
		case SKU_1_LTE:
		case SKU_3_LTE_2CAM:
			cfg->disable_xhci_lfps_pm = 1;
			return;
		default:
			return;
		}
	}
}
