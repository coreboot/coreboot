/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <compiler.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>
#include <chip.h>

/* All FSP specific code goes in this block */
void mainboard_romstage_entry(struct romstage_params *rp)
{
	struct pei_data *ps = rp->pei_data;

	mainboard_fill_spd_data(ps);

	/* Call back into chipset code with platform values updated. */
	romstage_common(rp);
}

void mainboard_memory_init_params(struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params)
{
	/* Update SPD data */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_CYAN)) {
		memory_params->PcdMemoryTypeEnable = MEM_DDR3;
		memory_params->PcdMemorySpdPtr =
				(u32)params->pei_data->spd_data_ch0;
	} else
		memory_params->PcdMemoryTypeEnable = MEM_LPDDR3;

	memory_params->PcdMemChannel0Config = params->pei_data->spd_ch0_config;
	memory_params->PcdMemChannel1Config = params->pei_data->spd_ch1_config;

	/* Variant-specific memory params */
	variant_memory_init_params(memory_params);
}

__weak
void variant_memory_init_params(MEMORY_INIT_UPD *memory_params)
{
}
