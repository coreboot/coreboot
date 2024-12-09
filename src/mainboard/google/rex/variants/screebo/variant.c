/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <ec/google/chromeec/ec.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_SAR_ID));
}

void variant_update_soc_chip_config(struct soc_intel_meteorlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, ALC1019_ALC5682I_I2S)))
		config->cnvi_bt_audio_offload = true;

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

const struct cpu_tdp_power_limits variant_perf_efficient_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 84000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.power_limits_index = MTL_P_282_242_CORE,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 84000
	},
};

const struct cpu_tdp_power_limits variant_power_efficient_limits[] = {
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_2,
		.cpu_tdp = 15,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 47000
	},
	{
		.mch_id = PCI_DID_INTEL_MTL_P_ID_5,
		.cpu_tdp = 15,
		.pl1_min_power = 10000,
		.pl1_max_power = 15000,
		.pl2_min_power = 40000,
		.pl2_max_power = 40000,
		.pl4_power = 47000
	},
};

void variant_devtree_update(void)
{
	const struct cpu_tdp_power_limits *limits = variant_perf_efficient_limits;
	size_t limits_size = ARRAY_SIZE(variant_perf_efficient_limits);

	/*
	 * If battery is not present or battery level is at or below critical threshold
	 * to boot a platform with the performance efficient configuration, boot with
	 * the power optimized configuration.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC)) {
		if (!google_chromeec_is_battery_present_and_above_critical_threshold()) {
			limits = variant_power_efficient_limits;
			limits_size = ARRAY_SIZE(variant_power_efficient_limits);
		}
	}

	variant_update_cpu_power_limits(limits, limits_size);
}
