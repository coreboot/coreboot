/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>
#include <intelblocks/graphics.h>

/* Per-pipe DDI Function Control 2 */
#define TRANS_DDI_FUNC_CTL2_A					0x60404
#define TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS		(3 << 6)

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	/*
	* SOC Aux orientation override:
	* This is a bitfield that corresponds to up to 4 TCSS ports.
	* Bits (0,1) allocated for TCSS Port1 configuration and Bits (2,3)for TCSS Port2.
	* Bit0, Bit2 set to "0" indicates has retimer on TCSS Port.
	* Bit0, Bit2 set to "1" indicates no retimer on TCSS Port.
	* Bit1, Bit3 set to "0" indicates Aux lines are not swapped on TCSS Port.
	* Bit1, Bit3 set to "1" indicates Aux lines are swapped on TCSS Port.
	*/

	if (fw_config_probe(FW_CONFIG(DB_AUX_BIAS, SOC))) {
		printk(BIOS_INFO, "DB DP AUX BIAS connect to SOC.\n");
		config->tcss_aux_ori = 5;
		config->typec_aux_bias_pads[0].pad_auxp_dc = GPP_A19;
		config->typec_aux_bias_pads[0].pad_auxn_dc = GPP_A20;
		config->typec_aux_bias_pads[1].pad_auxp_dc = GPP_E22;
		config->typec_aux_bias_pads[1].pad_auxn_dc = GPP_E23;
	} else {
		printk(BIOS_INFO, "DB DP AUX BIAS connect to redriver IC.\n");
		config->tcss_aux_ori = 4;
		config->typec_aux_bias_pads[1].pad_auxp_dc = GPP_E22;
		config->typec_aux_bias_pads[1].pad_auxn_dc = GPP_E23;
	}
}

void variant_finalize(void)
{
	/*
	 * Panel CSOT MNB601LS1-3 will flicker once during enter Chrome login screen,
	 * it is because it inserts 12 blank frames if it receives the unmute in VB-ID.
	 *
	 * Always override the mute in VB-ID to avoid Tcon EC detected the
	 * audiomute_flag change.
	 */
	graphics_gtt_rmw(TRANS_DDI_FUNC_CTL2_A, ~TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS,
		TRANS_DDI_AUDIO_MUTE_OVERRIDE_BITS_FIELDS);
}
