/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

/**
 * @file ddr3_util.h
 *
 * \brief Utilities for decoding DDR3 SPDs
 * @{
 */

#include <console/console.h>
#include <device/device.h>
#include <device/dram_util/ddr3_util.h>


/**
 * \brief Checks if the DIMM is Registered based on byte[3] of the spd
 *
 * Tells if the DIMM type is registered or not.
 *
 * @param type DIMM type. This is byte[3] of the spd.
 */
int dimm_is_registered(enum dimm_type type)
{
	if( (type == DIMM_TYPE_RDIMM)
		| (type == DIMM_TYPE_MINI_RDIMM)
		| (type == DIMM_TYPE_72B_SO_RDIMM) )
		return 1;

	return 0;
}

/**
 * \brief Decode the raw spd data
 *
 * Decodes a raw SPD data from a DDR3 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr stucture where the decoded data is to
 * 	       be stored
 * @param spd array of raw data previously read from the SPD.
 */
void spd_decode_ddr3(dimm_attr *dimm, spd_raw_data spd)
{
	int nCRC, i;
	u16 crc, spd_crc;
	u8 * ptr = spd;
	u8 ftb_divisor;
	u8 ftb_dividend;
	u8 capacity_shift, bus_width, sdram_width;
	u32 mtb; /* medium time base */
	/* Don't assume we memset 0 dimm struct. Clear all our flags */
	dimm->flags.raw = 0;
	/* Make sure that the spd dump is indeed from a DDR3 module */
	if(spd[2] != DRAM_TYPE_DDR3)
	{
		printram("Not a DDR3 SPD!\n");
		dimm->dram_type = DRAM_TYPE_UNDEFINED;
		return;
	}
	dimm->dram_type = DRAM_TYPE_DDR3;

	/* Find the number of bytes covered by CRC */
	if(spd[0] & 0x80) {
		nCRC = 117;
	} else {
		nCRC =126;
	}

	/* Compute the CRC */
	crc = 0;
	while (--nCRC >= 0) {
		crc = crc ^ (int)*ptr++ << 8;
		for (i = 0; i < 8; ++i)
			if (crc & 0x8000) {
				crc = crc << 1 ^ 0x1021;
			} else {
				crc = crc << 1;
			}
	}
	/* Compare with the CRC in the SPD */
	spd_crc = (spd[127] << 8) + spd[126];
	/* Verify the CRC is correct */
	if(crc != spd_crc)
		printram("ERROR: SPD CRC failed!!!");


	unsigned int reg, val, param;
	printram("  Revision: %x \n", spd[1] );
	printram("  Type    : %x \n", spd[2] );
	printram("  Key     : %x \n", spd[3] );

	reg = spd[4];
	/* Number of memory banks */
	val = (reg >> 4) & 0x07;
	if (val > 0x03) printram("  Invalid number of memory banks\n");
	param = 1 << (val + 3);
	printram("  Banks   : %u \n", param );
	/* SDRAM capacity */
	capacity_shift = reg & 0x0f;
	if (capacity_shift > 0x06) printram("  Invalid module capacity\n");
	if (capacity_shift < 0x02) {
		printram("  Capacity: %u Mb\n", 256 << capacity_shift);
	} else {
		printram("  Capacity: %u Gb\n", 1 << (capacity_shift - 2));
	}

	reg = spd[5];
	/* Row address bits */
	val = (reg >> 4) & 0x07;
	if(val > 0x04) printram("  Invalid row address bits\n");
	dimm->row_bits = val + 12;
	/* Column address bits */
	val = reg & 0x07;
	if(val > 0x03) printram("  Invalid column address bits\n");
	dimm->col_bits = val + 9;

	/* Module nominal voltage */
	reg = spd[6];
	printram("  Supported voltages: ");
	if(reg & (1<<2) ) {
		dimm->flags.operable_1_50V = 1;
		printram("1.2V ");
	}
	if(reg & (1<<1) ) {
		dimm->flags.operable_1_35V = 1;
		printram("1.35V ");
	}
	if( !(reg & (1<<0)) ) {
		dimm->flags.operable_1_50V = 1;
		printram("1.5V ");
	}
	printram("\n");

	/* Module organization */
	reg = spd[7];
	/* Number of ranks*/
	val = (reg >> 3) & 0x07;
	if(val > 3) printram("  Invalid number of ranks\n");
	dimm->ranks = val + 1;
	/* SDRAM device width */
	val = (reg & 0x07);
	if(val > 3) printram("  Invalid SDRAM width\n");
	sdram_width = (4 << val);
	printram("  SDRAM width       : %u \n", sdram_width);

	/* Memory bus width */
	reg = spd[8];
	/* Bus extension */
	val = (reg >> 3) & 0x03;
	if(val > 1) printram("  Invalid bus extension\n");
	dimm->flags.is_ecc = val ? 1 : 0;
	printram("  Bus extension     : %u bits\n", val?8:0);
	/* Bus width */
	val = reg & 0x07;
	if(val > 3) printram("  Invalid bus width\n");
	bus_width = 8 << val;
	printram("  Bus width         : %u \n", bus_width);

	/* We have all the info we need to compute the dimm size */
	/* Capacity is 256Mbit multiplied by the power of 2 specified in
	 * capacity_shift
	 * The rest is the JEDEC formula */
	/* I am certain it will fit in 16 bits
	 * Remember, It's in units of 2^24 bytes*/
	dimm->size = ( (1 << (capacity_shift + (25-24)) ) * bus_width
			* dimm->ranks ) / sdram_width;

	/* Fine Timebase (FTB) Dividend/Divisor */
	/* Dividend */
	ftb_dividend = (spd[9] >> 4) & 0x0f;
	/* Divisor */
	ftb_divisor = spd[9] & 0x0f;

	/* Medium Timebase =
	 *   Medium Timebase (MTB) Dividend /
	 *   Medium Timebase (MTB) Divisor */
	mtb = (((u32)spd[10]) << 8) / spd [11];

	/* SDRAM Minimum Cycle Time (tCKmin) */
	dimm->tCK = spd[12] * mtb;
	/* CAS Latencies Supported */
	dimm->cas_supported = (spd[15] << 8) + spd[14];
	/* Minimum CAS Latency Time (tAAmin) */
	dimm->tAA = spd[16] * mtb;
	/* Minimum Write Recovery Time (tWRmin) */
	dimm->tWR = spd[17] * mtb;
	/* Minimum RAS# to CAS# Delay Time (tRCDmin) */
	dimm->tRCD = spd[18] * mtb;
	/* Minimum Row Active to Row Active Delay Time (tRRDmin) */
	dimm->tRRD = spd[19] * mtb;
	/* Minimum Row Precharge Delay Time (tRPmin)*/
	dimm->tRP = spd[20] * mtb;
	/* Minimum Active to Precharge Delay Time (tRASmin) */
	dimm->tRAS = (((spd[21] & 0x0f) << 8) + spd[22]) * mtb;
	/* Minimum Active to Active/Refresh Delay Time (tRCmin) */
	dimm->tRC = (((spd[21] & 0xf0) << 4) + spd[23]) * mtb;
	/* Minimum Refresh Recovery Delay Time (tRFCmin) */
	dimm->tRFC = ((spd[25] << 8) + spd[24]) * mtb;
	/* Minimum Internal Write to Read Command Delay Time (tWTRmin) */
	dimm->tWTR = spd[26] * mtb;
	/* Minimum Internal Read to Precharge Command Delay Time (tRTPmin) */
	dimm->tRTP = spd[27] * mtb;
	/* Minimum Four Activate Window Delay Time (tFAWmin) */
	dimm->tFAW = (((spd[28] & 0x0f) << 8) + spd[29]) * mtb;

	/* SDRAM Optional Features */
	reg = spd[30];
	printram("  Optional features :");
	if(reg & 0x80) {
		dimm->flags.dll_off_mode = 1;
		printram(" DLL-Off_mode");
	}
	if(reg & 0x02) {
		dimm->flags.rzq7_supported = 1;
		printram(" RZQ/7");
	}
	if(reg & 0x01) {
		dimm->flags.rzq6_supported = 1;
		printram(" RZQ/6");
	}
	printram("\n");

	/* SDRAM Thermal and Refresh Options */
	reg = spd[31];
	printram("  Thermal features  :");
	if(reg & 0x80) {
		dimm->flags.pasr = 1;
		printram(" PASR");
	}
	if(reg & 0x08) {
		dimm->flags.odts = 1;
		printram(" ODTS");
	}
	if(reg & 0x04) {
		dimm->flags.asr = 1;
		printram(" ASR");
	}
	if(reg & 0x02) {
		dimm->flags.ext_temp_range = 1;
		printram(" ext_temp_refresh");
	}
	if(reg & 0x01) {
		dimm->flags.ext_temp_refresh = 1;
		printram(" ext_temp_range");
	}
	printram("\n");

	/*  Module Thermal Sensor */
	reg = spd[32];
	if(reg & 0x80) dimm->flags.therm_sensor = 1;
	printram("  Thermal sensor    : %s\n",
		 dimm->flags.therm_sensor?"yes":"no");

	/*  SDRAM Device Type */
	reg = spd[33];
	printram("  Standard SDRAM    : %s\n", (reg & 0x80)?"yes":"no");

	if(spd[60] & 0x01) {
		dimm->flags.pins_mirrored = 1;
		printram("  DIMM Rank1 Address bits mirrorred!!!\n");
	}

}

static void print_ns(const char * msg, u32 val)
{
	u32 mant, fp;
	mant = val/256;
	fp = (val % 256) * 1000/256;

	printram("%s%3u.%.3u ns\n", msg, mant, fp);
}

/**
* \brief Print the info in dimm
*
* Print info about the dimm. Useful to use when CONFIG_DEBUG_RAM_SETUP is
* selected.
*
* @param dimm pointer to already decoded @ref dimm_attr stucture
*/
void dram_print_spd_ddr3(const dimm_attr *dimm)
{
	u16 val16;
	int i;

	printram("  Row    addr bits  : %u \n", dimm->row_bits);
	printram("  Column addr bits  : %u \n", dimm->col_bits);
	printram("  Number of ranks   : %u \n", dimm->ranks);
	printram("  DIMM Capacity     : %u MB\n", dimm->size << 4);

	/* CAS Latencies Supported */
	val16 = dimm->cas_supported;
	printram("  CAS latencies     :");
	i = 0;
	do{
		if(val16 & 1) printram(" %u", i + 4);
		i++;
		val16 >>= 1;
	} while(val16);
	printram("\n");

	print_ns("  tCKmin            : ", dimm->tCK);
	print_ns("  tAAmin            : ", dimm->tAA);
	print_ns("  tWRmin            : ", dimm->tWR);
	print_ns("  tRCDmin           : ", dimm->tRCD);
	print_ns("  tRRDmin           : ", dimm->tRRD);
	print_ns("  tRPmin            : ", dimm->tRP);
	print_ns("  tRASmin           : ", dimm->tRAS);
	print_ns("  tRCmin            : ", dimm->tRC);
	print_ns("  tRFCmin           : ", dimm->tRFC);
	print_ns("  tWTRmin           : ", dimm->tWTR);
	print_ns("  tRTPmin           : ", dimm->tRTP);
	print_ns("  tFAWmin           : ", dimm->tFAW);

}

/* @} */

