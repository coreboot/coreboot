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

#include <gpio.h>
#include <soc/romstage.h>
#include "gpio.h"
#include "spd/spd.h"

void mainboard_memory_init_params(struct romstage_params *params,
				  MEMORY_INIT_UPD *memory_params)
{
	spd_memory_init_params(memory_params);
	mainboard_fill_dq_map_data(&memory_params->DqByteMapCh0);
	mainboard_fill_dqs_map_data(&memory_params->DqsMapCpu2DramCh0);
	mainboard_fill_rcomp_res_data(&memory_params->RcompResistor);
	mainboard_fill_rcomp_strength_data(&memory_params->RcompTarget);
	memory_params->MemorySpdDataLen = SPD_LEN;
	memory_params->DqPinsInterleaved = FALSE;
}
