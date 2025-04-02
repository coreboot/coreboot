/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <fsp/api.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* CNVi */
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;

		if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_MAX98360_ALC5682I_I2S)) ||
			fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC722_SNDW)) ||
			fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC721_SNDW))) {
			printk(BIOS_INFO, "BT audio offload configured.\n");
			config->cnvi_bt_audio_offload = true;
		}
	}
}

void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	/* HDA Audio */
	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256_HDA))) {
		printk(BIOS_INFO, "Overriding HDA SDI lanes.\n");
		m_cfg->PchHdaSdiEnable[0] = true;
		m_cfg->PchHdaSdiEnable[1] = false;
	}
}
