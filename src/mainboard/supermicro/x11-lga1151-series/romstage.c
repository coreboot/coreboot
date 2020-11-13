/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

	struct spd_block blk = {
		.addr_map = { 0x50, 0x51, 0x52, 0x53, },
	};

	mem_cfg->DqPinsInterleaved = 1;
	get_spd_smbus(&blk);
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (uintptr_t)blk.spd_array[2];
	mem_cfg->MemorySpdPtr01 = (uintptr_t)blk.spd_array[1];
	mem_cfg->MemorySpdPtr11 = (uintptr_t)blk.spd_array[3];
	mem_cfg->UserBd = BOARD_TYPE_SERVER;

	mupd->FspmTestConfig.DmiVc1 = 1;
}
