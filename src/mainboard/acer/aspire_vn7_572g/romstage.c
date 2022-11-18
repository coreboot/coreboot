/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <stdint.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *const mem_cfg = &mupd->FspmConfig;

	/* TODO: Search vendor FW for Dq/Dqs */
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52 },
	};
	const uint16_t rcomp_resistors[3] = { 121, 80, 100 };
	/* Also the default values in FSP binary */
	const uint16_t rcomp_targets[5] = { 100, 40, 40, 23, 40 };

	get_spd_smbus(&blk);
	dump_spd_info(&blk);
	assert(blk.spd_array[0][0] != 0);

	assert(sizeof(mem_cfg->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(mem_cfg->RcompTarget) == sizeof(rcomp_targets));
	memcpy(mem_cfg->RcompResistor, rcomp_resistors, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget, rcomp_targets, sizeof(mem_cfg->RcompTarget));

	mem_cfg->CaVrefConfig = 2;
	mem_cfg->DqPinsInterleaved = 1;
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];
	mem_cfg->MemorySpdPtr10 = (uintptr_t)blk.spd_array[1];

	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[1] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[1] = 1;
}
