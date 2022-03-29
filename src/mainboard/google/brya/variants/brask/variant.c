/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>
#include <variant/gpio.h>
#include <acpi/acpigen.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	config->cnvi_bt_audio_offload = fw_config_probe(FW_CONFIG(AUDIO,
							NAU88L25B_I2S));
}

void variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	if (entry == S0IX_ENTRY)
		acpigen_soc_clear_tx_gpio(NFC_POWER);
	else if (entry == S0IX_EXIT)
		acpigen_soc_set_tx_gpio(NFC_POWER);
}
