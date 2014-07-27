/*
 * Memory information
 *
 * Copyright (C) 2014, Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MEMORY_INFO_H_
#define _MEMORY_INFO_H_

/*
 * If this table is filled and put in CBMEM,
 * then these info in CBMEM will be used to generate smbios type 17 table
 */
struct dimm_info {
	uint32_t dimm_size;
	uint16_t ddr_type;
	uint16_t ddr_frequency;
	uint8_t rank_per_dimm;
	uint8_t channel_num;
	uint8_t dimm_num;
	uint8_t bank_locator;
	/* The 5th byte is '\0' for the end of string */
	uint8_t serial[5];
	/* The 19th byte is '\0' for the end of string */
	uint8_t module_part_number[19];
	uint16_t mod_id;
	uint8_t mod_type;
	uint8_t bus_width;
} __attribute__((packed));

struct memory_info {
	uint8_t dimm_cnt;
	/* Maximum num of dimm is 8 */
	struct dimm_info dimm[8];
} __attribute__((packed));

#endif
