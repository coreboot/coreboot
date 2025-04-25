/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>
#include <intelblocks/graphics.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <variant/gpio.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_WIFI6E))
		|| fw_config_probe(FW_CONFIG(WIFI, WIFI_UNKNOWN))) {
		printk(BIOS_INFO, "CNVi bluetooth enabled by fw_config\n");
		config->cnvi_bt_core = true;
		config->cnvi_bt_audio_offload = true;
	} else {
		printk(BIOS_INFO, "CNVi bluetooth disabled by fw_config\n");
		config->cnvi_bt_core = false;
		config->cnvi_bt_audio_offload = false;
	}
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(SLP_S0_HDMI_EN);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(SLP_S0_HDMI_EN);
}
