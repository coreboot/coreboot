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

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;
	int board_id = get_rvp_board_id();
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	pads = variant_romstage_gpio_table(&pads_num);
	if (pads_num)
		gpio_configure_pads(pads, pads_num);
	fw_config_configure_pre_mem_gpio();

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	switch (board_id) {
	case PTLP_LP5_T3_RVP:
	case PTLP_LP5_T4_RVP:
	case GCS_32GB:
	case GCS_64GB:
		memcfg_init(memupd, mem_config, &spd_info, half_populated);
		break;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}

	/* Override FSP-M UPD per board if required. */
	variant_update_soc_memory_init_params(memupd);
}
