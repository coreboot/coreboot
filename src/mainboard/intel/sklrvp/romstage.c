/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <ec/google/chromeec/ec.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include "spd/spd.h"

void mainboard_romstage_entry(struct romstage_params *params)
{
	post_code(0x31);
	/* Fill out PEI DATA */
	mainboard_fill_pei_data(params->pei_data);
	mainboard_fill_spd_data(params->pei_data);
	/* Initliaze memory */
	romstage_common(params);
}

void mainboard_memory_init_params(
	struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params)
{
	/* Get SPD data passing strucutre and initialize it.*/
	if (params->pei_data->spd_data[0][0][0] != 0) {
		memory_params->MemorySpdPtr00 =
				(UINT32)(params->pei_data->spd_data[0][0]);
		memory_params->MemorySpdPtr10 =
				(UINT32)(params->pei_data->spd_data[1][0]);
		printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_0_0\n",
				memory_params->MemorySpdPtr00);
		printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_0_1\n",
				memory_params->MemorySpdPtr01);
		printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_1_0\n",
				memory_params->MemorySpdPtr10);
		printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_1_1\n",
				memory_params->MemorySpdPtr11);
	}
	/*
	* Configure the DQ/DQS settings if required. In general the settings
	* should be set in the FSP flash image and should not need to be
	* changed.
	*/
	memcpy(memory_params->DqByteMapCh0, params->pei_data->dq_map[0],
			sizeof(params->pei_data->dq_map[0]));
	memcpy(memory_params->DqByteMapCh1, params->pei_data->dq_map[1],
			sizeof(params->pei_data->dq_map[1]));
	memcpy(memory_params->DqsMapCpu2DramCh0, params->pei_data->dqs_map[0],
			sizeof(params->pei_data->dqs_map[0]));
	memcpy(memory_params->DqsMapCpu2DramCh1, params->pei_data->dqs_map[1],
			sizeof(params->pei_data->dqs_map[1]));
	memcpy(memory_params->RcompResistor, params->pei_data->RcompResistor,
			sizeof(params->pei_data->RcompResistor));
	memcpy(memory_params->RcompTarget, params->pei_data->RcompTarget,
			sizeof(params->pei_data->RcompTarget));

	/* update spd length*/
	memory_params->MemorySpdDataLen = SPD_LEN;
	memory_params->DqPinsInterleaved = FALSE;
}
