/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <stdint.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const u16 rcomp_resistors[3] = { 121, 81, 100 };

	const u16 rcomp_targets[5] = { 100, 40, 20, 20, 26 };

	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

	struct spd_block blk = {
		.addr_map = { 0x50 },
	};

	assert(sizeof(mem_cfg->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(mem_cfg->RcompTarget)   == sizeof(rcomp_targets));

	get_spd_smbus(&blk);
	dump_spd_info(&blk);
	assert(blk.spd_array[0][0] != 0);

	memcpy(mem_cfg->RcompResistor, rcomp_resistors, sizeof(mem_cfg->RcompResistor));
	memcpy(mem_cfg->RcompTarget,   rcomp_targets,   sizeof(mem_cfg->RcompTarget));

	mem_cfg->DqPinsInterleaved = TRUE;
	mem_cfg->CaVrefConfig = 2;
	mem_cfg->MemorySpdDataLen = blk.len;
	mem_cfg->MemorySpdPtr00 = (uintptr_t)blk.spd_array[0];

	/* Enable and set SATA HSIO adjustments for ports 0 and 2 */
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[0] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[2] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[0] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[2] = 1;
}
