/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <sar.h>

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	power_off_lte_module();
}

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id != 4)
		filename = "wifi_sar-fleex.hex";

	return filename;
}
