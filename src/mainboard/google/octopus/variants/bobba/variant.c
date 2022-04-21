/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <sar.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <ec/google/chromeec/ec.h>
#include <soc/intel/apollolake/chip.h>

enum {
	SKU_37_DROID = 37, /* LTE */
	SKU_38_DROID = 38, /* LTE + Touch */
	SKU_39_DROID = 39, /* LTE + KB backlight*/
	SKU_40_DROID = 40, /* LTE + Touch + KB backlight*/
};

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id >= 33 && sku_id <= 44)
		return "wifi_sar-droid.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	switch (google_chromeec_get_board_sku()) {
	case SKU_37_DROID:
	case SKU_38_DROID:
	case SKU_39_DROID:
	case SKU_40_DROID:
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
		case 37:
		case 38:
		case 39:
		case 40:
			cfg->disable_xhci_lfps_pm = 1;
			return;
		default:
			return;
		}
	}
}
