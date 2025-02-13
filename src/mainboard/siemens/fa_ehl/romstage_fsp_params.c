/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/dram/common.h>
#include <device/mmio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <string.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	static struct spd_info spd_info;
	const struct mb_cfg *board_cfg = variant_memcfg_config();

	bool half_populated = false;
	spd_info.read_type = READ_SPD_CBFS;
	spd_info.spd_spec.spd_index = 0x00;

	memcfg_init(&memupd->FspmConfig, board_cfg, &spd_info, half_populated);

	/* Enable Row-Hammer prevention */
	memupd->FspmConfig.RhPrevention = 1;
}
