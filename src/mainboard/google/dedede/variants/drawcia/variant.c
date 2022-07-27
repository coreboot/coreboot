/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>
#include <gpio.h>
#include <sar.h>
#include <ec/google/chromeec/ec.h>

enum {
	OSCINO_SKU_START   = 0x40004,
	OSCINO_SKU_END     = 0x40005,
};

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (fw_config_probe(FW_CONFIG(TABLETMODE, TABLETMODE_ENABLED))) {

		if (sku_id >= OSCINO_SKU_START && sku_id <= OSCINO_SKU_END)
			return "wifi_sar-oscino.hex";
		else
			return "wifi_sar-drawcia.hex";
	}

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI)) ||
		fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI_LTE)))
		return "vbt_drawman.bin";

	return "vbt.bin";
}

static void power_off_lte_module(void)
{
	gpio_output(GPP_H0, 0);
	gpio_output(GPP_H17, 0);
	mdelay(20);
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
