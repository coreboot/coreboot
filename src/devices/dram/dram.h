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

#ifndef DRAM_H
#define DRAM_H

#include <stdint.h>

/* DRAM type, byte 2 of spd */
#define DRAM_TYPE_UNDEFINED             0x00
#define DRAM_TYPE_FPM_DRAM              0x01
#define DRAM_TYPE_EDO                   0x02
#define DRAM_TYPE_PIPELINED_NIBBLE      0x03
#define DRAM_TYPE_SDRAM                 0x04
#define DRAM_TYPE_ROM                   0x05
#define DRAM_TYPE_DDR_SGRAM             0x06
#define DRAM_TYPE_DDR                   0x07
#define DRAM_TYPE_DDR2                  0x08
#define DRAM_TYPE_DDR2_FBDIMM           0x09
#define DRAM_TYPE_DDR2_FB_PROBE         0x0a
#define DRAM_TYPE_DDR3                  0x0b

/* Module type (byte 3, bits 3:0) of SPD */
#define DIMM_TYPE_UNDEFINED     0
#define DIMM_TYPE_RDIMM         1
#define DIMM_TYPE_UDIMM         2
#define DIMM_TYPE_SO_DIMM       3
#define DIMM_TYPE_MICRO_DIMM    4
#define DIMM_TYPE_MINI_RDIMM    5
#define DIMM_TYPE_MINI_UDIMM    6
#define DIMM_TYPE_MINI_CDIMM    7
#define DIMM_TYPE_72B_SO_UDIMM  8
#define DIMM_TYPE_72B_SO_RDIMM  9
#define DIMM_TYPE_72B_SO_CDIMM  10

#if defined(CONFIG_DEBUG_RAM_SETUP) && (CONFIG_DEBUG_RAM_SETUP)
#define printram(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printram(x, ...)
#endif

/* Different values for tCK, representing standard DDR3 frequencies
 * As always, these values are in 1/256 ns units */
#define TCK_1066MHZ     240
#define TCK_800MHZ      320
#define TCK_666MHZ      384
#define TCK_533MHZ      480
#define TCK_400MHZ      640
#define TCK_333MHZ      768
#define TCK_266MHZ      960
#define TCK_200MHZ      1280

#define RAM_UNIT (1<<24)

#include <stdint.h>

/**
 *\brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
typedef struct dimm_attr_st {
	u8 dram_type;
	u16 cas_supported;
	/* Number of ranks */
	u8 ranks;
	/* Number or row address bits */
	u8 row_bits;
	/* Number or column address bits */
	u8 col_bits;
	/* Size of module in (1 << 24) bytes (16MB) */
	u16 size;
	/* Latencies are in units of ns, scaled by x256 */
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

} dimm_attr;

typedef struct ramctr_timing_st {
	u8 dram_type;
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
	
} ramctr_timing;


typedef u8 spd_raw_data[256];

/**
 * \brief Decode the raw spd data
 */
void spd_decode_ddr3(dimm_attr *dimm, spd_raw_data spd_data);

/**
 * \brief Checks if the DIMM is Registered based on byte[3] of the spd
 */
int dimm_is_registered(u8 spd_byte3);

/**
 * \brief Print the info in dimm
 */
void dram_print_spd_ddr3(const dimm_attr *dimm);

/**
 * \brief Read double word from specified address
 *
 * Should be useful when doing an MRS to the DIMM
 */
u32 volatile_read(u32 addr);

#endif /* DRAM_H */
