/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
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
#include "spd.h"

void mainboard_fill_dq_map_ch0(void *dq_map_ptr)
{
	/* DQ byte map Ch0 */
	const u8 dq_map[12] = {
		0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 };

	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dq_map_ch1(void *dq_map_ptr)
{
	const u8 dq_map[12] = {
		0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 };

	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dqs_map_ch0(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map Ch0 */
	const u8 dqs_map_h[8] = { 0, 1, 3, 2, 4, 5, 6, 7 };

	memcpy(dqs_map_ptr, dqs_map_h, sizeof(dqs_map_h));
}

void mainboard_fill_dqs_map_ch1(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map Ch1 */
	const u8 dqs_map_h[8] = { 1, 0, 4, 5, 2, 3, 6, 7 };

	memcpy(dqs_map_ptr, dqs_map_h, sizeof(dqs_map_h));
}

void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 100, 100, 100 };
	memcpy(rcomp_ptr, RcompResistor, sizeof(RcompResistor));
}

void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	/* Rcomp target */
	static const u16 RcompTarget[RCOMP_TARGET_PARAMS] = {
			100, 33, 32, 33, 28 };

	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}
