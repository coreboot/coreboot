/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <baseboard/variants.h>
#include <fw_config.h>
#include <sar.h>
#include <variant/gpio.h>

#define TCP2_AUX_SHIFT		4
#define TCP_AUX_MASK		0x03
#define TCP_AUX_FOLLOW_CC1	0x01

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

void variant_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_I2S)) ||
	    fw_config_probe(FW_CONFIG(AUDIO, MAX98363_CS42L42_SNDW))) {
		config->cnvi_bt_audio_offload = true;
	}

	if (fw_config_probe(FW_CONFIG(DB_USB, USB4_KB8010)) ||
	    fw_config_probe(FW_CONFIG(DB_USB, USB4_ANX7452)) ||
	    fw_config_probe(FW_CONFIG(DB_USB, USB4_ANX7452_V2))) {
		/*
		 * Configure TCP2 for
		 * "SBU orientation not handled by external retimer"
		 */
		config->tcss_aux_ori &= ~(TCP_AUX_MASK << TCP2_AUX_SHIFT);
		config->tcss_aux_ori |= TCP_AUX_FOLLOW_CC1 << TCP2_AUX_SHIFT;
	}
}
