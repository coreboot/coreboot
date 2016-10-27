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
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		    0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dqs_map_data(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 4, 5, 6, 7 },
		{ 1, 0, 4, 5, 2, 3, 6, 7 } };
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
	/* Rcomp target */
	static const u16 RcompTarget[RCOMP_TARGET_PARAMS] = {
		100, 40, 40, 23, 40 };

	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}

uintptr_t mainboard_get_spd_data(void)
{
	char *spd_file;
	int spd_index, spd_span;
	size_t spd_file_len;

	spd_index = 0;
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
	return 1;
}
