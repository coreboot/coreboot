/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RAMINIT_VX900_H
#define RAMINIT_VX900_H

#include <devices/dram/dram.h>

/* The maximum number of DIMM slots that the VX900 supports */
#define VX900_MAX_DIMM_SLOTS 2

#define VX900_MAX_MEM_RANKS 4


#define SPD_END_LIST 0xff

typedef struct dimm_layout_st
{
	/* The address of the DIMM on the SMBUS *
	 * 0xFF to terminate the array*/
	u8 spd_addr[VX900_MAX_DIMM_SLOTS + 1];
} dimm_layout;

typedef struct dimm_info_st
{
	dimm_attr dimm[VX900_MAX_DIMM_SLOTS];
} dimm_info;

typedef struct mem_rank_st {
	u16 start_addr;
	u16 end_addr;
}mem_rank;

typedef struct rank_layout_st {
	u32 phys_rank_size[VX900_MAX_MEM_RANKS];
	mem_rank virt[VX900_MAX_MEM_RANKS];
} rank_layout;
void vx900_init_dram_ddr3(const dimm_layout *dimms);

#endif /* RAMINIT_VX900_H */
