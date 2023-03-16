/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <fw_config.h>
#include <sar.h>
#include <variant/gpio.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY) {
		if (fw_config_probe(FW_CONFIG(UFC, UFC_USB)))
			acpigen_soc_clear_tx_gpio(GPP_B09);
	} else if (entry == S0IX_EXIT) {
		if (fw_config_probe(FW_CONFIG(UFC, UFC_USB)))
			acpigen_soc_set_tx_gpio(GPP_B09);
	}
}
