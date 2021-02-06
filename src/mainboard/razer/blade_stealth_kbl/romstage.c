/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include "spd/spd.h"
#include <ec/acpi/ec.h>
#include <stdint.h>

#define SPD_4X_2GB 0
#define SPD_4X_4GB 1

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;


#if CONFIG(BOARD_RAZER_BLADE_STEALTH_KBL_16GB)
	u8 spd_index = SPD_4X_4GB;
#else
	u8 spd_index = SPD_4X_2GB;
#endif

	printk(BIOS_INFO, "SPD index %d\n", spd_index);

	mainboard_fill_dq_map_data(&mem_cfg->DqByteMapCh0);
	mainboard_fill_dqs_map_data(&mem_cfg->DqsMapCpu2DramCh0);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->MemorySpdDataLen = CONFIG_DIMM_SPD_SIZE;
	/* Memory leak is ok since we have memory mapped boot media */
	// TODO evaluate google/eve way of loading
	mem_cfg->MemorySpdPtr00 = spd_cbfs_map(spd_index);
	if (!mem_cfg->MemorySpdPtr00)
		die("spd.bin not found\n");
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;

	mupd->FspmTestConfig.DmiVc1 = 1;
}
