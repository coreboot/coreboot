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

#include <console/console.h>
#include <fsp/api.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include "spd/spd.h"
#include <spd_bin.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6776/nct6776.h>

#define SERIAL_DEV PNP_DEV(0x2e, NCT6776_SP1)

void car_mainboard_pre_console_init(void)
{
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_romstage_entry(struct romstage_params *params)
{
	post_code(0x31);
	/* Fill out PEI DATA */
	mainboard_fill_pei_data(params->pei_data);
	romstage_common(params);
}

void mainboard_memory_init_params(
	struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params)
{
	struct spd_block blk = {
		.addr_map = { 0x50, 0x52, },
	};

	get_spd_smbus(&blk);
	dump_spd_info(&blk);
	printk(BIOS_SPEW, "spd block length: 0x%08x\n", blk.len);

	memory_params->MemorySpdPtr00 = (UINT32) blk.spd_array[0];
	memory_params->MemorySpdPtr10 = (UINT32) blk.spd_array[2];
	printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_0_0\n",
		memory_params->MemorySpdPtr00);
	printk(BIOS_SPEW, "0x%08x: SpdDataBuffer_1_0\n",
		memory_params->MemorySpdPtr10);

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
	memory_params->MemorySpdDataLen = blk.len;
	memory_params->DqPinsInterleaved = TRUE;
}
