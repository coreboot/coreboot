/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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
#include <arch/byteorder.h>
#include <cbfs.h>
#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include "boardid.h"
#include "spd.h"

void mainboard_fill_dq_map_data(void *dq_map_ptr)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0 ,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0 ,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dqs_map_data(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 6, 5, 4, 7 },
		{ 2, 3, 0, 1, 6, 7, 4, 5 } };
	memcpy(dqs_map_ptr, dqs_map, sizeof(dqs_map));
}

void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };
	memcpy(rcomp_ptr, RcompResistor,
		 sizeof(RcompResistor));
}

void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	int mem_cfg_id;

	mem_cfg_id = get_spd_index();
	/* Rcomp target */
	static const u16 RcompTarget[RCOMP_TARGET_PARAMS] = {
		100, 40, 40, 23, 40 };

	/* Strengthen the Rcomp Target Ctrl for 8GB K4E6E304EE -EGCF */
	static const u16 StrengthendRcompTarget[RCOMP_TARGET_PARAMS] = {
		100, 40, 40, 21, 40 };


	if (mem_cfg_id == K4E6E304EE_MEM_ID) {
		memcpy(rcomp_strength_ptr, StrengthendRcompTarget,
			sizeof(StrengthendRcompTarget));
	} else {
		memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
	}

}

uintptr_t mainboard_get_spd_data(void)
{
	char *spd_file;
	int spd_index, spd_span;
	size_t spd_file_len;

	spd_index = get_spd_index();
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
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spd_index = 0;
	}

	spd_span = spd_index * SPD_LEN;
	return (uintptr_t)(spd_file + spd_span);
}

int mainboard_has_dual_channel_mem(void)
{
	int spd_index;

	spd_index = get_spd_index();

	if (spd_index != HYNIX_SINGLE_CHAN && spd_index != SAMSUNG_SINGLE_CHAN
		&& spd_index != MIC_SINGLE_CHAN) {
		printk(BIOS_INFO,
			"Dual channel SPD detected writing second channel\n");
		return 1;
	}
	return 0;
}
