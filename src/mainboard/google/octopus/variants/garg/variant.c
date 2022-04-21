/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <ec/google/chromeec/ec.h>
#include <drivers/intel/gma/opregion.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/sku.h>
#include <soc/intel/apollolake/chip.h>

const char *mainboard_vbt_filename(void)
{
	uint32_t sku_id;

	sku_id = google_chromeec_get_board_sku();

	switch (sku_id) {
	case SKU_9_HDMI:
	case SKU_19_HDMI_TS:
		return "vbt_garg_hdmi.bin";
	case SKU_49_2A2C_TS:
	case SKU_51_2A2C:
		return "vbt_garfour.bin";
	case SKU_50_HDMI:
	case SKU_52_HDMI_TS:
		return "vbt_garfour_hdmi.bin";
	default:
		return "vbt.bin";
	}
}

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	switch (google_chromeec_get_board_sku()) {
	case SKU_17_LTE:
	case SKU_18_LTE_TS:
	case SKU_39_1A2C_360_LTE_TS_NO_STYLUES:
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
		case SKU_17_LTE:
		case SKU_18_LTE_TS:
		case SKU_39_1A2C_360_LTE_TS_NO_STYLUES:
			cfg->disable_xhci_lfps_pm = 1;
			return;
		default:
			return;
		}
	}
}
