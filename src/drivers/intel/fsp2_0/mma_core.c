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

#include <console/console.h>
#include <fsp/util.h>
#include <fsp/soc_binding.h>

static const uint8_t mma_results_uuid[16] = { 0x28, 0xe9, 0xf4, 0x08,
			0x5f, 0x0f, 0xd4, 0x46,
			0x84, 0x10, 0x47, 0x9f, 0xda, 0x27, 0x9d, 0xb6 };

int fsp_locate_mma_results(const void **mma_hob, size_t *mma_hob_size)
{
	*mma_hob_size = 0;
	*mma_hob = fsp_find_extension_hob_by_guid(mma_results_uuid,
				mma_hob_size);

	if (!(*mma_hob_size) || !(*mma_hob))
		return -1;
	return 0;
}

void setup_mma(FSP_M_CONFIG *memory_cfg)
{
	struct mma_config_param mma_cfg;

	if (mma_locate_param(&mma_cfg)) {
		printk(BIOS_DEBUG, "MMA: set up failed\n");
		return;
	}

	soc_update_memory_params_for_mma(memory_cfg, &mma_cfg);
	printk(BIOS_DEBUG, "MMA: set up completed successfully\n");
}
