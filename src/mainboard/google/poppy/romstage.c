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
#include <soc/romstage.h>
#include <string.h>

#include <fsp/soc_binding.h>

#define SPD_LEN		256

#define SPD_DRAM_TYPE		2
#define  SPD_DRAM_DDR3		0x0b
#define  SPD_DRAM_LPDDR3	0xf1
#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8
#define SPD_PART_OFF		128
#define  SPD_PART_LEN		18
#define SPD_MANU_OFF		148

static void mainboard_print_spd_info(uint8_t spd[])
{
	const int spd_banks[8] = {  8, 16, 32, 64, -1, -1, -1, -1 };
	const int spd_capmb[8] = {  1,  2,  4,  8, 16, 32, 64,  0 };
	const int spd_rows[8]  = { 12, 13, 14, 15, 16, -1, -1, -1 };
	const int spd_cols[8]  = {  9, 10, 11, 12, -1, -1, -1, -1 };
	const int spd_ranks[8] = {  1,  2,  3,  4, -1, -1, -1, -1 };
	const int spd_devw[8]  = {  4,  8, 16, 32, -1, -1, -1, -1 };
	const int spd_busw[8]  = {  8, 16, 32, 64, -1, -1, -1, -1 };
	char spd_name[SPD_PART_LEN+1] = { 0 };

	int banks = spd_banks[(spd[SPD_DENSITY_BANKS] >> 4) & 7];
	int capmb = spd_capmb[spd[SPD_DENSITY_BANKS] & 7] * 256;
	int rows  = spd_rows[(spd[SPD_ADDRESSING] >> 3) & 7];
	int cols  = spd_cols[spd[SPD_ADDRESSING] & 7];
	int ranks = spd_ranks[(spd[SPD_ORGANIZATION] >> 3) & 7];
	int devw  = spd_devw[spd[SPD_ORGANIZATION] & 7];
	int busw  = spd_busw[spd[SPD_BUS_DEV_WIDTH] & 7];

	/* Module type */
	printk(BIOS_INFO, "SPD: module type is ");
	switch (spd[SPD_DRAM_TYPE]) {
	case SPD_DRAM_LPDDR3:
		printk(BIOS_INFO, "LPDDR3\n");
		break;
	default:
		printk(BIOS_INFO, "Unknown (%02x)\n", spd[SPD_DRAM_TYPE]);
		return;
	}

	/* Module Part Number */
	memcpy(spd_name, &spd[SPD_PART_OFF], SPD_PART_LEN);
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

static uintptr_t mainboard_get_spd_data(void)
{
	char *spd_file;
	size_t spd_file_len;
	int spd_index;

	spd_index = variant_memory_sku();
	assert(spd_index >= 0);
	printk(BIOS_INFO, "SPD index %d\n", spd_index);

	/* Load SPD data from CBFS */
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
		&spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	/* make sure we have at least one SPD in the file. */
	if (spd_file_len < SPD_LEN)
		die("Missing SPD data.");

	/* Make sure we did not overrun the buffer */
	if (spd_file_len < ((spd_index + 1) * SPD_LEN)) {
		printk(BIOS_ERR, "SPD index override to 1 - old hardware?\n");
		spd_index = 1;
	}

	spd_index *= SPD_LEN;
	mainboard_print_spd_info((uint8_t *)(spd_file + spd_index));

	return (uintptr_t)(spd_file + spd_index);
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	struct memory_params p;

	variant_memory_params(&p);

	memcpy(&mem_cfg->DqByteMapCh0, p.dq_map, p.dq_map_size);
	memcpy(&mem_cfg->DqsMapCpu2DramCh0, p.dqs_map, p.dqs_map_size);
	memcpy(&mem_cfg->RcompResistor, p.rcomp_resistor,
		p.rcomp_resistor_size);
	memcpy(&mem_cfg->RcompTarget, p.rcomp_target, p.rcomp_target_size);

	mem_cfg->MemorySpdPtr00 = mainboard_get_spd_data();
	mem_cfg->MemorySpdPtr10 = mem_cfg->MemorySpdPtr00;
	mem_cfg->MemorySpdDataLen = SPD_LEN;
}
