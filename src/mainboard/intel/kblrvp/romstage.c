/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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
#include <gpio.h>
#include "gpio.h"
#include <soc/romstage.h>
#include <soc/gpio.h>
#include "spd/spd.h"
#include <string.h>
#include <spd_bin.h>
#include "board_id.h"


void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg;
	mem_cfg = &mupd->FspmConfig;
	u8 spd_index = (get_board_id() >> 5) & 0x7;

	printk(BIOS_INFO, "SPD index %d\n", spd_index);

	mainboard_fill_dq_map_data(&mem_cfg->DqByteMapCh0);
	mainboard_fill_dqs_map_data(&mem_cfg->DqsMapCpu2DramCh0);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	if (IS_ENABLED(CONFIG_BOARD_INTEL_KBLRVP3)) {
		struct region_device spd_rdev;

		mem_cfg->DqPinsInterleaved = 0;
		if (get_spd_cbfs_rdev(&spd_rdev, spd_index) < 0)
			die("spd.bin not found\n");
		mem_cfg->MemorySpdDataLen = region_device_sz(&spd_rdev);
		/* Memory leak is ok since we have memory mapped boot media */
		mem_cfg->MemorySpdPtr00 = (uintptr_t)rdev_mmap_full(&spd_rdev);
	} else {  /* for CONFIG_BOARD_INTEL_KBLRVP7 */
		struct spd_block blk;

		mem_cfg->DqPinsInterleaved = 1;
		get_spd_smbus(&blk);
		mem_cfg->MemorySpdDataLen = blk.len;
		mem_cfg->MemorySpdPtr00 = (u32)blk.spd_array[0];
	}
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
}
