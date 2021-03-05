/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>
#include <soc/intel/apollolake/chip.h>

#define MIN_LTE_SKU         4
#define FLEEX_MAX_SKU         0x20

static bool is_lte_sku(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	/* Only Fleex has LTE sku */
	if (sku_id <= FLEEX_MAX_SKU)
		return (sku_id >= MIN_LTE_SKU);

	return false;
}

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;
	if (is_lte_sku())
		power_off_lte_module();
}

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;

	if (!is_lte_sku())
		filename = "wifi_sar-fleex.hex";

	return filename;
}

void variant_update_devtree(struct device *dev)
{
	struct soc_intel_apollolake_config *cfg = NULL;

	cfg = (struct soc_intel_apollolake_config *)dev->chip_info;
	// Force disable_xhci_lfps_pm to update if it is LTE sku
	if (cfg != NULL && is_lte_sku())
		cfg->disable_xhci_lfps_pm = 1;
}
