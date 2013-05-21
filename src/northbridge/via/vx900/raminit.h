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

#include <device/dram/ddr3.h>
#include "vx900.h"

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
} mem_rank;

typedef struct rank_layout_st {
	u32 phys_rank_size_mb[VX900_MAX_MEM_RANKS];
	mem_rank virt[VX900_MAX_MEM_RANKS];
	dimm_flags_t flags[VX900_MAX_MEM_RANKS];
} rank_layout;

typedef struct pci_reg8_st {
	u8 addr;
	u8 val;
} pci_reg8;

typedef u8 timing_dly[8];

typedef struct delay_range_st {
	timing_dly low;
	timing_dly avg;
	timing_dly high;
} delay_range;

typedef struct vx900_delay_calib_st {
	delay_range rx_dq_cr;
	delay_range rx_dqs;
	/* Transmit delays are calibrated for each dimm */
	delay_range tx_dq[VX900_MAX_DIMM_SLOTS];
	delay_range tx_dqs[VX900_MAX_DIMM_SLOTS];
} vx900_delay_calib;

typedef struct ramctr_timing_st {
	enum spd_memory_type dram_type;
	u16 cas_supported;
	/* tLatencies are in units of ns, scaled by x256 */
	u32 tCK;
	u32 tAA;
	u32 tWR;
	u32 tRCD;
	u32 tRRD;
	u32 tRP;
	u32 tRAS;
	u32 tRC;
	u32 tRFC;
	u32 tWTR;
	u32 tRTP;
	u32 tFAW;
	/* Latencies in terms of clock cycles
	 * They are saved separately as they are needed for DRAM MRS commands*/
	u8 CAS; /* CAS read latency */
	u8 CWL; /* CAS write latency */
	u8 WR;  /* write recovery time */
	/* Number of dimms currently connected */
	u8 n_dimms;

} ramctr_timing;

void vx900_init_dram_ddr3(const dimm_layout *dimms);

#endif /* RAMINIT_VX900_H */
