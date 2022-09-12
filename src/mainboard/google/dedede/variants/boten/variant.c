/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <sar.h>

enum {
	SKU_ID_BOTEN_MIN = 0x60000,
	SKU_ID_BOTEN_MAX = 0x6ffff,
	SKU_ID_BOTENFLEX_MIN = 0x90000,
	SKU_ID_BOTENFLEX_MAX = 0x9ffff,
	SKU_ID_BOOKEM_MIN = 0x290000,
	SKU_ID_BOOKEM_MAX = 0x29ffff,
};

static void power_off_lte_module(void)
{
	gpio_output(GPP_H17, 0);
	mdelay(10);
	gpio_output(GPP_A10, 0);
}

void variant_smi_sleep(u8 slp_typ)
{
	/*
	 * Once the FW_CONFIG is provisioned, power off LTE module only under
	 * the situation where it is stuffed.
	 */
	if (slp_typ == ACPI_S5)
		power_off_lte_module();
}

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id >= SKU_ID_BOTEN_MIN && sku_id <= SKU_ID_BOTEN_MAX)
		return "wifi_sar-boten.hex";
	if (sku_id >= SKU_ID_BOTENFLEX_MIN && sku_id <= SKU_ID_BOTENFLEX_MAX)
		return "wifi_sar-botenflex.hex";
	if (sku_id >= SKU_ID_BOOKEM_MIN && sku_id <= SKU_ID_BOOKEM_MAX)
		return "wifi_sar-bookem.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(GPP_D13);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(GPP_D13);
}
