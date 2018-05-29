/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
 * Copyright (C) 2017-2018 Online SAS.
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
#include <string.h>

#include <soc/hob_mem.h>

#include <smbios.h>
#include <memory_info.h>
#include <soc/ramstage.h>

/* Save the DIMM information for SMBIOS table 17 */
void soc_save_dimm_info(void)
{
	int channel;
	const CHANNEL_INFO *channel_info;
	int dimm;
	const DIMM_INFO *dimm_info;
	int dimm_max;
	int index;
	struct memory_info *mem_info;
	const FSP_SMBIOS_MEMORY_INFO *memory_info_hob;

	/* Get the memory info HOB */
	memory_info_hob = soc_get_fsp_smbios_memory_info_hob();

	if (memory_info_hob == NULL)
		return;

	/* Display the data in the FSP_SMBIOS_MEMORY_INFO HOB */
	if (IS_ENABLED(CONFIG_DISPLAY_HOBS))
		soc_display_fsp_smbios_memory_info_hob(memory_info_hob);

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	printk(BIOS_DEBUG, "CBMEM entry for DIMM info: 0x%p\n", mem_info);
	if (mem_info == NULL)
		return;
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
			dimm_info = &channel_info->DimmInfo[dimm];

			/* Populate the DIMM information */
			if (!dimm_info->SizeInMb)
				continue;

			mem_info->dimm[index].dimm_size =
				dimm_info->SizeInMb;
			mem_info->dimm[index].ddr_type =
				memory_info_hob->MemoryType;
			mem_info->dimm[index].ddr_frequency =
				memory_info_hob->MemoryFrequencyInMHz;
			mem_info->dimm[index].channel_num =
				channel_info->ChannelId;
			mem_info->dimm[index].dimm_num =
				dimm_info->DimmId;

			strncpy((char *)
				mem_info->dimm[index].module_part_number,
				(char *)dimm_info->ModulePartNum, 18);
			mem_info->dimm[index].mod_id =
				dimm_info->MfgId;
			switch (memory_info_hob->DataWidth) {
			default:
			case 8:
				mem_info->dimm[index].bus_width =
					MEMORY_BUS_WIDTH_8;
				break;

			case 16:
				mem_info->dimm[index].bus_width =
					MEMORY_BUS_WIDTH_16;
				break;

			case 32:
				mem_info->dimm[index].bus_width =
					MEMORY_BUS_WIDTH_32;
				break;

			case 64:
				mem_info->dimm[index].bus_width =
					MEMORY_BUS_WIDTH_64;
				break;

			case 128:
				mem_info->dimm[index].bus_width =
					MEMORY_BUS_WIDTH_128;
				break;
			}

			/* Add any mainboard specific information */
			mainboard_add_dimm_info(mem_info, channel, dimm, index);
			index++;
		}
	}
	mem_info->dimm_cnt = index;
	printk(BIOS_DEBUG, "%d DIMMs found\n", mem_info->dimm_cnt);
}

/* Add any mainboard specific information */
__attribute__((weak)) void mainboard_add_dimm_info(struct memory_info *mem_info,
						   int channel, int dimm,
						   int index)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
