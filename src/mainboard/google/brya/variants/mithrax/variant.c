/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(DB_USB, USB3_PS8815))) {
		config->typec_aux_bias_pads[2].pad_auxp_dc = GPP_A19;
		config->typec_aux_bias_pads[2].pad_auxn_dc = GPP_A20;
		config->tcss_aux_ori = 0x10;
	}
}
