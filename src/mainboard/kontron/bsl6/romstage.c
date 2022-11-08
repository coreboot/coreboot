/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <fsp/soc_binding.h>
#include <pc80/mc146818rtc.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <stdint.h>
#include <string.h>
#include <variant.h>

/* Rcomp resistor */
static const u16 rcomp_resistors[3] = { 121, 75, 100 };

/* Rcomp target */
static const u16 rcomp_targets[5] = { 50, 26, 20, 20, 26 };

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *const memory_params = &mupd->FspmConfig;
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52 },
	};

	assert(sizeof(memory_params->RcompResistor) == sizeof(rcomp_resistors));
	assert(sizeof(memory_params->RcompTarget) == sizeof(rcomp_targets));

	memory_params->MemorySpdDataLen = CONFIG_DIMM_SPD_SIZE;
	get_spd_smbus(&blk);
	memory_params->MemorySpdPtr00 = (u32)blk.spd_array[0];
	memory_params->MemorySpdPtr10 = (u32)blk.spd_array[1];

	memcpy(memory_params->RcompResistor, rcomp_resistors,
	       sizeof(memory_params->RcompResistor));
	memcpy(memory_params->RcompTarget, rcomp_targets,
	       sizeof(memory_params->RcompTarget));

	memory_params->DqPinsInterleaved = true;
	memory_params->CaVrefConfig = 2;

	memory_params->UserBd = BOARD_TYPE_MOBILE;

	variant_memory_init_params(mupd);
}
