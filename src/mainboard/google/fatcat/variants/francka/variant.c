/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <fsp/api.h>
#include <fw_config.h>

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
