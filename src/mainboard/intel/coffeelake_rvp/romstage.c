/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2018 Intel Corp.
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

	mainboard_fill_dq_map_ch0(&mem_cfg->DqByteMapCh0);
	mainboard_fill_dq_map_ch1(&mem_cfg->DqByteMapCh1);
	mainboard_fill_dqs_map_ch0(&mem_cfg->DqsMapCpu2DramCh0);
	mainboard_fill_dqs_map_ch1(&mem_cfg->DqsMapCpu2DramCh1);
	mainboard_fill_rcomp_res_data(&mem_cfg->RcompResistor);
	mainboard_fill_rcomp_strength_data(&mem_cfg->RcompTarget);

	mem_cfg->DqPinsInterleaved = 1;
	mem_cfg->CaVrefConfig = 2; /* VREF_CA->CHA/CHB */
	mem_cfg->ECT = 1; /* Early Command Training Enabled */

	/* Setting standard SPD addresses */
	mem_cfg->SpdAddressTable[0] = 0xA0;
	mem_cfg->SpdAddressTable[1] = 0xA2;
	mem_cfg->SpdAddressTable[2] = 0xA4;
	mem_cfg->SpdAddressTable[3] = 0xA6;
}
