/* SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * @file ddr2.c
 *
 * \brief Utilities for decoding DDR2 SPDs
 */

#include <console/console.h>
#include <device/device.h>
#include <device/dram/ddr2.h>
#include <lib.h>
#include <string.h>
#include <types.h>

/*==============================================================================
 * = DDR2 SPD decoding helpers
 *----------------------------------------------------------------------------*/

/**
 * \brief Checks if the DIMM is Registered based on byte[20] of the SPD
 *
 * Tells if the DIMM type is registered or not.
 *
 * @param type DIMM type. This is byte[20] of the SPD.
 */
int spd_dimm_is_registered_ddr2(enum spd_dimm_type_ddr2 type)
{
	if ((type == SPD_DDR2_DIMM_TYPE_RDIMM) || (type == SPD_DDR2_DIMM_TYPE_72B_SO_RDIMM) ||
			(type == SPD_DDR2_DIMM_TYPE_MINI_RDIMM))
		return 1;

	return 0;
}

/**
 * \brief Calculate the checksum of a DDR2 SPD unique identifier
 *
 * @param spd pointer to raw SPD data
 * @param len length of data in SPD
 *
 * @return the checksum of SPD data bytes 63, or 0 when spd data is truncated.
 */
u8 spd_ddr2_calc_checksum(u8 *spd, int len)
{
	int i;
	u8 c = 0;

	if (len < 63)
		/* Not enough bytes available to get the checksum */
		return 0;

	for (i = 0; i < 63; i++)
		c += spd[i];

	return c;
}

/**
 * \brief Calculate the CRC of a DDR2 SPD unique identifier
 *
 * @param spd pointer to raw SPD data
 * @param len length of data in SPD
 *
 * @return the CRC of SPD data bytes 64..72 and 93..98, or 0
 *  when spd data is truncated.
 */
u16 spd_ddr2_calc_unique_crc(const u8 *spd, int len)
{
	u8 id_bytes[15];
	int i, j = 0;
	if (len < 98)
		/* Not enough bytes available to get the CRC */
		return 0;
	for (i = 64; i <= 72; i++)
		id_bytes[j++] = spd[i];
	for (i = 93; i <= 98; i++)
		id_bytes[j++] = spd[i];

	return ddr_crc16(id_bytes, 15);
}

/**
 * \brief Return size of SPD.
 *
 * Returns size of SPD. Usually 128 Byte.
 */
u32 spd_decode_spd_size_ddr2(u8 byte0)
{
	return MIN(byte0, SPD_SIZE_MAX_DDR2);
}

/**
 * \brief Return size of eeprom.
 *
 * Returns size of eeprom. Usually 256 Byte.
 */
u32 spd_decode_eeprom_size_ddr2(u8 byte1)
{
	if (!byte1)
		return 0;

	if (byte1 > 0x0e)
		return 0x3fff;

	return 1 << byte1;
}

/**
 * \brief Return index of MSB set
 *
 * Returns the index of MSB set.
 */
u8 spd_get_msbs(u8 c)
{
	return log2(c);
}

/**
 * \brief Decode SPD tck cycle time
 *
 * Decodes a raw SPD data from a DDR2 DIMM.
 * Returns cycle time in 1/256th ns.
 */
static enum cb_err spd_decode_tck_time(u32 *tck, u8 c)
{
	u8 high, low;

	high = c >> 4;

	switch (c & 0xf) {
	case 0xa:
		low = 25;
		break;
	case 0xb:
		low = 33;
		break;
	case 0xc:
		low = 66;
		break;
	case 0xd:
		low = 75;
		break;
	case 0xe:
	case 0xf:
		printk(BIOS_WARNING, "Invalid tck setting. lower nibble is 0x%x\n", c & 0xf);
		return CB_ERR;
	default:
		low = (c & 0xf) * 10;
	}

	*tck = ((high * 100 + low) << 8) / 100;
	return CB_SUCCESS;
}

/**
 * \brief Decode SPD bcd style timings
 *
 * Decodes a raw SPD data from a DDR2 DIMM.
 * Returns cycle time in 1/256th ns.
 */
static enum cb_err spd_decode_bcd_time(u32 *bcd, u8 c)
{
	u8 high, low;

	high = c >> 4;
	low = c & 0xf;
	if (high >= 10 || low >= 10)
		return CB_ERR;

	*bcd = ((high * 10 + low) << 8) / 100;
	return CB_SUCCESS;
}

/**
 * \brief Decode SPD tRP, tRRP cycle time
 *
 * Decodes a raw SPD data from a DDR2 DIMM.
 * Returns cycle time in 1/256th ns.
 */
static u32 spd_decode_quarter_time(u8 c)
{
	u8 high, low;

	high = c >> 2;
	low = 25 * (c & 0x3);

	return ((high * 100 + low) << 8) / 100;
}

/**
 * \brief Decode SPD tRR time
 *
 * Decodes a raw SPD data from a DDR2 DIMM.
 * Returns cycle time in 1/256th us.
 */
static enum cb_err spd_decode_tRR_time(u32 *tRR, u8 c)
{
	switch (c & ~0x80) {
	default:
		printk(BIOS_WARNING, "Invalid tRR value 0x%x\n", c);
		return CB_ERR;
	case 0x0:
		*tRR = 15625 << 8;
		break;
	case 0x1:
		*tRR = 15625 << 6;
		break;
	case 0x2:
		*tRR = 15625 << 7;
		break;
	case 0x3:
		*tRR = 15625 << 9;
		break;
	case 0x4:
		*tRR = 15625 << 10;
		break;
	case 0x5:
		*tRR = 15625 << 11;
		break;
	}
	return CB_SUCCESS;
}

/**
 * \brief Decode SPD tRC,tRFC time
 *
 * Decodes a raw SPD data from a DDR2 DIMM.
 * Returns cycle time in 1/256th us.
 */
static void spd_decode_tRCtRFC_time(u8 *spd_40_41_42, u32 *tRC, u32 *tRFC)
{
	u8 b40, b41, b42;

	b40 = spd_40_41_42[0];
	b41 = spd_40_41_42[1];
	b42 = spd_40_41_42[2];

	*tRC = b41 * 100;
	*tRFC = b42 * 100;

	if (b40 & 0x01)
		*tRFC += 256 * 100;

	switch ((b40 >> 1) & 0x07) {
	case 1:
		*tRFC += 25;
		break;
	case 2:
		*tRFC += 33;
		break;
	case 3:
		*tRFC += 50;
		break;
	case 4:
		*tRFC += 66;
		break;
	case 5:
		*tRFC += 75;
		break;
	default:
		break;
	}

	switch ((b40 >> 4) & 0x07) {
	case 1:
		*tRC += 25;
		break;
	case 2:
		*tRC += 33;
		break;
	case 3:
		*tRC += 50;
		break;
	case 4:
		*tRC += 66;
		break;
	case 5:
		*tRC += 75;
		break;
	default:
		break;
	}

	/* Convert to 1/256th us */
	*tRC = (*tRC << 8) / 100;
	*tRFC = (*tRFC << 8) / 100;
}

/**
 * \brief Decode the raw SPD data
 *
 * Decodes a raw SPD data from a DDR2 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr structure where the decoded data is to
 *        be stored
 * @param spd array of raw data previously read from the SPD.
 *
 * @return @ref spd_status enumerator
 *         SPD_STATUS_OK -- decoding was successful
 *         SPD_STATUS_INVALID -- invalid SPD or not a DDR2 SPD
 *         SPD_STATUS_CRC_ERROR -- CRC did not verify
 *         SPD_STATUS_INVALID_FIELD -- A field with an invalid value was
 *             detected.
 */
int spd_decode_ddr2(struct dimm_attr_ddr2_st *dimm, u8 spd[SPD_SIZE_MAX_DDR2])
{
	u8 spd_size, cl, reg8;
	u16 eeprom_size;
	int ret = SPD_STATUS_OK;

	memset(dimm, 0, sizeof(*dimm));

	spd_size = spd_decode_spd_size_ddr2(spd[0]);
	eeprom_size = spd_decode_eeprom_size_ddr2(spd[1]);

	printram("EEPROM with 0x%04x bytes\n", eeprom_size);
	printram("SPD contains 0x%02x bytes\n", spd_size);

	if (spd_size < 64 || eeprom_size < 64) {
		printk(BIOS_ERR, "SPD too small\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}

	if (spd_ddr2_calc_checksum(spd, spd_size) != spd[63]) {
		printk(BIOS_ERR, "SPD checksum error\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_CRC_ERROR;
	}
	dimm->checksum = spd[63];

	reg8 = spd[62];
	if ((reg8 & 0xf0) != 0x10) {
		printk(BIOS_ERR, "Unsupported SPD revision %01x.%01x\n", reg8 >> 4, reg8 & 0xf);
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}
	dimm->rev = reg8;
	printram("  Revision           : %01x.%01x\n", dimm->rev >> 4, dimm->rev & 0xf);

	reg8 = spd[2];
	printram("  Type               : 0x%02x\n", reg8);
	if (reg8 != 0x08) {
		printk(BIOS_ERR, "Unsupported SPD type %x\n", reg8);
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}
	dimm->dram_type = SPD_MEMORY_TYPE_SDRAM_DDR2;

	dimm->row_bits = spd[3];
	printram("  Rows               : %u\n", dimm->row_bits);
	if ((dimm->row_bits > 31) || ((dimm->row_bits > 15) && (dimm->rev < 0x13))) {
		printk(BIOS_WARNING, "SPD decode: invalid number of memory rows\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	dimm->col_bits = spd[4];
	printram("  Columns            : %u\n", dimm->col_bits);
	if (dimm->col_bits > 15) {
		printk(BIOS_WARNING, "SPD decode: invalid number of memory columns\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	dimm->ranks = (spd[5] & 0x7) + 1;
	printram("  Ranks              : %u\n", dimm->ranks);

	dimm->mod_width = spd[6];
	printram("  Module data width  : x%u\n", dimm->mod_width);
	if (!dimm->mod_width) {
		printk(BIOS_WARNING, "SPD decode: invalid module data width\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	dimm->width = spd[13];
	printram("  SDRAM width        : x%u\n", dimm->width);
	if (!dimm->width) {
		printk(BIOS_WARNING, "SPD decode: invalid SDRAM width\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	dimm->banks = spd[17];
	printram("  Banks              : %u\n", dimm->banks);
	if (!dimm->banks) {
		printk(BIOS_WARNING, "SPD decode: invalid module banks count\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	switch (spd[8]) {
	case 0:
		dimm->flags.operable_5_00V = 1;
		printram("  Voltage            : 5.0V\n");
		break;
	case 1:
		dimm->flags.operable_3_33V = 1;
		printram("  Voltage            : 3.3V\n");
		break;
	case 2:
		dimm->flags.operable_1_50V = 1;
		printram("  Voltage            : 1.5V\n");
		break;
	case 3:
		dimm->flags.operable_3_33V = 1;
		printram("  Voltage            : 3.3V\n");
		break;
	case 4:
		dimm->flags.operable_2_50V = 1;
		printram("  Voltage            : 2.5V\n");
		break;
	case 5:
		dimm->flags.operable_1_80V = 1;
		printram("  Voltage            : 1.8V\n");
		break;
	default:
		printk(BIOS_WARNING, "SPD decode: unknown voltage level.\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	dimm->cas_supported = spd[18];
	if ((dimm->cas_supported & 0x3) || !dimm->cas_supported) {
		printk(BIOS_WARNING, "SPD decode: invalid CAS support advertised.\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	printram("  Supported CAS mask : 0x%x\n", dimm->cas_supported);

	if ((dimm->rev < 0x13) && (dimm->cas_supported & 0x80)) {
		printk(BIOS_WARNING, "SPD decode: invalid CAS support advertised.\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	if ((dimm->rev < 0x12) && (dimm->cas_supported & 0x40)) {
		printk(BIOS_WARNING, "SPD decode: invalid CAS support advertised.\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	/* CL=X */
	cl = spd_get_msbs(dimm->cas_supported);

	/* SDRAM Cycle time at Maximum Supported CAS Latency (CL), CL=X */
	if (spd_decode_tck_time(&dimm->cycle_time[cl], spd[9]) != CB_SUCCESS) {
		printk(BIOS_WARNING, "SPD decode: invalid min tCL for CAS%d\n", cl);
		ret = SPD_STATUS_INVALID_FIELD;
	}
	/* SDRAM Access from Clock */
	if (spd_decode_bcd_time(&dimm->access_time[cl], spd[10]) != CB_SUCCESS) {
		printk(BIOS_WARNING, "SPD decode: invalid min tAC for CAS%d\n", cl);
		ret = SPD_STATUS_INVALID_FIELD;
	}

	if (dimm->cas_supported & (1 << (cl - 1))) {
		/* Minimum Clock Cycle at CLX-1 */
		if (spd_decode_tck_time(&dimm->cycle_time[cl - 1], spd[23]) != CB_SUCCESS) {
			printk(BIOS_WARNING, "SPD decode: invalid min tCL for CAS%d\n", cl - 1);
			ret = SPD_STATUS_INVALID_FIELD;
		}
		/* Maximum Data Access Time (tAC) from Clock at CLX-1 */
		if (spd_decode_bcd_time(&dimm->access_time[cl - 1], spd[24]) != CB_SUCCESS) {
			printk(BIOS_WARNING, "SPD decode: invalid min tAC for CAS%d\n", cl - 1);
			ret = SPD_STATUS_INVALID_FIELD;
		}
	}
	if (dimm->cas_supported & (1 << (cl - 2))) {
		/* Minimum Clock Cycle at CLX-2 */
		if (spd_decode_tck_time(&dimm->cycle_time[cl - 2], spd[25]) != CB_SUCCESS) {
			printk(BIOS_WARNING, "SPD decode: invalid min tCL for CAS%d\n", cl - 2);
			ret = SPD_STATUS_INVALID_FIELD;
		}
		/* Maximum Data Access Time (tAC) from Clock at CLX-2 */
		if (spd_decode_bcd_time(&dimm->access_time[cl - 2], spd[26]) != CB_SUCCESS) {
			printk(BIOS_WARNING, "SPD decode: invalid min tAC for CAS%d\n", cl - 2);
			ret = SPD_STATUS_INVALID_FIELD;
		}
	}

	reg8 = (spd[31] >> 5) | (spd[31] << 3);
	if (!reg8) {
		printk(BIOS_WARNING, "SPD decode: invalid rank density.\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}

	/* Rank density */
	dimm->ranksize_mb = 128 * reg8;
	/* Module density */
	dimm->size_mb = dimm->ranksize_mb * dimm->ranks;
	if (dimm->size_mb < 1024)
		printram("  Capacity           : %u MB\n", dimm->size_mb);
	else
		printram("  Capacity           : %u GB\n", dimm->size_mb >> 10);

	/* SDRAM Maximum Cycle Time (tCKmax) */
	if (spd_decode_bcd_time(&dimm->tCK, spd[43]) != CB_SUCCESS) {
		printk(BIOS_WARNING, "SPD decode: invalid Max tCK\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	/* Minimum Write Recovery Time (tWRmin) */
	dimm->tWR = spd_decode_quarter_time(spd[36]);
	/* Minimum RAS# to CAS# Delay Time (tRCDmin) */
	dimm->tRCD = spd_decode_quarter_time(spd[29]);
	/* Minimum Row Active to Row Active Delay Time (tRRDmin) */
	dimm->tRRD = spd_decode_quarter_time(spd[28]);
	/* Minimum Row Precharge Delay Time (tRPmin) */
	dimm->tRP = spd_decode_quarter_time(spd[27]);
	/* Minimum Active to Precharge Delay Time (tRASmin) */
	dimm->tRAS = spd[30] << 8;
	/* Minimum Active to Active/Refresh Delay Time (tRCmin) */
	/* Minimum Refresh Recovery Delay Time (tRFCmin) */
	spd_decode_tRCtRFC_time(&spd[40], &dimm->tRC, &dimm->tRFC);
	/* Minimum Internal Write to Read Command Delay Time (tWTRmin) */
	dimm->tWTR = spd_decode_quarter_time(spd[37]);
	/* Minimum Internal Read to Precharge Command Delay Time (tRTPmin) */
	dimm->tRTP = spd_decode_quarter_time(spd[38]);
	/* Data Input Setup Time Before Strobe */
	if (spd_decode_bcd_time(&dimm->tDS, spd[34]) != CB_SUCCESS) {
		printk(BIOS_WARNING, "SPD decode: invalid tDS\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	/* Data Input Hold Time After Strobe */
	if (spd_decode_bcd_time(&dimm->tDH, spd[35]) != CB_SUCCESS) {
		printk(BIOS_WARNING, "SPD decode: invalid tDH\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	/* SDRAM Device DQS-DQ Skew for DQS and associated DQ signals */
	dimm->tDQSQ = (spd[44] << 8) / 100;
	/* SDRAM Device Maximum Read Data Hold Skew Factor */
	dimm->tQHS = (spd[45] << 8) / 100;
	/* PLL Relock Time in us */
	dimm->tPLL = spd[46] << 8;
	/* Refresh rate in us */
	if (spd_decode_tRR_time(&dimm->tRR, spd[12]) != CB_SUCCESS)
		ret = SPD_STATUS_INVALID_FIELD;
	dimm->flags.self_refresh = (spd[12] >> 7) & 1;
	printram("The assembly supports self refresh: %s\n",
			dimm->flags.self_refresh ? "true" : "false");

	/* Number of PLLs on DIMM */
	if (dimm->rev >= 0x11)
		dimm->plls = (spd[21] >> 2) & 0x3;

	/* SDRAM Thermal and Refresh Options */
	printram("  General features   :");
	if ((dimm->rev >= 0x12) && (spd[22] & 0x04)) {
		dimm->flags.pasr = 1;
		printram(" PASR");
	}
	if ((dimm->rev >= 0x12) && (spd[22] & 0x02)) {
		dimm->flags.terminate_50ohms = 1;
		printram(" 50Ohm");
	}
	if (spd[22] & 0x01) {
		dimm->flags.weak_driver = 1;
		printram(" WEAK_DRIVER");
	}
	printram("\n");

	/* SDRAM Supported Burst length */
	printram("  Burst length       :");
	if (spd[16] & 0x08) {
		dimm->flags.bl8 = 1;
		printram(" BL8");
	}
	if (spd[16] & 0x04) {
		dimm->flags.bl4 = 1;
		printram(" BL4");
	}
	printram("\n");

	dimm->dimm_type = spd[20] & SPD_DDR2_DIMM_TYPE_MASK;
	printram("  Dimm type          : %x\n", dimm->dimm_type);

	dimm->flags.is_ecc = !!(spd[11] & 0x3);
	printram("  ECC support        : %x\n", dimm->flags.is_ecc);

	dimm->flags.stacked = !!(spd[5] & 0x10);
	printram("  Package            : %s\n", dimm->flags.stacked ? "stack" : "planar");

	if (spd_size > 71) {
		memcpy(&dimm->manufacturer_id, &spd[64], 4);
		printram("  Manufacturer ID    : %x\n", dimm->manufacturer_id);
	}

	if (spd_size > 90) {
		dimm->part_number[16] = 0;
		memcpy(dimm->part_number, &spd[73], 16);
		printram("  Part number        : %s\n", dimm->part_number);
	}

	if (spd_size > 94) {
		dimm->year = spd[93] + 2000;
		dimm->weeks = spd[94];
		printram("  Date               : %d week %d\n", dimm->year, dimm->weeks);
	}

	if (spd_size > 98) {
		memcpy(&dimm->serial, &spd[95], 4);
		printram("  Serial number      : 0x%08x\n", dimm->serial);
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

static void print_us(const char *msg, u32 val)
{
	u32 mant, fp;
	mant = val / 256;
	fp = (val % 256) * 1000 / 256;

	printk(BIOS_INFO, "%s%3u.%.3u us\n", msg, mant, fp);
}

/**
* \brief Print the info in DIMM
*
* Print info about the DIMM. Useful to use when CONFIG(DEBUG_RAM_SETUP) is
* selected, or for a purely informative output.
*
* @param dimm pointer to already decoded @ref dimm_attr structure
*/
void dram_print_spd_ddr2(const struct dimm_attr_ddr2_st *dimm)
{
	char buf[32];
	int i;

	printk(BIOS_INFO, "  Row    addr bits  : %u\n", dimm->row_bits);
	printk(BIOS_INFO, "  Column addr bits  : %u\n", dimm->col_bits);
	printk(BIOS_INFO, "  Number of ranks   : %u\n", dimm->ranks);
	printk(BIOS_INFO, "  DIMM Capacity     : %u MB\n", dimm->size_mb);
	printk(BIOS_INFO, "  Width             : x%u\n", dimm->width);
	printk(BIOS_INFO, "  Banks             : %u\n", dimm->banks);

	/* CAS Latencies Supported */
	printk(BIOS_INFO, "  CAS latencies     :");
	for (i = 2; i < 8; i++) {
		if (dimm->cas_supported & (1 << i))
			printk(BIOS_INFO, " %u", i);
	}
	printk(BIOS_INFO, "\n");

	for (i = 2; i < 8; i++) {
		if (!(dimm->cas_supported & (1 << i)))
			continue;

		strcpy(buf, "  tCK at CLx        : ");
		/* Simple snprintf replacement */
		buf[11] = '0' + i;
		print_ns(buf, dimm->cycle_time[i]);

		strcpy(buf, "  tAC at CLx        : ");
		/* Simple snprintf replacement */
		buf[11] = '0' + i;
		print_ns(buf, dimm->access_time[i]);
	}
	print_ns("  tCKmax            : ", dimm->tCK);
	print_ns("  tWRmin            : ", dimm->tWR);
	print_ns("  tRCDmin           : ", dimm->tRCD);
	print_ns("  tRRDmin           : ", dimm->tRRD);
	print_ns("  tRPmin            : ", dimm->tRP);
	print_ns("  tRASmin           : ", dimm->tRAS);
	print_ns("  tRCmin            : ", dimm->tRC);
	print_ns("  tRFCmin           : ", dimm->tRFC);
	print_ns("  tWTRmin           : ", dimm->tWTR);
	print_ns("  tRTPmin           : ", dimm->tRTP);
	print_ns("  tDS               : ", dimm->tDS);
	print_ns("  tDH               : ", dimm->tDH);
	print_ns("  tDQSQmax          : ", dimm->tDQSQ);
	print_ns("  tQHSmax           : ", dimm->tQHS);
	print_us("  tPLL              : ", dimm->tPLL);
	print_us("  tRR               : ", dimm->tRR);
}

void normalize_tck(u32 *tclk)
{
	if (*tclk <= TCK_800MHZ) {
		*tclk = TCK_800MHZ;
	} else if (*tclk <= TCK_666MHZ) {
		*tclk = TCK_666MHZ;
	} else if (*tclk <= TCK_533MHZ) {
		*tclk = TCK_533MHZ;
	} else if (*tclk <= TCK_400MHZ) {
		*tclk = TCK_400MHZ;
	} else if (*tclk <= TCK_333MHZ) {
		*tclk = TCK_333MHZ;
	} else if (*tclk <= TCK_266MHZ) {
		*tclk = TCK_266MHZ;
	} else if (*tclk <= TCK_200MHZ) {
		*tclk = TCK_200MHZ;
	} else {
		*tclk = 0;
		printk(BIOS_ERR, "Too slow common tCLK found\n");
	}
}
