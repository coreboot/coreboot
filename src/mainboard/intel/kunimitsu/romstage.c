/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <gpio.h>
#include "gpio.h"
#include <soc/romstage.h>
#include "spd/spd.h"

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;

	mainboard_fill_dq_map_data(&mem_cfg->DqByteMapCh0, &mem_cfg->DqByteMapCh1);
	mainboard_fill_dqs_map_data(&mem_cfg->DqsMapCpu2DramCh0, &mem_cfg->DqsMapCpu2DramCh1);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->MemorySpdPtr00 = mainboard_get_spd_data();
	if (mainboard_has_dual_channel_mem())
		mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
	mem_cfg->MemorySpdDataLen = SPD_LEN;
}
