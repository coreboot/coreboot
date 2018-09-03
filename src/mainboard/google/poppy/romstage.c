/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <baseboard/variants.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <string.h>
#include <variant/gpio.h>

#include <fsp/soc_binding.h>

/* Offset to identify DRAM type. */
#define SPD_DRAM_TYPE_OFF	2
#define SPD_DRAM_LPDDR3	0xf1
#define SPD_DRAM_DDR4		0x0c

/* Length of SPD data. */
#define SPD_LEN_LPDDR3		256
#define SPD_LEN_DDR4		512

/* Fields that are common across different memory types. */
#define SPD_DENSITY_BANKS_OFF	4
#define SPD_ADDRESSING_OFF	5
#define SPD_PART_LEN		18

/* Fields that are different depending upon memory type. */
#define SPD_ORG_OFF_LPDDR3	7
#define SPD_BUSW_OFF_LPDDR3	8
#define SPD_PART_OFF_LPDDR3	128

#define SPD_ORG_OFF_DDR4	12
#define SPD_BUSW_OFF_DDR4	13
#define SPD_PART_OFF_DDR4	329

#define SPD_INFO(_type)				\
	[MEMORY_##_type] = {				\
		.str = #_type,				\
		.type_code = SPD_DRAM_##_type,		\
		.len = SPD_LEN_##_type,		\
		.org_off = SPD_ORG_OFF_##_type,	\
		.busw_off = SPD_BUSW_OFF_##_type,	\
		.part_off = SPD_PART_OFF_##_type,	\
	}

static const struct dram_info {
	const char *str;
	uint16_t type_code;
	uint16_t len;
	uint16_t org_off;
	uint16_t busw_off;
	uint16_t part_off;
} spd_info[MEMORY_COUNT] = {
	SPD_INFO(LPDDR3),
	SPD_INFO(DDR4),
};

static void mainboard_print_spd_info(const uint8_t *spd, enum memory_type type)
{
	const int spd_banks[8] = {  8, 16, 32, 64, -1, -1, -1, -1 };
	const int spd_capmb[8] = {  1,  2,  4,  8, 16, 32, 64,  0 };
	const int spd_rows[8]  = { 12, 13, 14, 15, 16, -1, -1, -1 };
	const int spd_cols[8]  = {  9, 10, 11, 12, -1, -1, -1, -1 };
	const int spd_ranks[8] = {  1,  2,  3,  4, -1, -1, -1, -1 };
	const int spd_devw[8]  = {  4,  8, 16, 32, -1, -1, -1, -1 };
	const int spd_busw[8]  = {  8, 16, 32, 64, -1, -1, -1, -1 };
	char spd_name[SPD_PART_LEN+1] = { 0 };
	const struct dram_info *info = &spd_info[type];

	assert (info->type_code == spd[SPD_DRAM_TYPE_OFF]);

	/* Module type */
	printk(BIOS_INFO, "SPD: module type is %s\n", info->str);

	int banks = spd_banks[(spd[SPD_DENSITY_BANKS_OFF] >> 4) & 7];
	int capmb = spd_capmb[spd[SPD_DENSITY_BANKS_OFF] & 7] * 256;
	int rows  = spd_rows[(spd[SPD_ADDRESSING_OFF] >> 3) & 7];
	int cols  = spd_cols[spd[SPD_ADDRESSING_OFF] & 7];
	int ranks = spd_ranks[(spd[info->org_off] >> 3) & 7];
	int devw  = spd_devw[spd[info->org_off] & 7];
	int busw  = spd_busw[spd[info->busw_off] & 7];

	/* Module Part Number */
	memcpy(spd_name, &spd[info->part_off], SPD_PART_LEN);
	spd_name[SPD_PART_LEN] = 0;
	printk(BIOS_INFO, "SPD: module part is %s\n", spd_name);

	printk(BIOS_INFO,
		"SPD: banks %d, ranks %d, rows %d, columns %d, density %d Mb\n",
		banks, ranks, rows, cols, capmb);
	printk(BIOS_INFO, "SPD: device width %d bits, bus width %d bits\n",
		devw, busw);

	if (capmb > 0 && busw > 0 && devw > 0 && ranks > 0) {
		/* SIZE = DENSITY / 8 * BUS_WIDTH / SDRAM_WIDTH * RANKS */
		printk(BIOS_INFO, "SPD: module size is %u MB (per channel)\n",
			capmb / 8 * busw / devw * ranks);
	}
}

static uintptr_t mainboard_get_spd_data(enum memory_type type, bool use_sec_spd)
{
	char *spd_file;
	size_t spd_file_len;
	int spd_index;
	const size_t spd_len = spd_info[type].len;
	const char *spd_bin = use_sec_spd ? "sec-spd.bin" : "spd.bin";

	spd_index = variant_memory_sku();
	assert(spd_index >= 0);
	printk(BIOS_INFO, "SPD index %d\n", spd_index);

	/* Load SPD data from CBFS */
	spd_file = cbfs_boot_map_with_leak(spd_bin, CBFS_TYPE_SPD,
					   &spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	/* make sure we have at least one SPD in the file. */
	if (spd_file_len < spd_len)
		die("Missing SPD data.");

	/* Make sure we did not overrun the buffer */
	if (spd_file_len < ((spd_index + 1) * spd_len))
		die("Invalid SPD index.");

	spd_index *= spd_len;
	mainboard_print_spd_info((uint8_t *)(spd_file + spd_index), type);

	return (uintptr_t)(spd_file + spd_index);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	struct memory_params p;

	const struct pad_config *pads;
	size_t num;

	pads = variant_romstage_gpio_table(&num);
	gpio_configure_pads(pads, num);

	memset(&p, 0, sizeof(p));
	variant_memory_params(&p);

	assert(p.type < MEMORY_COUNT);

	if (p.dq_map && p.dq_map_size)
		memcpy(&mem_cfg->DqByteMapCh0, p.dq_map, p.dq_map_size);

	if (p.dqs_map && p.dqs_map_size)
		memcpy(&mem_cfg->DqsMapCpu2DramCh0, p.dqs_map, p.dqs_map_size);

	memcpy(&mem_cfg->RcompResistor, p.rcomp_resistor,
		p.rcomp_resistor_size);
	memcpy(&mem_cfg->RcompTarget, p.rcomp_target, p.rcomp_target_size);

	mem_cfg->MemorySpdPtr00 = mainboard_get_spd_data(p.type, p.use_sec_spd);
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
	mem_cfg->MemorySpdDataLen = spd_info[p.type].len;

	mem_cfg->SaOcSupport = p.enable_sa_oc_support;
	mem_cfg->SaVoltageOffset = p.sa_voltage_offset_val;
}
