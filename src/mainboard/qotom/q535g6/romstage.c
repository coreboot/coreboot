/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>
#include <types.h>

static void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	const u16 rcomp_resistor[] = {121, 81, 100};

	memcpy(rcomp_ptr, rcomp_resistor, sizeof(rcomp_resistor));
}

static void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	static const u16 rcomp_target[] = {100, 40, 20, 20, 26};

	memcpy(rcomp_strength_ptr, rcomp_target, sizeof(rcomp_target));
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	struct spd_block blk = {
		.addr_map = {0x50},
	};

	get_spd_smbus(&blk);
	dump_spd_info(&blk);

	mem_cfg->DqPinsInterleaved = 1;
	mem_cfg->CaVrefConfig = 2;

	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
}
