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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/dram/ddr3.h>

/*==============================================================================
 * = DDR3 SPD decoding helpers
 *----------------------------------------------------------------------------*/

/**
 * \brief Checks if the DIMM is Registered based on byte[3] of the SPD
 *
 * Tells if the DIMM type is registered or not.
 *
 * @param type DIMM type. This is byte[3] of the SPD.
 */
int dimm_is_registered(enum spd_dimm_type type)
{
	if ((type == SPD_DIMM_TYPE_RDIMM)
	    | (type == SPD_DIMM_TYPE_MINI_RDIMM)
	    | (type == SPD_DIMM_TYPE_72B_SO_RDIMM))
		return 1;

	return 0;
}

/**
 * \brief Decode the raw SPD data
 *
 * Decodes a raw SPD data from a DDR3 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr structure where the decoded data is to
 * 	       be stored
 * @param spd array of raw data previously read from the SPD.
 *
 * @return @ref spd_status enumerator
 *		SPD_STATUS_OK -- decoding was successful
 *		SPD_STATUS_INVALID -- invalid SPD or not a DDR3 SPD
 *		SPD_STATUS_CRC_ERROR -- CRC did not verify
 *		SPD_STATUS_INVALID_FIELD -- A field with an invalid value was
 * 					    detected.
 */
int spd_decode_ddr3(dimm_attr * dimm, spd_raw_data spd)
{
	int nCRC, i, ret;
	u16 crc, spd_crc;
	u8 *ptr = spd;
	u8 ftb_divisor, ftb_dividend, capacity_shift, bus_width, sdram_width;
	u8 reg8;
	u32 mtb;		/* medium time base */
	unsigned int val, param;

	ret = SPD_STATUS_OK;

	/* Don't assume we memset 0 dimm struct. Clear all our flags */
	dimm->flags.raw = 0;
	/* Make sure that the SPD dump is indeed from a DDR3 module */
	if (spd[2] != SPD_MEMORY_TYPE_SDRAM_DDR3) {
		printram("Not a DDR3 SPD!\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}
	dimm->dram_type = SPD_MEMORY_TYPE_SDRAM_DDR3;

	/* Find the number of bytes covered by CRC */
	if (spd[0] & 0x80) {
		nCRC = 117;
	} else {
		nCRC = 126;
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
	if (crc != spd_crc) {
		printram("ERROR: SPD CRC failed!!!");
		ret = SPD_STATUS_CRC_ERROR;
	};

	printram("  Revision: %x\n", spd[1]);
	printram("  Type    : %x\n", spd[2]);
	printram("  Key     : %x\n", spd[3]);

	reg8 = spd[4];
	/* Number of memory banks */
	val = (reg8 >> 4) & 0x07;
	if (val > 0x03) {
		printram("  Invalid number of memory banks\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	param = 1 << (val + 3);
	printram("  Banks   : %u\n", param);
	/* SDRAM capacity */
	capacity_shift = reg8 & 0x0f;
	if (capacity_shift > 0x06) {
		printram("  Invalid module capacity\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	if (capacity_shift < 0x02) {
		printram("  Capacity: %u Mb\n", 256 << capacity_shift);
	} else {
		printram("  Capacity: %u Gb\n", 1 << (capacity_shift - 2));
	}

	reg8 = spd[5];
	/* Row address bits */
	val = (reg8 >> 3) & 0x07;
	if (val > 0x04) {
		printram("  Invalid row address bits\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	dimm->row_bits = val + 12;
	/* Column address bits */
	val = reg8 & 0x07;
	if (val > 0x03) {
		printram("  Invalid column address bits\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	dimm->col_bits = val + 9;

	/* Module nominal voltage */
	reg8 = spd[6];
	printram("  Supported voltages:");
	if (reg8 & (1 << 2)) {
		dimm->flags.operable_1_25V = 1;
		printram(" 1.25V");
	}
	if (reg8 & (1 << 1)) {
		dimm->flags.operable_1_35V = 1;
		printram(" 1.35V");
	}
	if (!(reg8 & (1 << 0))) {
		dimm->flags.operable_1_50V = 1;
		printram(" 1.5V");
	}
	printram("\n");

	/* Module organization */
	reg8 = spd[7];
	/* Number of ranks */
	val = (reg8 >> 3) & 0x07;
	if (val > 3) {
		printram("  Invalid number of ranks\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	dimm->ranks = val + 1;
	/* SDRAM device width */
	val = (reg8 & 0x07);
	if (val > 3) {
		printram("  Invalid SDRAM width\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	sdram_width = (4 << val);
	printram("  SDRAM width       : %u\n", sdram_width);

	/* Memory bus width */
	reg8 = spd[8];
	/* Bus extension */
	val = (reg8 >> 3) & 0x03;
	if (val > 1) {
		printram("  Invalid bus extension\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	dimm->flags.is_ecc = val ? 1 : 0;
	printram("  Bus extension     : %u bits\n", val ? 8 : 0);
	/* Bus width */
	val = reg8 & 0x07;
	if (val > 3) {
		printram("  Invalid bus width\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	bus_width = 8 << val;
	printram("  Bus width         : %u\n", bus_width);

	/* We have all the info we need to compute the dimm size */
	/* Capacity is 256Mbit multiplied by the power of 2 specified in
	 * capacity_shift
	 * The rest is the JEDEC formula */
	dimm->size_mb = ((1 << (capacity_shift + (25 - 20))) * bus_width
			 * dimm->ranks) / sdram_width;

	/* Fine Timebase (FTB) Dividend/Divisor */
	/* Dividend */
	ftb_dividend = (spd[9] >> 4) & 0x0f;
	/* Divisor */
	ftb_divisor = spd[9] & 0x0f;

	/* Medium Timebase =
	 *   Medium Timebase (MTB) Dividend /
	 *   Medium Timebase (MTB) Divisor */
	mtb = (((u32) spd[10]) << 8) / spd[11];

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
	/* Minimum Row Precharge Delay Time (tRPmin) */
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
	reg8 = spd[30];
	printram("  Optional features :");
	if (reg8 & 0x80) {
		dimm->flags.dll_off_mode = 1;
		printram(" DLL-Off_mode");
	}
	if (reg8 & 0x02) {
		dimm->flags.rzq7_supported = 1;
		printram(" RZQ/7");
	}
	if (reg8 & 0x01) {
		dimm->flags.rzq6_supported = 1;
		printram(" RZQ/6");
	}
	printram("\n");

	/* SDRAM Thermal and Refresh Options */
	reg8 = spd[31];
	printram("  Thermal features  :");
	if (reg8 & 0x80) {
		dimm->flags.pasr = 1;
		printram(" PASR");
	}
	if (reg8 & 0x08) {
		dimm->flags.odts = 1;
		printram(" ODTS");
	}
	if (reg8 & 0x04) {
		dimm->flags.asr = 1;
		printram(" ASR");
	}
	if (reg8 & 0x02) {
		dimm->flags.ext_temp_range = 1;
		printram(" ext_temp_refresh");
	}
	if (reg8 & 0x01) {
		dimm->flags.ext_temp_refresh = 1;
		printram(" ext_temp_range");
	}
	printram("\n");

	/*  Module Thermal Sensor */
	reg8 = spd[32];
	if (reg8 & 0x80)
		dimm->flags.therm_sensor = 1;
	printram("  Thermal sensor    : %s\n",
		 dimm->flags.therm_sensor ? "yes" : "no");

	/*  SDRAM Device Type */
	reg8 = spd[33];
	printram("  Standard SDRAM    : %s\n", (reg8 & 0x80) ? "no" : "yes");

	if (spd[63] & 0x01) {
		dimm->flags.pins_mirrored = 1;
		printram("  DIMM Rank1 Address bits mirrored!!!\n");
	}

	return ret;
}

/*
 * The information printed below has a more informational character, and is not
 * necessarily tied in to RAM init debugging. Hence, we stop using printram(),
 * and use the standard printk()'s below.
 */

static void print_ns(const char *msg, u32 val)
{
	u32 mant, fp;
	mant = val / 256;
	fp = (val % 256) * 1000 / 256;

	printk(BIOS_INFO, "%s%3u.%.3u ns\n", msg, mant, fp);
}

/**
* \brief Print the info in DIMM
*
* Print info about the DIMM. Useful to use when CONFIG_DEBUG_RAM_SETUP is
* selected, or for a purely informative output.
*
* @param dimm pointer to already decoded @ref dimm_attr structure
*/
void dram_print_spd_ddr3(const dimm_attr * dimm)
{
	u16 val16;
	int i;

	printk(BIOS_INFO, "  Row    addr bits  : %u\n", dimm->row_bits);
	printk(BIOS_INFO, "  Column addr bits  : %u\n", dimm->col_bits);
	printk(BIOS_INFO, "  Number of ranks   : %u\n", dimm->ranks);
	printk(BIOS_INFO, "  DIMM Capacity     : %u MB\n", dimm->size_mb);

	/* CAS Latencies Supported */
	val16 = dimm->cas_supported;
	printk(BIOS_INFO, "  CAS latencies     :");
	i = 0;
	do {
		if (val16 & 1)
			printk(BIOS_INFO, " %u", i + 4);
		i++;
		val16 >>= 1;
	} while (val16);
	printk(BIOS_INFO, "\n");

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

/*==============================================================================
 *= DDR3 MRS helpers
 *----------------------------------------------------------------------------*/

/*
 * MRS command structure:
 * cmd[15:0] = Address pins MA[15:0]
 * cmd[18:16] = Bank address BA[2:0]
 */

/* Map tWR value to a bitmask of the MR0 cycle */
static u16 ddr3_twr_to_mr0_map(u8 twr)
{
	if ((twr >= 5) && (twr <= 8))
		return (twr - 4) << 9;

	/*
	 * From 8T onwards, we can only use even values. Round up if we are
	 * given an odd value.
	 */
	if ((twr >= 9) && (twr <= 14))
		return ((twr + 1) >> 1) << 9;

	/* tWR == 16T is [000] */
	return 0;
}

/* Map the CAS latency to a bitmask for the MR0 cycle */
static u16 ddr3_cas_to_mr0_map(u8 cas)
{
	u16 mask = 0;
	/* A[6:4] are bits [2:0] of (CAS - 4) */
	mask = ((cas - 4) & 0x07) << 4;

	/* A2 is the MSB of (CAS - 4) */
	if ((cas - 4) & (1 << 3))
		mask |= (1 << 2);

	return mask;
}

/**
 * \brief Get command address for a DDR3 MR0 command
 *
 * The DDR3 specification only covers odd write_recovery up to 7T. If an odd
 * write_recovery greater than 7 is specified, it will be rounded up. If a tWR
 * greater than 8 is specified, it is recommended to explicitly round it up or
 * down before calling this function.
 *
 * write_recovery and cas are given in clock cycles. For example, a CAS of 7T
 * should be given as 7.
 *
 * @param write_recovery Write recovery latency, tWR in clock cycles.
 * @param cas CAS latency in clock cycles.
 */
mrs_cmd_t ddr3_get_mr0(enum ddr3_mr0_precharge precharge_pd,
		       u8 write_recovery,
		       enum ddr3_mr0_dll_reset dll_reset,
		       enum ddr3_mr0_mode mode,
		       u8 cas,
		       enum ddr3_mr0_burst_type burst_type,
		       enum ddr3_mr0_burst_length burst_length)
{
	mrs_cmd_t cmd = 0 << 16;

	if (precharge_pd == DDR3_MR0_PRECHARGE_FAST)
		cmd |= (1 << 12);

	cmd |= ddr3_twr_to_mr0_map(write_recovery);

	if (dll_reset == DDR3_MR0_DLL_RESET_YES)
		cmd |= (1 << 8);

	if (mode == DDR3_MR0_MODE_TEST)
		cmd |= (1 << 7);

	cmd |= ddr3_cas_to_mr0_map(cas);

	if (burst_type == DDR3_MR0_BURST_TYPE_INTERLEAVED)
		cmd |= (1 << 3);

	cmd |= (burst_length & 0x03) << 0;

	return cmd;
}

static u16 ddr3_rtt_nom_to_mr1_map(enum ddr3_mr1_rtt_nom rtt_nom)
{
	u16 mask = 0;
	/* A9 <-> rtt_nom[2] */
	if (rtt_nom & (1 << 2))
		mask |= (1 << 9);
	/* A6 <-> rtt_nom[1] */
	if (rtt_nom & (1 << 1))
		mask |= (1 << 6);
	/* A2 <-> rtt_nom[0] */
	if (rtt_nom & (1 << 0))
		mask |= (1 << 2);

	return mask;
}

static u16 ddr3_ods_to_mr1_map(enum ddr3_mr1_ods ods)
{
	u16 mask = 0;
	/* A5 <-> ods[1] */
	if (ods & (1 << 1))
		mask |= (1 << 5);
	/* A1 <-> ods[0] */
	if (ods & (1 << 0))
		mask |= (1 << 1);

	return mask;
}

/**
 * \brief Get command address for a DDR3 MR1 command
 */
mrs_cmd_t ddr3_get_mr1(enum ddr3_mr1_qoff qoff,
		       enum ddr3_mr1_tqds tqds,
		       enum ddr3_mr1_rtt_nom rtt_nom,
		       enum ddr3_mr1_write_leveling write_leveling,
		       enum ddr3_mr1_ods ods,
		       enum ddr3_mr1_additive_latency additive_latency,
		       enum ddr3_mr1_dll dll_disable)
{
	mrs_cmd_t cmd = 1 << 16;

	if (qoff == DDR3_MR1_QOFF_DISABLE)
		cmd |= (1 << 12);

	if (tqds == DDR3_MR1_TQDS_ENABLE)
		cmd |= (1 << 11);

	cmd |= ddr3_rtt_nom_to_mr1_map(rtt_nom);

	if (write_leveling == DDR3_MR1_WRLVL_ENABLE)
		cmd |= (1 << 7);

	cmd |= ddr3_ods_to_mr1_map(ods);

	cmd |= (additive_latency & 0x03) << 3;

	if (dll_disable == DDR3_MR1_DLL_DISABLE)
		cmd |= (1 << 0);

	return cmd;
}

/**
 * \brief Get command address for a DDR3 MR2 command
 *
 * cas_cwl is given in clock cycles. For example, a cas_cwl of 7T should be
 * given as 7.
 *
 * @param cas_cwl CAS write latency in clock cycles.
 */
mrs_cmd_t ddr3_get_mr2(enum ddr3_mr2_rttwr rtt_wr,
		       enum ddr3_mr2_srt_range extended_temp,
		       enum ddr3_mr2_asr self_refresh, u8 cas_cwl)
{
	mrs_cmd_t cmd = 2 << 16;

	cmd |= (rtt_wr & 0x03) << 9;

	if (extended_temp == DDR3_MR2_SRT_EXTENDED)
		cmd |= (1 << 7);

	if (self_refresh == DDR3_MR2_ASR_AUTO)
		cmd |= (1 << 6);

	cmd |= ((cas_cwl - 5) & 0x07) << 3;

	return cmd;
}

/**
 * \brief Get command address for a DDR3 MR3 command
 *
 * @param dataflow_from_mpr Specify a non-zero value to put DRAM in read
 *			    leveling mode. Zero for normal operation.
 */
mrs_cmd_t ddr3_get_mr3(char dataflow_from_mpr)
{
	mrs_cmd_t cmd = 3 << 16;

	if (dataflow_from_mpr)
		cmd |= (1 << 2);

	return cmd;
}

/**
 * \brief Mirror the address bits for this MRS command
 *
 * Swap the following bits in the MRS command:
 *	- MA3 <-> MA4
 *	- MA5 <-> MA6
 *	- MA7 <-> MA8
 *	- BA0 <-> BA1
 */
mrs_cmd_t ddr3_mrs_mirror_pins(mrs_cmd_t cmd)
{
	u32 downshift, upshift;
	/* High bits=    A4    |    A6    |    A8    |    BA1 */
	/* Low bits =    A3    |    A5    |    A7    |    BA0 */
	u32 lowbits = (1 << 3) | (1 << 5) | (1 << 7) | (1 << 16);
	downshift = (cmd & (lowbits << 1));
	upshift = (cmd & lowbits);
	cmd &= ~(lowbits | (lowbits << 1));
	cmd |= (downshift >> 1) | (upshift << 1);
	return cmd;
}
