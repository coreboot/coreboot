/* SPDX-License-Identifier: GPL-2.0-only */

#include "chip.h"
#include "fsp_m_params.h"

void platform_fsp_memory_init_params_cb_sub(FSP_M_CONFIG *mcfg,
					    const struct soc_amd_cezanne_config *config)
{
	if (config->edp_phy_override) {
		mcfg->edp_phy_override = config->edp_phy_override;
		mcfg->edp_physel = config->edp_physel;
		mcfg->dp_vs_pemph_level = config->edp_tuningset.dp_vs_pemph_level;
		mcfg->tx_eq_main = config->edp_tuningset.tx_eq_main;
		mcfg->tx_eq_pre = config->edp_tuningset.tx_eq_pre;
		mcfg->tx_eq_post = config->edp_tuningset.tx_eq_post;
		mcfg->tx_vboost_lvl = config->edp_tuningset.tx_vboost_lvl;
	}
}
