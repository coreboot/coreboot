/* SPDX-License-Identifier: GPL-2.0-only */
#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>
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
	const struct mb_cfg *mem_config = variant_memory_params();
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = mainboard_get_spd_index(),
	};
	bool half_populated = false;

	memcfg_init(mupd, mem_config, &spd_info, half_populated);

}
