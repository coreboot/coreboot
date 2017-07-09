/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2017 Intel Corp.
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

#include <arch/byteorder.h>
#include <cbfs.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include "spd/spd.h"
#include <string.h>
#include <spd_bin.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;
	u8 spd_index;

	mainboard_fill_dq_map_ch0(&mem_cfg->DqByteMapCh0);
	mainboard_fill_dq_map_ch1(&mem_cfg->DqByteMapCh1);
	mainboard_fill_dqs_map_ch0(&mem_cfg->DqsMapCpu2DramCh0);
	mainboard_fill_dqs_map_ch1(&mem_cfg->DqsMapCpu2DramCh1);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	if (IS_ENABLED(CONFIG_BOARD_INTEL_CANNONLAKE_RVPU)) {
		mem_cfg->DqPinsInterleaved = 1;
		mem_cfg->CaVrefConfig = 2; /* VREF_CA->CHA VREF_DQ_B->CHB */
		spd_index = 1;
	} else { /* For CONFIG_BOARD_INTEL_CANNONLAKE_RVPY */
		mem_cfg->DqPinsInterleaved = 0;
		mem_cfg->CaVrefConfig = 0; /* VREF_CA->CHA/CHB */
		mem_cfg->ECT = 1; /* Early Command Training Enabled */
		spd_index = 2;
	}

	printk(BIOS_DEBUG,"SPD INDEX =0x%u\n", spd_index);

	struct region_device spd_rdev;

	if (get_spd_cbfs_rdev(&spd_rdev, spd_index) < 0)
		die("spd.bin not found\n");

	mem_cfg->MemorySpdDataLen = region_device_sz(&spd_rdev);
	/* Memory leak is ok since we have memory mapped boot media */
	mem_cfg->MemorySpdPtr00 = (uintptr_t)rdev_mmap_full(&spd_rdev);
	mem_cfg->RefClk = 0; /* Auto Select CLK freq */
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
}
