/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include "spd/spd_util.c"
#include "spd/spd.h"
#include <ec/acpi/ec.h>
#include <stdint.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;

	/* Use the correct entry in the SPD table defined in Makefile.inc */
	u8 spd_index = 6;
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
