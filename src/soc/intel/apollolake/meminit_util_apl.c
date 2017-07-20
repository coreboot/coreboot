/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <soc/meminit.h>
#include <string.h>

void save_lpddr4_dimm_info(const struct lpddr4_cfg *lp4cfg, size_t mem_sku)
{
	int channel, dimm, dimm_max, index;
	size_t hob_size;
	const DIMM_INFO *src_dimm;
	struct dimm_info *dest_dimm;
	struct memory_info *mem_info;
	const CHANNEL_INFO *channel_info;
	const FSP_SMBIOS_MEMORY_INFO *memory_info_hob;

	if (mem_sku >= lp4cfg->num_skus) {
		printk(BIOS_ERR, "Too few LPDDR4 SKUs: 0x%zx/0x%zx\n",
				mem_sku, lp4cfg->num_skus);
		return;
	}

	memory_info_hob = fsp_find_smbios_memory_info(&hob_size);

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (mem_info == NULL) {
		printk(BIOS_ERR, "CBMEM entry for DIMM info missing\n");
		return;
	}
	memset(mem_info, 0, sizeof(*mem_info));

	/* Describe the first N DIMMs in the system */
	index = 0;
	dimm_max = ARRAY_SIZE(mem_info->dimm);

	for (channel = 0; channel < memory_info_hob->ChannelCount; channel++) {
		if (index >= dimm_max)
			break;
		channel_info = &memory_info_hob->ChannelInfo[channel];
		for (dimm = 0; dimm < channel_info->DimmCount; dimm++) {
			if (index >= dimm_max)
				break;
			src_dimm = &channel_info->DimmInfo[dimm];
			dest_dimm = &mem_info->dimm[index];

			if (!src_dimm->SizeInMb)
				continue;

			/* Populate the DIMM information */
			dimm_info_fill(dest_dimm,
				src_dimm->SizeInMb,
				memory_info_hob->MemoryType,
				memory_info_hob->MemoryFrequencyInMHz,
				channel_info->ChannelId,
				src_dimm->DimmId,
				lp4cfg->skus[mem_sku].part_num,
				strlen(lp4cfg->skus[mem_sku].part_num),
				memory_info_hob->DataWidth);
			index++;
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}
