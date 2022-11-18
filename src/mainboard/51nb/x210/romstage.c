/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>

static void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 121, 81, 100 };
	memcpy(rcomp_ptr, RcompResistor, sizeof(RcompResistor));
}

static void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 20, 20, 26 };
	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52 },
	};

	mem_cfg = &mupd->FspmConfig;

	get_spd_smbus(&blk);
	dump_spd_info(&blk);
	assert(blk.spd_array[0][0] != 0);

	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	mem_cfg->DqPinsInterleaved = TRUE;
	mem_cfg->CaVrefConfig = 2;
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (uintptr_t)blk.spd_array[1];
}
