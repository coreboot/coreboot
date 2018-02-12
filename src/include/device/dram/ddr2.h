/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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
 */

/*
 * JEDEC Standard No. 21-C
 * Annex J: Annex J: Serial Presence Detects for DDR2 SDRAM (Revision 1.3)
 */

#ifndef DEVICE_DRAM_DDR2L_H
#define DEVICE_DRAM_DDR2L_H

/**
 * @file ddr2.h
 *
 * \brief Utilities for decoding DDR2 SPDs
 */

#include <stdint.h>
#include <spd.h>
#include <device/dram/common.h>

/*
 * Module type (byte 20, bits 5:0) of SPD
 * This definition is specific to DDR2. DDR3 SPDs have a different structure.
 */
enum spd_dimm_type_ddr2 {
	SPD_DDR2_DIMM_TYPE_UNDEFINED			= 0x00,
	SPD_DDR2_DIMM_TYPE_RDIMM			= 0x01,
	SPD_DDR2_DIMM_TYPE_UDIMM			= 0x02,
	SPD_DDR2_DIMM_TYPE_SO_DIMM			= 0x04,
	SPD_DDR2_DIMM_TYPE_72B_SO_CDIMM		= 0x06,
	SPD_DDR2_DIMM_TYPE_72B_SO_RDIMM		= 0x07,
	SPD_DDR2_DIMM_TYPE_MICRO_DIMM		= 0x08,
	SPD_DDR2_DIMM_TYPE_MINI_RDIMM		= 0x10,
	SPD_DDR2_DIMM_TYPE_MINI_UDIMM		= 0x20,
	/* Masks to bits 5:0 to give the dimm type */
	SPD_DDR2_DIMM_TYPE_MASK			= 0x3f,
};

/**
 * \brief DIMM flags
 *
 * Characteristic flags for the DIMM, as presented by the SPD
 */
union dimm_flags_ddr2_st {
	/* The whole point of the union/struct construct is to allow us to clear
	 * all the bits with one line: flags.raw = 0.
	 * We do not care how these bits are ordered */
	struct {
		/* Module can work at 5.00V */
		unsigned operable_5_00V:1;
		/* Module can work at 3.33V */
		unsigned operable_3_33V:1;
		/* Module can work at 2.50V */
		unsigned operable_2_50V:1;
		/* Module can work at 1.80V - All DIMMS must be 1.8V operable */
		unsigned operable_1_80V:1;
		/* Module can work at 1.50V */
		unsigned operable_1_50V:1;
		/* Module can work at 1.35V */
		unsigned operable_1_35V:1;
		/* Module can work at 1.20V */
		unsigned operable_1_25V:1;
		/* Has an 8-bit bus extension, meaning the DIMM supports ECC */
		unsigned is_ecc:1;
		/* Supports weak driver */
		unsigned weak_driver:1;
		/* Supports terminating at 50 Ohm */
		unsigned terminate_50ohms:1;
		/* Partial Array Self Refresh */
		unsigned pasr:1;
		/* Supports burst length 8 */
		unsigned bl8:1;
		/* Supports burst length 4 */
		unsigned bl4:1;
		/* DIMM Package is stack */
		unsigned stacked:1;
		/* the assembly supports self refresh */
		unsigned self_refresh:1;
	};
	unsigned int raw;
};

/**
 * \brief DIMM characteristics
 *
 * The characteristics of each DIMM, as presented by the SPD
 */
struct dimm_attr_ddr2_st {
	enum spd_memory_type dram_type;
	enum spd_dimm_type_ddr2 dimm_type;
	/* BCD SPD revision */
	u8 rev;
	/* Supported CAS mask, bit 0 == CL0 .. bit7 == CL7 */
	u8 cas_supported;
	/* Maximum cloclk to data cycle times for various CAS.
	 * Fields 0 and 1 are unused. */
	u32 cycle_time[8];
	/* Maximum data access times for various CAS.
	 * Fields 0 and 1 are unused. */
	u32 access_time[8];
	/* Flags extracted from SPD */
	union dimm_flags_ddr2_st flags;
	/* Number of banks */
	u8 banks;
	/* SDRAM width */
	u8 width;
	/* Module width */
	u8 mod_width;
	/* Number of ranks */
	u8 ranks;
	/* Number or row address bits */
	u8 row_bits;
	/* Number or column address bits */
	u8 col_bits;
	/* Number of PLLs on module */
	u8 plls;
	/* Size of module in MiB */
	u16 size_mb;
	/* Size of one rank in MiB */
	u16 ranksize_mb;
	/* Latencies are in units of 1/256 ns */
	u32 tCK;
	u32 tWR;
	u32 tRCD;
	u32 tRRD;
	u32 tRP;
	u32 tRAS;
	u32 tIS;
	u32 tIH;
	u32 tDS;
	u32 tDH;

	u32 tRC;
	u32 tRFC;
	u32 tWTR;
	u32 tRTP;
	u32 tDQSQ;
	u32 tQHS;

	/* Latencies are in units of 1/256 us */
	u32 tPLL;
	u32 tRR;

	u8 checksum;
	/* Manufacturer ID */
	u32 manufacturer_id;
	/* ASCII part number - NULL terminated */
	u8 part_number[17];
	/* Year manufactured */
	u16 year;
	/* Week manufactured */
	u8 weeks;
	/* Unique serial number */
	u32 serial;
};

/** Maximum SPD size supported */
#define SPD_SIZE_MAX_DDR2 128

int spd_dimm_is_registered_ddr2(enum spd_dimm_type_ddr2 type);
u8 spd_ddr2_calc_checksum(u8 *spd, int len);
u32 spd_decode_spd_size_ddr2(u8 byte0);
u32 spd_decode_eeprom_size_ddr2(u8 byte1);
int spd_decode_ddr2(struct dimm_attr_ddr2_st *dimm, u8 spd[SPD_SIZE_MAX_DDR2]);
void dram_print_spd_ddr2(const struct dimm_attr_ddr2_st *dimm);
void normalize_tck(u32 *tclk);
u8 spd_get_msbs(u8 c);

#endif /* DEVICE_DRAM_DDR2L_H */
