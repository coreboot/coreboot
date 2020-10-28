/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>
#include <cbfs.h>
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

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	int board_id = get_board_id();
	const bool half_populated = false;

	const struct spd_info lp4_lp5_spd_info = {
		.read_type = READ_SPD_CBFS,
		.spd_spec.spd_index = get_spd_index(),
	};

	const struct spd_info ddr4_ddr5_spd_info = {
		.read_type = READ_SMBUS,
		.spd_spec = {
			.spd_smbus_address = {
				[0] = 0xa0,
				[1] = 0xa2,
				[8] = 0xa4,
				[9] = 0xa6,
			},
		},
	};

	switch (board_id) {
	case ADL_P_DDR4_1:
	case ADL_P_DDR4_2:
	case ADL_P_DDR5:
		memcfg_init(&mupd->FspmConfig, mem_config, &ddr4_ddr5_spd_info, half_populated);
		break;
	case ADL_P_LP4_1:
	case ADL_P_LP4_2:
	case ADL_P_LP5:
		memcfg_init(&mupd->FspmConfig, mem_config, &lp4_lp5_spd_info, half_populated);
		break;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}
}
