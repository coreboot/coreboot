/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/romstage.h>
#include <stdint.h>
#include <string.h>
#include <spd_bin.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const u16 rcomp_resistors[3] = { 121, 75, 100 };

	const u16 rcomp_targets[5] = { 50, 26, 20, 20, 26 };

	FSP_M_CONFIG *const mem_cfg = &mupd->FspmConfig;

	struct spd_block blk = {
		.addr_map = { 0x50, 0x51, 0x52, 0x53, },
	};

	assert(sizeof(mem_cfg->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(mem_cfg->RcompTarget)   == sizeof(rcomp_targets));

	mem_cfg->DqPinsInterleaved = 1;
	get_spd_smbus(&blk);
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (uintptr_t)blk.spd_array[2];
	mem_cfg->MemorySpdPtr01 = (uintptr_t)blk.spd_array[1];
	mem_cfg->MemorySpdPtr11 = (uintptr_t)blk.spd_array[3];
	dump_spd_info(&blk);

	memcpy(mem_cfg->RcompResistor, rcomp_resistors, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget,   rcomp_targets,   sizeof(mem_cfg->RcompTarget));

	/* use virtual channel 1 for the dmi interface of the PCH */
	mupd->FspmTestConfig.DmiVc1 = 1;

	/* desktop type */
	mem_cfg->UserBd = BOARD_TYPE_DESKTOP;
}
