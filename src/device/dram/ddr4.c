/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/dram/ddr4.h>
#include <string.h>
#include <memory_info.h>
#include <smbios.h>
#include <types.h>

enum ddr4_speed_grade {
	DDR4_1600,
	DDR4_1866,
	DDR4_2133,
	DDR4_2400,
	DDR4_2666,
	DDR4_2933,
	DDR4_3200
};

struct ddr4_speed_attr {
	uint32_t min_clock_mhz; // inclusive
	uint32_t max_clock_mhz; // inclusive
	uint32_t reported_mts;
};

/**
 * DDR4 speed attributes derived from JEDEC 79-4C tables 169 & 170
 *
 * min_clock_mhz = 1000/max_tCk_avg(ns) + 1
 *                 Adding 1 to make minimum inclusive
 * max_clock_mhz = 1000/min_tCk_avg(ns)
 * reported_mts  = Standard reported DDR4 speed in MT/s
 *                 May be 1 less than the actual max MT/s
 */
static const struct ddr4_speed_attr ddr4_speeds[] = {
	[DDR4_1600] = {
		.min_clock_mhz = 668,
		.max_clock_mhz = 800,
		.reported_mts = 1600
	},
	[DDR4_1866] = {
		.min_clock_mhz = 801,
		.max_clock_mhz = 934,
		.reported_mts = 1866
	},
	[DDR4_2133] = {
		.min_clock_mhz = 935,
		.max_clock_mhz = 1067,
		.reported_mts = 2133
	},
	[DDR4_2400] = {
		.min_clock_mhz = 1068,
		.max_clock_mhz = 1200,
		.reported_mts = 2400
	},
	[DDR4_2666] = {
		.min_clock_mhz = 1201,
		.max_clock_mhz = 1333,
		.reported_mts = 2666
	},
	[DDR4_2933] = {
		.min_clock_mhz = 1334,
		.max_clock_mhz = 1466,
		.reported_mts = 2933
	},
	[DDR4_3200] = {
		.min_clock_mhz = 1467,
		.max_clock_mhz = 1600,
		.reported_mts = 3200
	}
};

typedef enum {
	BLOCK_0, /* Base Configuration and DRAM Parameters */
	BLOCK_1,
	BLOCK_1_L, /* Standard Module Parameters */
	BLOCK_1_H, /* Hybrid Module Parameters */
	BLOCK_2,
	BLOCK_2_L, /* Hybrid Module Extended Function Parameters */
	BLOCK_2_H, /* Manufacturing Information */
	BLOCK_3    /* End user programmable */
} spd_block_type;

typedef struct {
	spd_block_type type;
	uint16_t start;     /* starting offset from beginning of the spd */
	uint16_t len;       /* size of the block */
	uint16_t crc_start; /* offset from start of crc bytes, 0 if none */
} spd_block;

/* 'SPD contents architecture' as per datasheet */
const spd_block spd_blocks[] = {
	{.type = BLOCK_0, 0, 128, 126},   {.type = BLOCK_1, 128, 128, 126},
	{.type = BLOCK_1_L, 128, 64, 0},  {.type = BLOCK_1_H, 192, 64, 0},
	{.type = BLOCK_2_L, 256, 64, 62}, {.type = BLOCK_2_H, 320, 64, 0},
	{.type = BLOCK_3, 384, 128, 0} };

static bool verify_block(const spd_block *block, spd_raw_data spd)
{
	uint16_t crc, spd_crc;

	spd_crc = (spd[block->start + block->crc_start + 1] << 8)
		  | spd[block->start + block->crc_start];
	crc = ddr_crc16(&spd[block->start], block->len - 2);

	return spd_crc == crc;
}

/* Check if given block is 'reserved' for a given module type */
static bool block_exists(spd_block_type type, u8 dimm_type)
{
	bool is_hybrid;

	switch (type) {
	case BLOCK_0: /* fall-through */
	case BLOCK_1: /* fall-through */
	case BLOCK_1_L: /* fall-through */
	case BLOCK_1_H: /* fall-through */
	case BLOCK_2_H: /* fall-through */
	case BLOCK_3: /* fall-through */
		return true;
	case BLOCK_2_L:
		is_hybrid = (dimm_type >> 4) & ((1 << 3) - 1);
		if (is_hybrid)
			return true;
		return false;
	default: /* fall-through */
		return false;
	}
}

/**
 * Converts DDR4 clock speed in MHz to the standard reported speed in MT/s
 */
uint16_t ddr4_speed_mhz_to_reported_mts(uint16_t speed_mhz)
{
	for (enum ddr4_speed_grade speed = 0; speed < ARRAY_SIZE(ddr4_speeds); speed++) {
		const struct ddr4_speed_attr *speed_attr = &ddr4_speeds[speed];
		if (speed_mhz >= speed_attr->min_clock_mhz &&
		    speed_mhz <= speed_attr->max_clock_mhz) {
			return speed_attr->reported_mts;
		}
	}
	printk(BIOS_ERR, "DDR4 speed of %d MHz is out of range\n", speed_mhz);
	return 0;
}

/**
 * \brief Decode the raw SPD data
 *
 * Decodes a raw SPD data from a DDR4 DIMM, and organizes it into a
 * @ref dimm_attr structure. The SPD data must first be read in a contiguous
 * array, and passed to this function.
 *
 * @param dimm pointer to @ref dimm_attr structure where the decoded data is to
 *	       be stored
 * @param spd array of raw data previously read from the SPD.
 *
 * @return @ref spd_status enumerator
 *		SPD_STATUS_OK -- decoding was successful
 *		SPD_STATUS_INVALID -- invalid SPD or not a DDR4 SPD
 *		SPD_STATUS_CRC_ERROR -- checksum mismatch
 */
int spd_decode_ddr4(struct dimm_attr_ddr4_st *dimm, spd_raw_data spd)
{
	u8 reg8;
	u8 bus_width, sdram_width;
	u16 cap_per_die_mbit;
	u16 spd_bytes_total, spd_bytes_used;
	const uint16_t spd_bytes_used_table[] = {0, 128, 256, 384, 512};

	/* Make sure that the SPD dump is indeed from a DDR4 module */
	if (spd[2] != SPD_MEMORY_TYPE_DDR4_SDRAM) {
		printk(BIOS_ERR, "Not a DDR4 SPD!\n");
		dimm->dram_type = SPD_MEMORY_TYPE_UNDEFINED;
		return SPD_STATUS_INVALID;
	}

	spd_bytes_total = (spd[0] >> 4) & 0x7;
	spd_bytes_used = spd[0] & 0xf;

	if (!spd_bytes_total || !spd_bytes_used) {
		printk(BIOS_ERR, "SPD failed basic sanity checks\n");
		return SPD_STATUS_INVALID;
	}

	if (spd_bytes_total >= 3)
		printk(BIOS_WARNING, "SPD Bytes Total value is reserved\n");

	spd_bytes_total = 256 << (spd_bytes_total - 1);

	if (spd_bytes_used > 4) {
		printk(BIOS_ERR, "SPD Bytes Used value is reserved\n");
		return SPD_STATUS_INVALID;
	}

	spd_bytes_used = spd_bytes_used_table[spd_bytes_used];

	if (spd_bytes_used > spd_bytes_total) {
		printk(BIOS_ERR, "SPD Bytes Used is greater than SPD Bytes Total\n");
		return SPD_STATUS_INVALID;
	}

	/* Verify CRC of blocks that have them, do not step over 'used' length */
	for (int i = 0; i < ARRAY_SIZE(spd_blocks); i++) {
		/* this block is not checksummed */
		if (spd_blocks[i].crc_start == 0)
			continue;
		/* we shouldn't have this block */
		if (spd_blocks[i].start + spd_blocks[i].len > spd_bytes_used)
			continue;
		/* check if block exists in the current schema */
		if (!block_exists(spd_blocks[i].type, spd[3]))
			continue;
		if (!verify_block(&spd_blocks[i], spd)) {
			printk(BIOS_ERR, "CRC failed for block %d\n", i);
			return SPD_STATUS_CRC_ERROR;
		}
	}

	dimm->dram_type = SPD_MEMORY_TYPE_DDR4_SDRAM;
	dimm->dimm_type = spd[3] & ((1 << 4) - 1);

	reg8 = spd[13] & ((1 << 4) - 1);
	dimm->bus_width = reg8;
	bus_width = 8 << (reg8 & ((1 << 3) - 1));

	reg8 = spd[12] & ((1 << 3) - 1);
	dimm->sdram_width = reg8;
	sdram_width = 4 << reg8;

	reg8 = spd[4] & ((1 << 4) - 1);
	dimm->cap_per_die_mbit = reg8;
	cap_per_die_mbit = (1 << reg8) * 256;

	reg8 = (spd[12] >> 3) & ((1 << 3) - 1);
	dimm->ranks = reg8 + 1;

	if (!bus_width || !sdram_width) {
		printk(BIOS_ERR, "SPD information is invalid");
		dimm->size_mb = 0;
		return SPD_STATUS_INVALID;
	}

	/* seems to be only one, in mV */
	dimm->vdd_voltage = 1200;

	/* calculate size */
	/* FIXME: this is wrong for 3DS devices */
	dimm->size_mb = cap_per_die_mbit / 8 * bus_width / sdram_width * dimm->ranks;

	dimm->ecc_extension = spd[SPD_PRIMARY_SDRAM_WIDTH] & SPD_ECC_8BIT;

	/* make sure we have the manufacturing information block */
	if (spd_bytes_used > 320) {
		dimm->manufacturer_id = (spd[351] << 8) | spd[350];
		memcpy(dimm->part_number, &spd[329], SPD_DDR4_PART_LEN);
		dimm->part_number[SPD_DDR4_PART_LEN] = 0;
		memcpy(dimm->serial_number, &spd[325], sizeof(dimm->serial_number));
	}
	return SPD_STATUS_OK;
}

enum cb_err spd_add_smbios17_ddr4(const u8 channel, const u8 slot, const u16 selected_freq,
				  const struct dimm_attr_ddr4_st *info)
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
		dimm->ddr_type = MEMORY_TYPE_DDR4;
		dimm->ddr_frequency = selected_freq;
		dimm->dimm_size = info->size_mb;
		dimm->channel_num = channel;
		dimm->rank_per_dimm = info->ranks;
		dimm->dimm_num = slot;
		memcpy(dimm->module_part_number, info->part_number, SPD_DDR4_PART_LEN);
		dimm->mod_id = info->manufacturer_id;

		switch (info->dimm_type) {
		case SPD_DDR4_DIMM_TYPE_SO_DIMM:
			dimm->mod_type = DDR4_SPD_SODIMM;
			break;
		case SPD_DDR4_DIMM_TYPE_72B_SO_RDIMM:
			dimm->mod_type = DDR4_SPD_72B_SO_RDIMM;
			break;
		case SPD_DDR4_DIMM_TYPE_UDIMM:
			dimm->mod_type = DDR4_SPD_UDIMM;
			break;
		case SPD_DDR4_DIMM_TYPE_RDIMM:
			dimm->mod_type = DDR4_SPD_RDIMM;
			break;
		default:
			dimm->mod_type = SPD_UNDEFINED;
			break;
		}

		dimm->bus_width = info->bus_width;
		memcpy(dimm->serial, info->serial_number,
		       MIN(sizeof(dimm->serial), sizeof(info->serial_number)));

		dimm->vdd_voltage = info->vdd_voltage;
		mem_info->dimm_cnt++;
	}

	return CB_SUCCESS;
}
