/*
 * This file is part of the coreboot project.
 *
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

#ifndef _COMMON_SMBIOS_H_
#define _COMMON_SMBIOS_H_

#include <stdint.h>
#include <memory_info.h>

/* Offset info DIMM_INFO SpdSave for start of serial number */
#define SPD_SAVE_OFFSET_SERIAL	5

/* Fill the SMBIOS memory information from FSP MEM_INFO_DATA_HOB in CBMEM.*/
void dimm_info_fill(struct dimm_info *dimm, u32 dimm_capacity, u8 ddr_type,
		u32 frequency, u8 rank_per_dimm, u8 channel_id, u8 dimm_id,
		const char *module_part_num, size_t module_part_number_size,
		const u8 *module_serial_num, u16 data_width, u32 vdd_voltage,
		bool ecc_support, u16 mod_id, u8 mod_type);

#endif /* _COMMON_SMBIOS_H_ */
