/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;

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
