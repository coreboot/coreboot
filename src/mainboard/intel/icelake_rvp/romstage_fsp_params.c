/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
		struct region_device spd_rdev;

		if (get_spd_cbfs_rdev(&spd_rdev, spd_index) < 0)
			die("spd.bin not found\n");

		mem_cfg->MemorySpdDataLen = region_device_sz(&spd_rdev);

		/* Memory leak is ok since we have memory mapped boot media */
		mem_cfg->MemorySpdPtr00 = (uintptr_t)rdev_mmap_full(&spd_rdev);
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
