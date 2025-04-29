/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <fsp/api.h>
#include <fw_config.h>
#include <intelblocks/hda.h>

void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	/* HDA Audio */
	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256M_CG_HDA))) {
		printk(BIOS_INFO, "Overriding HDA SDI lanes.\n");
		m_cfg->PchHdaSdiEnable[0] = true;
		m_cfg->PchHdaSdiEnable[1] = false;
	}
}

/*
 * HDA verb table loading is supported based on the firmware configuration.
 *
 * This function determines if the current platform has an HDA codec enabled by
 * examining the `FW_CONFIG` value. Specifically, it checks if the
 * `FW_CONFIG` includes the `AUDIO_ALC256M_CG_HDA` value, which is used to identify
 * Francka SKUs with HDA codec support.
 *
 * Return true if the `FW_CONFIG` indicates HDA support (i.e., contains
 * `AUDIO_ALC256M_CG_HDA`), false otherwise.
 */
bool mainboard_is_hda_codec_enabled(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256M_CG_HDA)))
		return true;

	return false;
}
