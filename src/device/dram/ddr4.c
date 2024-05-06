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
	[DDR4_1600] = {.min_clock_mhz =  668, .max_clock_mhz =  800, .reported_mts = 1600},
	[DDR4_1866] = {.min_clock_mhz =  801, .max_clock_mhz =  934, .reported_mts = 1866},
	[DDR4_2133] = {.min_clock_mhz =  935, .max_clock_mhz = 1067, .reported_mts = 2133},
	[DDR4_2400] = {.min_clock_mhz = 1068, .max_clock_mhz = 1200, .reported_mts = 2400},
	[DDR4_2666] = {.min_clock_mhz = 1201, .max_clock_mhz = 1333, .reported_mts = 2666},
	[DDR4_2933] = {.min_clock_mhz = 1334, .max_clock_mhz = 1466, .reported_mts = 2933},
	[DDR4_3200] = {.min_clock_mhz = 1467, .max_clock_mhz = 1600, .reported_mts = 3200}
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
	{.type = BLOCK_3, 384, 128, 0}
};

static bool verify_block(const spd_block *block, spd_ddr4_raw_data spd)
{
	uint16_t crc, spd_crc;

	spd_crc = (spd[block->start + block->crc_start + 1] << 8) |
		  spd[block->start + block->crc_start];
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
int spd_decode_ddr4(struct dimm_attr_ddr4_st *dimm, spd_ddr4_raw_data spd)
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

/* Returns MRS command */
static uint32_t ddr4_wr_to_mr0_map(u8 wr)
{
	static const unsigned int enc[] = {0, 1, 2, 3, 4, 5, 7, 6, 8};
	int wr_idx = wr/2 - 5;
	if (wr_idx < 0 || wr_idx >= ARRAY_SIZE(enc))
		die("WR index out of bounds: %d (derived from %d)\n", wr_idx, wr);

	return enc[wr_idx] << 9;
}

/* Returns MRS command */
static uint32_t ddr4_cas_to_mr0_map(u8 cas)
{
	static const unsigned int enc[] = {
		/*
		 * The only non-zero bits are at positions (LSB0): 12, 6, 5, 4, 2.
		 */
		0x0000,		/* CL = 9 */
		0x0004,		/* CL = 10 */
		0x0010,		/* CL = 11 */
		0x0014,		/* CL = 12 */
		0x0020,		/* CL = 13 */
		0x0024,		/* CL = 14 */
		0x0030,		/* CL = 15 */
		0x0034,		/* CL = 16 */
		0x0064,		/* CL = 17 */
		0x0040,		/* CL = 18 */
		0x0070,		/* CL = 19 */
		0x0044,		/* CL = 20 */
		0x0074,		/* CL = 21 */
		0x0050,		/* CL = 22 */
		0x0060,		/* CL = 23 */
		0x0054,		/* CL = 24 */
		0x1000,		/* CL = 25 */
		0x1004,		/* CL = 26 */
		0x1010,		/* CL = 27 (only 3DS) */
		0x1014,		/* CL = 28 */
		0x1020,		/* reserved for CL = 29 */
		0x1024,		/* CL = 30 */
		0x1030,		/* reserved for CL = 31 */
		0x1034,		/* CL = 32 */
	};

	int cas_idx = cas - 9;
	if (cas_idx < 0 || cas_idx >= ARRAY_SIZE(enc))
		die("CAS index out of bounds: %d (derived from %d)\n", cas_idx, cas);

	return enc[cas_idx];
}

uint32_t ddr4_get_mr0(u8 write_recovery,
		      enum ddr4_mr0_dll_reset dll_reset,
		      u8 cas,
		      enum ddr4_mr0_burst_type burst_type,
		      enum ddr4_mr0_burst_length burst_length)
{
	uint32_t cmd = 0 << 20;

	cmd |= ddr4_wr_to_mr0_map(write_recovery);
	cmd |= dll_reset << 8;
	cmd |= DDR4_MR0_MODE_NORMAL << 7;
	cmd |= ddr4_cas_to_mr0_map(cas);
	cmd |= burst_type << 3;
	cmd |= burst_length << 0;

	return cmd;
}

uint32_t ddr4_get_mr1(enum ddr4_mr1_qoff qoff,
		      enum ddr4_mr1_tdqs tdqs,
		      enum ddr4_mr1_rtt_nom rtt_nom,
		      enum ddr4_mr1_write_leveling write_leveling,
		      enum ddr4_mr1_odimp output_drive_impedance,
		      enum ddr4_mr1_additive_latency additive_latency,
		      enum ddr4_mr1_dll dll_enable)
{
	uint32_t cmd = 1 << 20;

	cmd |= qoff << 12;
	cmd |= tdqs << 11;
	cmd |= rtt_nom << 8;
	cmd |= write_leveling << 7;
	cmd |= output_drive_impedance << 1;
	cmd |= additive_latency << 3;
	cmd |= dll_enable << 0;

	return cmd;
}

/* Returns MRS command */
static uint32_t ddr4_cwl_to_mr2_map(u8 cwl)
{
	/* Encoding is (starting with 0): 9, 10, 11, 12, 14, 16, 18, 20 */
	if (cwl < 14)
		cwl -= 9;
	else
		cwl = (cwl - 14) / 2 + 4;

	return cwl << 3;
}

uint32_t ddr4_get_mr2(enum ddr4_mr2_wr_crc wr_crc,
		      enum ddr4_mr2_rtt_wr rtt_wr,
		      enum ddr4_mr2_lp_asr self_refresh, u8 cwl)
{
	uint32_t cmd = 2 << 20;

	cmd |= wr_crc << 12;
	cmd |= rtt_wr << 9;
	cmd |= self_refresh << 6;
	cmd |= ddr4_cwl_to_mr2_map(cwl);

	return cmd;
}

uint32_t ddr4_get_mr3(enum ddr4_mr3_mpr_read_format mpr_read_format,
		      enum ddr4_mr3_wr_cmd_lat_crc_dm command_latency_crc_dm,
		      enum ddr4_mr3_fine_gran_ref fine_refresh,
		      enum ddr4_mr3_temp_sensor_readout temp_sensor,
		      enum ddr4_mr3_pda pda,
		      enum ddr4_mr3_geardown_mode geardown,
		      enum ddr4_mr3_mpr_operation mpr_operation,
		      u8 mpr_page)
{
	uint32_t cmd = 3 << 20;

	cmd |= mpr_read_format << 11;
	cmd |= command_latency_crc_dm << 9;
	cmd |= fine_refresh << 6;
	cmd |= temp_sensor << 5;
	cmd |= pda << 4;
	cmd |= geardown << 3;
	cmd |= mpr_operation << 2;
	cmd |= (mpr_page & 3) << 0;

	return cmd;
}

uint32_t ddr4_get_mr4(enum ddr4_mr4_hppr hppr,
		      enum ddr4_mr4_wr_preamble wr_preamble,
		      enum ddr4_mr4_rd_preamble rd_preamble,
		      enum ddr4_mr4_rd_preamble_training rd_preamble_train,
		      enum ddr4_mr4_self_refr_abort self_ref_abrt,
		      enum ddr4_mr4_cs_to_cmd_latency cs2cmd_lat,
		      enum ddr4_mr4_sppr sppr,
		      enum ddr4_mr4_internal_vref_mon int_vref_mon,
		      enum ddr4_mr4_temp_controlled_refr temp_ctrl_ref,
		      enum ddr4_mr4_max_pd_mode max_pd)
{
	uint32_t cmd = 4 << 20;

	cmd |= hppr << 13;
	cmd |= wr_preamble << 12;
	cmd |= rd_preamble << 11;
	cmd |= rd_preamble_train << 10;
	cmd |= self_ref_abrt << 9;
	cmd |= cs2cmd_lat << 6;
	cmd |= sppr << 5;
	cmd |= int_vref_mon << 4;
	cmd |= temp_ctrl_ref << 2;
	cmd |= max_pd << 1;

	return cmd;
}

uint32_t ddr4_get_mr5(enum ddr4_mr5_rd_dbi rd_dbi,
		      enum ddr4_mr5_wr_dbi wr_dbi,
		      enum ddr4_mr5_data_mask dm,
		      enum ddr4_mr5_rtt_park rtt_park,
		      enum ddr4_mr5_odt_pd odt_pd,
		      enum ddr4_mr5_ca_parity_lat pl)
{
	uint32_t cmd = 5 << 20;

	cmd |= rd_dbi << 12;
	cmd |= wr_dbi << 11;
	cmd |= dm << 10;
	cmd |= rtt_park << 6;
	cmd |= odt_pd << 5;
	cmd |= pl << 0;

	return cmd;
}

/* Returns MRS command */
static uint32_t ddr4_tccd_l_to_mr6_map(u8 tccd_l)
{
	if (tccd_l < 4 || tccd_l > 8)
		die("tCCD_l out of range: %d\n", tccd_l);

	return (tccd_l - 4) << 10;
}

uint32_t ddr4_get_mr6(u8 tccd_l,
		      enum ddr4_mr6_vrefdq_training vrefdq_training,
		      enum ddr4_mr6_vrefdq_training_range range,
		      u8 vrefdq_value)
{
	uint32_t cmd = 6 << 20;

	cmd |= ddr4_tccd_l_to_mr6_map(tccd_l);
	cmd |= vrefdq_training << 7;
	cmd |= range << 6;
	cmd |= vrefdq_value & 0x3F;

	return cmd;
}

/*
 * ZQCL: A16 = H, A15 = H, A14 = L, A10 = H, rest either L or H
 * ZQCS: A16 = H, A15 = H, A14 = L, A10 = L, rest either L or H
 */
uint32_t ddr4_get_zqcal_cmd(enum ddr4_zqcal_ls long_short)
{
	uint32_t cmd = 1 << 16 | 1 << 15;

	cmd |= long_short << 10;

	return cmd;
}
