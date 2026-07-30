/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	config->cnvi_bt_audio_offload = fw_config_probe(FW_CONFIG(AUDIO,
							NAU88L25B_I2S));

	if (fw_config_probe(FW_CONFIG(MB_USBC, TC_USB3))) {
		config->tcss_ports[0] = (struct tcss_port_config)TCSS_PORT_CC1(OC0);
		config->typec_aux_bias_pads[0].pad_auxp_dc = GPP_C3;
		config->typec_aux_bias_pads[0].pad_auxn_dc = GPP_C4;
	}
}
