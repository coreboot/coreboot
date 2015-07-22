/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/byteorder.h>
#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <soc/gpio.h>
#include <soc/pei_data.h>
#include <soc/pcr.h>
#include <soc/romstage.h>
#include "spd.h"

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
	case SPD_DRAM_DDR3:
		printk(BIOS_INFO, "DDR3\n");
		break;
	case SPD_DRAM_LPDDR3:
		printk(BIOS_INFO, "LPDDR3\n");
		break;
	default:
		printk(BIOS_INFO, "Unknown (%02x)\n", spd[SPD_DRAM_TYPE]);
		break;
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

/* Copy SPD data for on-board memory */
void mainboard_fill_spd_data(struct pei_data *pei_data)
{
	char *spd_file;
	size_t spd_file_len;
	int spd_index;
	int spd_gpio[4];

	/*************************************************************
	 * FIXME: Remove when real GPIO support is ready.
	 */
	GPIO_PAD gpio_set[4] = {
		GPIO_LP_GPP_C12,	/* PCH_MEM_CONFIG[0] */
		GPIO_LP_GPP_C13,	/* PCH_MEM_CONFIG[1] */
		GPIO_LP_GPP_C14,	/* PCH_MEM_CONFIG[2] */
		GPIO_LP_GPP_C15,	/* PCH_MEM_CONFIG[3] */
	};
	int index;

	for (index = 0; index < ARRAY_SIZE(gpio_set); index++) {
		u32 number = GPIO_GET_PAD_NUMBER(gpio_set[index]);
		u32 cfgreg = 8 * number + R_PCH_PCR_GPIO_GPP_C_PADCFG_OFFSET;
		/*
		 * Set GPIO mode and enable input
		 * Clear PMODE0 | PMODE1 | GPIORXDIS
		 */
		u32 dw0mask = (1 << 10) | (1 << 11) | (1 << 9);
		u32 dw0reg = 0;
		pcr_andthenor32(PID_GPIOCOM1, cfgreg, ~dw0mask, dw0reg);

		/* Read current input value */
		pcr_read32(PID_GPIOCOM1, cfgreg, &dw0reg);
		spd_gpio[index] = !!(dw0reg & (1 << 1));
	}
	/*************************************************************/

	spd_index = (spd_gpio[3] << 3) | (spd_gpio[2] << 2) |
		(spd_gpio[1] << 1) | spd_gpio[0];

	printk(BIOS_DEBUG,
	       "SPD: index %d (GPP_C15=%d GPP_C14=%d GPP_C13=%d GPP_C12=%d)\n",
	       spd_index, spd_gpio[3], spd_gpio[2], spd_gpio[1], spd_gpio[0]);

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

	/* Assume same memory in both channels */
	spd_index *= SPD_LEN;
	memcpy(pei_data->spd_data[0][0], spd_file + spd_index, SPD_LEN);
	memcpy(pei_data->spd_data[1][0], spd_file + spd_index, SPD_LEN);

	/* Make sure a valid SPD was found */
	if (pei_data->spd_data[0][0][0] == 0)
		die("Invalid SPD data.");

	mainboard_print_spd_info(pei_data->spd_data[0][0]);
}
