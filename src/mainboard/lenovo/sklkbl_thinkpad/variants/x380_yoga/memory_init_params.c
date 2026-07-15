/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/romstage.h>
#include "../../spd/spd.h"

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;

	mem_cfg->CaVrefConfig      = 2;		/* VREF_CA to CH_A and VREF_DQ_B to CH_B */
	mem_cfg->DqPinsInterleaved = false;	/* DDR_DQ in non-interleave mode */
	mem_cfg->MemorySpdDataLen  = CONFIG_DIMM_SPD_SIZE;

	/* Memory is soldered down; MEMORYID straps unpopulated, SPD fixed */
	mem_cfg->MemorySpdPtr00    = (uintptr_t)mainboard_find_spd_data(0);
	mem_cfg->MemorySpdPtr10    = mem_cfg->MemorySpdPtr00;
}
