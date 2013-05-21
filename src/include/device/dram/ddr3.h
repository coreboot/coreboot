/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#ifndef DEVICE_DRAM_DDR3L_H
#define DEVICE_DRAM_DDR3L_H

/**
 * @file ddr3.h
 *
 * \brief Utilities for decoding DDR3 SPDs
 */

#include <stdint.h>
#include <spd.h>

/**
 * \brief Convenience definitions for TCK values
 *
 * Different values for tCK, representing standard DDR3 frequencies.
 * These values are in 1/256 ns units.
 * @{
 */
#define TCK_1066MHZ     240
#define TCK_800MHZ      320
#define TCK_666MHZ      384
#define TCK_533MHZ      480
#define TCK_400MHZ      640
#define TCK_333MHZ      768
#define TCK_266MHZ      960
#define TCK_200MHZ      1280
/** @} */

/**
 * \brief Convenience macro for enabling printk with CONFIG_DEBUG_RAM_SETUP
 *
 * Use this macro instead of printk(); for verbose RAM initialization messages.
 * When CONFIG_DEBUG_RAM_SETUP is not selected, these messages are automatically
 * disabled.
 * @{
 */
#if defined(CONFIG_DEBUG_RAM_SETUP) && (CONFIG_DEBUG_RAM_SETUP)
#define printram(x, ...) printk(BIOS_DEBUG, x, ##__VA_ARGS__)
#else
#define printram(x, ...)
#endif
/** @} */

/**
 * \brief DIMM flags
 *
 * Characteristic flags for the DIMM, as presented by the SPD
 */
typedef union dimm_flags_st {
	/* The whole point of the union/struct construct is to allow us to clear
	 * all the bits with one line: flags.raw = 0.
	 * We do not care how these bits are ordered */
	struct {
		/* Indicates if rank 1 of DIMM uses a mirrored pin mapping. See:
		 * Annex K: Serial Presence Detect (SPD) for DDR3 SDRAM */
		unsigned pins_mirrored:1;
		/* Module can work at 1.50V - All DIMMS must be 1.5V operable */
		unsigned operable_1_50V:1;
		/* Module can work at 1.35V */
		unsigned operable_1_35V:1;
		/* Module can work at 1.20V */
		unsigned operable_1_20V:1;
		/* Has an 8-bit bus extension, meaning the DIMM supports ECC */
		unsigned is_ecc:1;
		/* DLL-Off Mode Support */
		unsigned dll_off_mode:1;
		/* Indicates a drive strength of RZQ/6 (40 Ohm) is supported */
		unsigned rzq6_supported:1;
		/* Indicates a drive strength of RZQ/7 (35 Ohm) is supported */
		unsigned rzq7_supported:1;
		/* Partial Array Self Refresh */
		unsigned pasr:1;
		/* On-die Thermal Sensor Readout */
		unsigned odts:1;
		/* Auto Self Refresh */
		unsigned asr:1;
		/* Extended temperature range supported */
		unsigned ext_temp_range:1;
		/* Operating at extended temperature requires 2X refresh rate */
		unsigned ext_temp_refresh:1;
		/* Thermal sensor incorporated */
		unsigned therm_sensor:1;
	};
	unsigned raw;
} dimm_flags_t;

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
typedef struct dimm_attr_st {
	enum spd_memory_type dram_type;
	u16 cas_supported;
	/* Flags extracted from SPD */
	dimm_flags_t flags;
	/* Number of ranks */
	u8 ranks;
	/* Number or row address bits */
	u8 row_bits;
	/* Number or column address bits */
	u8 col_bits;
	/* Size of module in (1 << 24) bytes (16MB) */
	u16 size;
	/* Latencies are in units of 1/256 ns */
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

typedef u8 spd_raw_data[256];

void spd_decode_ddr3(dimm_attr * dimm, spd_raw_data spd_data);
int dimm_is_registered(enum spd_dimm_type type);
void dram_print_spd_ddr3(const dimm_attr * dimm);

/**
 * \brief Read double word from specified address
 *
 * Should be useful when doing an MRS to the DIMM
 */
static inline u32 volatile_read(volatile u32 addr)
{
	volatile u32 result;
	result = *(volatile u32 *)addr;
	return result;
}

#endif				/* DEVICE_DRAM_DDR3_H */
