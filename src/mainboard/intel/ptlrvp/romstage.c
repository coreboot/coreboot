/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/intel/board_id.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>

/*
 * Placeholder to configure GPIO early from romstage relying on the FW_CONFIG.
 *
 * If any platform would like to override early GPIOs, they should override from
 * the variant directory.
 */
__weak void fw_config_configure_pre_mem_gpio(void)
{
	/* Nothing to do */
}

__weak void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	/* Nothing to do */
}

static void update_ddr5_sagv_points(FSP_M_CONFIG *m_cfg)
{
	int board_id = get_rvp_board_id();

	if (board_id != PTLP_DDR5_RVP)
		return;

	m_cfg->SaGvFreq[0] = 3200;
	m_cfg->SaGvGear[0] = GEAR_4;

	m_cfg->SaGvFreq[1] = 4800;
	m_cfg->SaGvGear[1] = GEAR_4;

	m_cfg->SaGvFreq[2] = 5600;
	m_cfg->SaGvGear[2] = GEAR_4;

	m_cfg->SaGvFreq[3] = 6400;
	m_cfg->SaGvGear[3] = GEAR_4;
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	pads = variant_romstage_gpio_table(&pads_num);
	if (pads_num)
		gpio_configure_pads(pads, pads_num);
	fw_config_configure_pre_mem_gpio();

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	memcfg_init(memupd, mem_config, &spd_info, half_populated);

	/* Override FSP-M SaGv frequency and gear for DDR5 boards */
	update_ddr5_sagv_points(&memupd->FspmConfig);

	/* Override FSP-M UPD per board if required. */
	variant_update_soc_memory_init_params(memupd);
}
