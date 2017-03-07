/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Inc.
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

#include <smbios.h>
#include "smbios.h"
#include <string.h>
#include <console/console.h>

/* Fill the SMBIOS memory information from FSP MEM_INFO_DATA_HOB in CBMEM.*/
void dimm_info_fill(struct dimm_info *dimm, u32 dimm_capacity, u8 ddr_type,
		u32 frequency, u8 channel_id, u8 dimm_id,
		const char *module_part_num, size_t module_part_number_size,
		u16 data_width)
{
	dimm->dimm_size = dimm_capacity;
	dimm->ddr_type = ddr_type;
	dimm->ddr_frequency = frequency;
	dimm->channel_num = channel_id;
	dimm->dimm_num = dimm_id;
	strncpy((char *)dimm->module_part_number,
			module_part_num,
			min(sizeof(dimm->module_part_number),
				module_part_number_size));
	switch (data_width) {
	case 8:
		dimm->bus_width = MEMORY_BUS_WIDTH_8;
		break;
	case 16:
		dimm->bus_width = MEMORY_BUS_WIDTH_16;
		break;
	case 32:
		dimm->bus_width = MEMORY_BUS_WIDTH_32;
		break;
	case 64:
		dimm->bus_width = MEMORY_BUS_WIDTH_64;
		break;
	case 128:
		dimm->bus_width = MEMORY_BUS_WIDTH_128;
		break;
	default:
		printk(BIOS_ERR, "Incorrect DIMM Data width");
	}
}
