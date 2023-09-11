/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <sar.h>
#include <variant/gpio.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	if (fw_config_probe(FW_CONFIG(WIFI_SAR_ID, WIFI_SAR_ID_1))) {
		printk(BIOS_INFO, "Use wifi_sar_1.hex for Intel Wi-Fi module AX211NGW.\n");
		return "wifi_sar_1.hex";
	}

	printk(BIOS_INFO, "Intel Wi-Fi SAR not used, return NULL!\n");
	return NULL;
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* Add board-specific MS0X entries */
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(CAM_PWR);
	if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(CAM_PWR);
}
