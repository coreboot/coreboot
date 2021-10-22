/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <option.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <string.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	const u16 rcomp_resistor[] = {121, 81, 100};
	const u16 rcomp_target[] = {100, 40, 20, 20, 26};

	memcpy(&mem_cfg->RcompResistor, rcomp_resistor, sizeof(rcomp_resistor));
	memcpy(&mem_cfg->RcompTarget, rcomp_target, sizeof(rcomp_target));

	mem_cfg->MemorySpdPtr00 = spd_cbfs_map(6);
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
	mem_cfg->MemorySpdDataLen = CONFIG_DIMM_SPD_SIZE;

	const uint8_t ht = get_uint_option("hyper_threading",
		mupd->FspmConfig.HyperThreading);
	mupd->FspmConfig.HyperThreading = ht;
}
