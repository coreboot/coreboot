/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include "board_id.h"
#include "spd/spd.h"

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	u8 spd_index = (get_board_id() & 0x1F) & 0x7;
	printk(BIOS_DEBUG, "spd index is 0x%x\n", spd_index);

	if (spd_index > 0 && spd_index != 2) {
		mem_cfg->MemorySpdDataLen = CONFIG_DIMM_SPD_SIZE;

		/* Memory leak is ok since we have memory mapped boot media */
		mem_cfg->MemorySpdPtr00 = spd_cbfs_map(spd_index);
		if (!mem_cfg->MemorySpdPtr00)
			die("spd.bin not found\n");
		mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;

		mem_cfg->SpdAddressTable[0] = 0x0;
		mem_cfg->SpdAddressTable[1] = 0x0;
		mem_cfg->SpdAddressTable[2] = 0x0;
		mem_cfg->SpdAddressTable[3] = 0x0;
	} else {
		mem_cfg->MemorySpdPtr00 = 0;
		mem_cfg->MemorySpdPtr01 = 0;
		mem_cfg->MemorySpdPtr10 = 0;
		mem_cfg->MemorySpdPtr11 = 0;

		mem_cfg->SpdAddressTable[0] = 0xA0;
		mem_cfg->SpdAddressTable[1] = 0xA2;
		mem_cfg->SpdAddressTable[2] = 0xA4;
		mem_cfg->SpdAddressTable[3] = 0xA6;
	}
	mem_cfg->DqPinsInterleaved = 0;
	mem_cfg->CaVrefConfig = 0x2; /* VREF_CA->CHA/CHB */
	mem_cfg->ECT = 1; /* Early Command Training Enabled */
	mem_cfg->RefClk = 0; /* Auto Select CLK freq */

	mainboard_fill_dq_map_ch0(&mem_cfg->DqByteMapCh0);
	mainboard_fill_dq_map_ch1(&mem_cfg->DqByteMapCh1);
	mainboard_fill_dqs_map_ch0(&mem_cfg->DqsMapCpu2DramCh0);
	mainboard_fill_dqs_map_ch1(&mem_cfg->DqsMapCpu2DramCh1);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);
}
