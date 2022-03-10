/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>
#include "board_id.h"

#define SPD_ID_MASK 0x7

static size_t get_spd_index(void)
{
	uint8_t board_id = get_board_id();
	size_t spd_index;

	printk(BIOS_INFO, "board id is 0x%x\n", board_id);

	spd_index = board_id & SPD_ID_MASK;

	printk(BIOS_INFO, "SPD index is 0x%x\n", (unsigned int)spd_index);
	return spd_index;
}

/*
 * ADL-P silicon can support 7 SRC CLK's and 10 CLKREQ signals. Out of 7 SRCCLK's
 * 3 will be used for CPU, the rest are for PCH. If more than 4 PCH devices are
 * connected on the platform, an external differential buffer chip needs to be placed at
 * the platform level.
 *
 * GEN3_EXTERNAL_CLOCK_BUFFER Kconfig is selected for ADL-P RVP (not applicable for
 * ADL-M/N RVP)
 *
 * CONFIG_CLKSRC_FOR_EXTERNAL_BUFFER provides the CLKSRC that feed clock to discrete
 * buffer for further distribution to platform.
 */
static void configure_external_clksrc(FSP_M_CONFIG *m_cfg)
{
	for (unsigned int i = CONFIG_MAX_PCIE_CLOCK_SRC; i < CONFIG_MAX_PCIE_CLOCK_REQ; i++)
		m_cfg->PcieClkSrcUsage[i] = CONFIG_CLKSRC_FOR_EXTERNAL_BUFFER;
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;
	const struct mb_cfg *mem_config = variant_memory_params();
	int board_id = get_board_id();
	const bool half_populated = false;
	bool dimms_changed = false;

	const struct mem_spd memory_down_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = get_spd_index(),
	};

	const struct mem_spd dimm_module_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
				.addr_dimm[1] = 0x51,
			},
			[1] = {
				.addr_dimm[0] = 0x52,
				.addr_dimm[1] = 0x53,
			},
		},
	};

	switch (board_id) {
	case ADL_P_DDR4_1:
	case ADL_P_DDR4_2:
	case ADL_P_DDR5_1:
		memcfg_init(memupd, mem_config, &dimm_module_spd_info, half_populated,
				&dimms_changed);
		break;
	case ADL_P_DDR5_2:
	case ADL_P_LP4_1:
	case ADL_P_LP4_2:
	case ADL_P_LP5_1:
	case ADL_P_LP5_2:
	case ADL_M_LP4:
	case ADL_M_LP5:
	case ADL_N_LP5:
		memcfg_init(memupd, mem_config, &memory_down_spd_info, half_populated,
				&dimms_changed);
		break;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}

	if (CONFIG(GEN3_EXTERNAL_CLOCK_BUFFER))
		configure_external_clksrc(m_cfg);
}
