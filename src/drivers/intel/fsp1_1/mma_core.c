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
#include <fsp/romstage.h>
#include <fsp/soc_binding.h>

#define FSP_MMA_RESULTS_GUID	{ 0x8f4e928, 0xf5f, 0x46d4, \
		{ 0x84, 0x10, 0x47, 0x9f, 0xda, 0x27, 0x9d, 0xb6 } }

int fsp_locate_mma_results(const void **mma_hob, size_t *mma_hob_size)
{
	const void *mma_hob_start;
	const EFI_GUID mma_results_guid = FSP_MMA_RESULTS_GUID;

	mma_hob_start = get_first_guid_hob(&mma_results_guid);
	if (!mma_hob_start)
		return -1;
	*mma_hob = GET_GUID_HOB_DATA(mma_hob_start);
	*mma_hob_size = GET_HOB_LENGTH(mma_hob);

	if (!(*mma_hob_size) || !(*mma_hob))
		return -1;
	return 0;
}

void setup_mma(MEMORY_INIT_UPD *memory_cfg)
{
	struct mma_config_param mma_cfg;

	if (mma_locate_param(&mma_cfg)) {
		printk(BIOS_DEBUG, "MMA: set up failed\n");
		return;
	}

	soc_update_memory_params_for_mma(memory_cfg, &mma_cfg);
	printk(BIOS_DEBUG, "MMA: set up completed successfully\n");
}
