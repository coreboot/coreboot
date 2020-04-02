/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
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
#include "spd/spd.h"

static uintptr_t mainboard_get_spd_index(void)
{
	uint8_t board_id = (get_board_id() & 0xFF);
	int spd_index;

	printk(BIOS_INFO, "board id is 0x%x\n", board_id);

	switch (board_id) {
	case TGL_UP3_LP4_MICRON:
	case TGL_UP4_LP4_MICRON:
		spd_index = SPD_ID_MICRON;
		break;
	case TGL_UP3_LP4_SAMSUNG:
	case TGL_UP4_LP4_SAMSUNG:
		spd_index = SPD_ID_SAMSUNG;
		break;
	case TGL_UP3_LP4_HYNIX:
	case TGL_UP4_LP4_HYNIX:
		spd_index = SPD_ID_HYNIX;
		break;
	default:
		spd_index = SPD_ID_MICRON;
		printk(BIOS_WARNING, "Invalid board_id 0x%x\n", board_id);
	}

	printk(BIOS_INFO, "SPD index is 0x%x\n", spd_index);
	return spd_index;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

	const struct lpddr4x_cfg *mem_config = variant_memory_params();
	const struct spd_info spd_info = {
		.topology = MEMORY_DOWN,
		.md_spd_loc = SPD_CBFS,
		.cbfs_index = mainboard_get_spd_index(),
	};
	bool half_populated = false;

	meminit_lpddr4x(mem_cfg, mem_config, &spd_info, half_populated);

}
