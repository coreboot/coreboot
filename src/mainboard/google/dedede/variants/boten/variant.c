/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <sar.h>

#define SKU_ID_BOTENFLEX 0x90000

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

	if (sku_id == SKU_ID_BOTENFLEX)
		return "wifi_sar-botenflex.hex";

	return "wifi_sar-boten.hex";
}
