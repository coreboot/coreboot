/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

void variant_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
    /* SOC Aux orientation override:
	* This is a bitfield that corresponds to up to 4 TCSS ports.
	* Bits (0,1) allocated for TCSS Port1 configuration and Bits (2,3)for TCSS Port2.
	* TcssAuxOri = 0101b
	* Bit0,Bit2 set to "1" indicates no retimer on USBC Ports
	* Bit1,Bit3 set to "0" indicates Aux lines are not swapped on the
	* motherboard to USBC connector
    */
	if (fw_config_probe(FW_CONFIG(MB_CONFIG, MB_TYPEC))) {
		config->typec_aux_bias_pads[1].pad_auxp_dc = GPP_C16;
		config->typec_aux_bias_pads[1].pad_auxn_dc = GPP_C17;
		config->tcss_aux_ori = 0x04;
	}
}
