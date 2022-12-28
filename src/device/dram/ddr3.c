/* SPDX-License-Identifier: GPL-2.0-or-later */

/**
 * @file ddr3.c
 *
 * \brief Utilities for decoding DDR3 SPDs
 */

#include <console/console.h>
#include <device/device.h>
#include <device/dram/ddr3.h>
#include <device/dram/common.h>
#include <string.h>
#include <memory_info.h>
#include <cbmem.h>
#include <smbios.h>
#include <types.h>

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
int spd_dimm_is_registered_ddr3(enum spd_dimm_type_ddr3 type)
{
	if ((type == SPD_DDR3_DIMM_TYPE_RDIMM) | (type == SPD_DDR3_DIMM_TYPE_MINI_RDIMM) |
			(type == SPD_DDR3_DIMM_TYPE_72B_SO_RDIMM))
		return 1;

	return 0;
}

/**
 * \brief Calculate the CRC of a DDR3 SPD
 *
 * @param spd pointer to raw SPD data
 * @param len length of data in SPD
 *
 * @return the CRC of the SPD data, or 0 when spd data is truncated.
 */
u16 spd_ddr3_calc_crc(u8 *spd, int len)
{
	int n_crc;

	/* Find the number of bytes covered by CRC */
	if (spd[0] & 0x80) {
		n_crc = 117;
	} else {
		n_crc = 126;
	}

	if (len < n_crc)
		/* Not enough bytes available to get the CRC */
		return 0;

	return ddr_crc16(spd, n_crc);
}

/**
 * \brief Calculate the CRC of a DDR3 SPD unique identifier
 *
 * @param spd pointer to raw SPD data
 * @param len length of data in SPD
 *
 * @return the CRC of SPD data bytes 117..127, or 0 when spd data is truncated.
 */
u16 spd_ddr3_calc_unique_crc(u8 *spd, int len)
{
	if (len < (117 + 11))
		/* Not enough bytes available to get the CRC */
		return 0;

	return ddr_crc16(&spd[117], 11);
}

/**
 * \brief Decode the raw SPD data
 *
 * Decodes a raw SPD data from a DDR3 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr structure where the decoded data is to
 *	       be stored
 * @param spd array of raw data previously read from the SPD.
 *
 * @return @ref spd_status enumerator
 *		SPD_STATUS_OK -- decoding was successful
 *		SPD_STATUS_INVALID -- invalid SPD or not a DDR3 SPD
 *		SPD_STATUS_CRC_ERROR -- CRC did not verify
 *		SPD_STATUS_INVALID_FIELD -- A field with an invalid value was
 *					    detected.
 */
int spd_decode_ddr3(struct dimm_attr_ddr3_st *dimm, spd_raw_data spd)
{
	int ret;
	u16 crc, spd_crc;
	u8 capacity_shift, bus_width;
	u8 reg8;
	u32 mtb; /* medium time base */
	u32 ftb; /* fine time base */
	unsigned int val;

	ret = SPD_STATUS_OK;

	/* Don't assume we memset 0 dimm struct. Clear all our flags */
	dimm->flags.raw = 0;
	dimm->dimms_per_channel = 3;

	/* Make sure that the SPD dump is indeed from a DDR3 module */
	if (spd[2] != SPD_MEMORY_TYPE_SDRAM_DDR3) {
		printram("Not a DDR3 SPD!\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}
	dimm->dram_type = SPD_MEMORY_TYPE_SDRAM_DDR3;
	dimm->dimm_type = spd[3] & 0xf;

	crc = spd_ddr3_calc_crc(spd, sizeof(spd_raw_data));
	/* Compare with the CRC in the SPD */
	spd_crc = (spd[127] << 8) + spd[126];
	/* Verify the CRC is correct */
	if (crc != spd_crc) {
		printram("ERROR: SPD CRC failed!!!\n");
		ret = SPD_STATUS_CRC_ERROR;
	};

	printram("  Revision           : %x\n", spd[1]);
	printram("  Type               : %x\n", spd[2]);
	printram("  Key                : %x\n", spd[3]);

	reg8 = spd[4];
	/* Number of memory banks */
	val = (reg8 >> 4) & 0x07;
	if (val > 0x03) {
		printram("  Invalid number of memory banks\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	printram("  Banks              : %u\n", 1 << (val + 3));
	/* SDRAM capacity */
	capacity_shift = reg8 & 0x0f;
	if (capacity_shift > 0x06) {
		printram("  Invalid module capacity\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	if (capacity_shift < 0x02) {
		printram("  Capacity           : %u Mb\n", 256 << capacity_shift);
	} else {
		printram("  Capacity           : %u Gb\n", 1 << (capacity_shift - 2));
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
	printram("  Supported voltages :");
	if (reg8 & (1 << 2)) {
		dimm->flags.operable_1_25V = 1;
		dimm->voltage = 1250;
		printram(" 1.25V");
	}
	if (reg8 & (1 << 1)) {
		dimm->flags.operable_1_35V = 1;
		dimm->voltage = 1300;
		printram(" 1.35V");
	}
	if (!(reg8 & (1 << 0))) {
		dimm->flags.operable_1_50V = 1;
		dimm->voltage = 1500;
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
	dimm->width = (4 << val);
	printram("  SDRAM width        : %u\n", dimm->width);

	/* Memory bus width */
	reg8 = spd[8];
	/* Bus extension */
	val = (reg8 >> 3) & 0x03;
	if (val > 1) {
		printram("  Invalid bus extension\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	dimm->flags.is_ecc = val ? 1 : 0;
	printram("  Bus extension      : %u bits\n", val ? 8 : 0);
	/* Bus width */
	val = reg8 & 0x07;
	if (val > 3) {
		printram("  Invalid bus width\n");
		ret = SPD_STATUS_INVALID_FIELD;
	}
	bus_width = 8 << val;
	printram("  Bus width          : %u\n", bus_width);

	/* We have all the info we need to compute the dimm size */
	/* Capacity is 256Mbit multiplied by the power of 2 specified in
	 * capacity_shift
	 * The rest is the JEDEC formula */
	dimm->size_mb =
		((1 << (capacity_shift + (25 - 20))) * bus_width * dimm->ranks) / dimm->width;

	/* Medium Timebase =
	 *   Medium Timebase (MTB) Dividend /
	 *   Medium Timebase (MTB) Divisor */
	mtb = (((u32)spd[10]) << 8) / spd[11];

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
	/* Minimum CAS Write Latency Time (tCWLmin)
	 * - not present in standard SPD */
	dimm->tCWL = 0;
	/* System CMD Rate Mode - not present in standard SPD */
	dimm->tCMD = 0;

	printram("  FTB timings        :");
	/* FTB is introduced in SPD revision 1.1 */
	if (spd[1] >= 0x11 && spd[9] & 0x0f) {
		printram(" yes\n");

		/* Fine timebase (1/256 ps) =
		 *   Fine Timebase (FTB) Dividend /
		 *   Fine Timebase (FTB) Divisor */
		ftb = (((u16)spd[9] & 0xf0) << 4) / (spd[9] & 0x0f);

		/* SPD recommends to round up the MTB part and use a negative
		 * FTB, so a negative rounding should be always safe */

		/* SDRAM Minimum Cycle Time (tCKmin) correction */
		dimm->tCK += (s32)((s8)spd[34] * ftb - 500) / 1000;
		/* Minimum CAS Latency Time (tAAmin) correction */
		dimm->tAA += (s32)((s8)spd[35] * ftb - 500) / 1000;
		/* Minimum RAS# to CAS# Delay Time (tRCDmin) correction */
		dimm->tRCD += (s32)((s8)spd[36] * ftb - 500) / 1000;
		/* Minimum Row Precharge Delay Time (tRPmin) correction */
		dimm->tRP += (s32)((s8)spd[37] * ftb - 500) / 1000;
		/* Minimum Active to Active/Refresh Delay Time (tRCmin) corr. */
		dimm->tRC += (s32)((s8)spd[38] * ftb - 500) / 1000;
	} else {
		printram(" no\n");
	}

	/* SDRAM Optional Features */
	reg8 = spd[30];
	printram("  Optional features  :");
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
	printram("  Thermal features   :");
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
	printram("  Thermal sensor     : %s\n", dimm->flags.therm_sensor ? "yes" : "no");

	/*  SDRAM Device Type */
	printram("  Standard SDRAM     : %s\n", (spd[33] & 0x80) ? "no" : "yes");

	if (spd[63] & 0x01) {
		dimm->flags.pins_mirrored = 1;
	}
	printram("  Rank1 Address bits : %s\n", (spd[63] & 0x01) ? "mirrored" : "normal");

	dimm->reference_card = spd[62] & 0x1f;
	printram("  DIMM Reference card: %c\n", 'A' + dimm->reference_card);

	dimm->manufacturer_id = (spd[118] << 8) | spd[117];
	printram("  Manufacturer ID    : %x\n", dimm->manufacturer_id);

	dimm->part_number[16] = 0;
	memcpy(dimm->part_number, &spd[128], 16);
	printram("  Part number        : %s\n", dimm->part_number);

	memcpy(dimm->serial, &spd[SPD_DIMM_SERIAL_NUM], SPD_DIMM_SERIAL_LEN);

	return ret;
}

/**
 * \brief Decode the raw SPD XMP data
 *
 * Decodes a raw SPD XMP data from a DDR3 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr structure where the decoded data is to
 *        be stored
 * @param spd array of raw data previously read from the SPD.
 *
 * @param profile select one of the profiles to load
 *
 * @return @ref spd_status enumerator
 *		SPD_STATUS_OK -- decoding was successful
 *		SPD_STATUS_INVALID -- invalid SPD or not a DDR3 SPD
 *		SPD_STATUS_CRC_ERROR -- CRC did not verify
 *		SPD_STATUS_INVALID_FIELD -- A field with an invalid value was
 *					    detected.
 */
int spd_xmp_decode_ddr3(struct dimm_attr_ddr3_st *dimm, spd_raw_data spd,
			enum ddr3_xmp_profile profile)
{
	int ret;
	u32 mtb; /* medium time base */
	u8 *xmp; /* pointer to XMP profile data */

	/* need a valid SPD */
	ret = spd_decode_ddr3(dimm, spd);
	if (ret != SPD_STATUS_OK)
		return ret;

	/* search for magic header */
	if (spd[176] != 0x0C || spd[177] != 0x4A) {
		printram("Not a DDR3 XMP profile!\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}

	if (profile == DDR3_XMP_PROFILE_1) {
		if (!(spd[178] & 1)) {
			printram("Selected XMP profile disabled!\n");
			dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
			return SPD_STATUS_INVALID;
		}

		printram("  XMP Profile        : 1\n");
		xmp = &spd[185];

		/* Medium Timebase =
		 *   Medium Timebase (MTB) Dividend /
		 *   Medium Timebase (MTB) Divisor */
		mtb = (((u32)spd[180]) << 8) / spd[181];

		dimm->dimms_per_channel = ((spd[178] >> 2) & 0x3) + 1;
	} else {
		if (!(spd[178] & 2)) {
			printram("Selected XMP profile disabled!\n");
			dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
			return SPD_STATUS_INVALID;
		}
		printram("  XMP Profile        : 2\n");
		xmp = &spd[220];

		/* Medium Timebase =
		 *   Medium Timebase (MTB) Dividend /
		 *   Medium Timebase (MTB) Divisor */
		mtb = (((u32)spd[182]) << 8) / spd[183];

		dimm->dimms_per_channel = ((spd[178] >> 4) & 0x3) + 1;
	}

	printram("  Max DIMMs/channel  : %u\n", dimm->dimms_per_channel);

	printram("  XMP Revision       : %u.%u\n", spd[179] >> 4, spd[179] & 0xf);

	/* calculate voltage in mV */
	dimm->voltage = (xmp[0] & 1) * 50;
	dimm->voltage += ((xmp[0] >> 1) & 0xf) * 100;
	dimm->voltage += ((xmp[0] >> 5) & 0x3) * 1000;

	printram("  Requested voltage  : %u mV\n", dimm->voltage);

	/* SDRAM Minimum Cycle Time (tCKmin) */
	dimm->tCK = xmp[1] * mtb;
	/* CAS Latencies Supported */
	dimm->cas_supported = ((xmp[4] << 8) + xmp[3]) & 0x7fff;
	/* Minimum CAS Latency Time (tAAmin) */
	dimm->tAA = xmp[2] * mtb;
	/* Minimum Write Recovery Time (tWRmin) */
	dimm->tWR = xmp[8] * mtb;
	/* Minimum RAS# to CAS# Delay Time (tRCDmin) */
	dimm->tRCD = xmp[7] * mtb;
	/* Minimum Row Active to Row Active Delay Time (tRRDmin) */
	dimm->tRRD = xmp[17] * mtb;
	/* Minimum Row Precharge Delay Time (tRPmin) */
	dimm->tRP = xmp[6] * mtb;
	/* Minimum Active to Precharge Delay Time (tRASmin) */
	dimm->tRAS = (((xmp[9] & 0x0f) << 8) + xmp[10]) * mtb;
	/* Minimum Active to Active/Refresh Delay Time (tRCmin) */
	dimm->tRC = (((xmp[9] & 0xf0) << 4) + xmp[11]) * mtb;
	/* Minimum Refresh Recovery Delay Time (tRFCmin) */
	dimm->tRFC = ((xmp[15] << 8) + xmp[14]) * mtb;
	/* Minimum Internal Write to Read Command Delay Time (tWTRmin) */
	dimm->tWTR = xmp[20] * mtb;
	/* Minimum Internal Read to Precharge Command Delay Time (tRTPmin) */
	dimm->tRTP = xmp[16] * mtb;
	/* Minimum Four Activate Window Delay Time (tFAWmin) */
	dimm->tFAW = (((xmp[18] & 0x0f) << 8) + xmp[19]) * mtb;
	/* Minimum CAS Write Latency Time (tCWLmin) */
	dimm->tCWL = xmp[5] * mtb;
	/* System CMD Rate Mode */
	dimm->tCMD = xmp[23] * mtb;

	return ret;
}

/**
 * Fill cbmem with information for SMBIOS type 17.
 *
 * @param channel Corresponding channel of provided @info
 * @param slot Corresponding slot of provided @info
 * @param selected_freq The actual frequency the DRAM is running on
 * @param info DIMM parameters read from SPD
 *
 * @return CB_SUCCESS if DIMM info was written
 */
enum cb_err spd_add_smbios17(const u8 channel, const u8 slot, const u16 selected_freq,
			     const struct dimm_attr_ddr3_st *info)
{
	struct memory_info *mem_info;
	struct dimm_info *dimm;

	/*
	 * Allocate CBMEM area for DIMM information used to populate SMBIOS
	 * table 17
	 */
	mem_info = cbmem_find(CBMEM_ID_MEMINFO);
	if (!mem_info) {
		mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));

		printk(BIOS_DEBUG, "CBMEM entry for DIMM info: %p\n", mem_info);
		if (!mem_info)
			return CB_ERR;

		memset(mem_info, 0, sizeof(*mem_info));
	}

	if (mem_info->dimm_cnt >= ARRAY_SIZE(mem_info->dimm)) {
		printk(BIOS_WARNING, "BUG: Too many DIMM infos for %s.\n", __func__);
		return CB_ERR;
	}

	dimm = &mem_info->dimm[mem_info->dimm_cnt];
	if (info->size_mb) {
		dimm->ddr_type = MEMORY_TYPE_DDR3;
		dimm->ddr_frequency = selected_freq;
		dimm->dimm_size = info->size_mb;
		dimm->channel_num = channel;
		dimm->rank_per_dimm = info->ranks;
		dimm->dimm_num = slot;
		memcpy(dimm->module_part_number, info->part_number, 16);
		dimm->mod_id = info->manufacturer_id;
		dimm->mod_type = info->dimm_type;
		dimm->bus_width = MEMORY_BUS_WIDTH_64; // non-ECC only
		memcpy(dimm->serial, info->serial,
		       MIN(sizeof(dimm->serial), sizeof(info->serial)));
		mem_info->dimm_cnt++;
	}

	return CB_SUCCESS;
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
* Print info about the DIMM. Useful to use when CONFIG(DEBUG_RAM_SETUP) is
* selected, or for a purely informative output.
*
* @param dimm pointer to already decoded @ref dimm_attr structure
*/
void dram_print_spd_ddr3(const struct dimm_attr_ddr3_st *dimm)
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
	/* Those values are only relevant if an XMP profile sets them */
	if (dimm->tCWL)
		print_ns("  tCWLmin           : ", dimm->tCWL);
	if (dimm->tCMD)
		printk(BIOS_INFO, "  tCMDmin           : %3u\n", DIV_ROUND_UP(dimm->tCMD, 256));
}
