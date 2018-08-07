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
		0x0F, 0xF0, 0x0F, 0xF0, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dq_map_ch1(void *dq_map_ptr)
{
	const u8 dq_map[12] = {
		0x0F, 0xF0, 0x0F, 0xF0, 0xFF, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	memcpy(dq_map_ptr, dq_map, sizeof(dq_map));
}

void mainboard_fill_dqs_map_ch0(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map Ch0 */
	const u8 dqs_map_u[8] = { 0, 3, 2, 1, 5, 6, 7, 4 };

	const u8 dqs_map_y[8] = { 2, 0, 3, 1, 6, 5, 7, 4 };

	if (IS_ENABLED(CONFIG_BOARD_INTEL_COFFEELAKE_RVPU))
		memcpy(dqs_map_ptr, dqs_map_u, sizeof(dqs_map_u));
	else
		memcpy(dqs_map_ptr, dqs_map_y, sizeof(dqs_map_y));
}

void mainboard_fill_dqs_map_ch1(void *dqs_map_ptr)
{
	/* DQS CPU<>DRAM map Ch1 */
	const u8 dqs_map_u[8] = { 3, 0, 1, 2, 5, 6, 4, 7 };

	const u8 dqs_map_y[8] = { 3, 1, 2, 0, 4, 5, 6, 7 };

	if (IS_ENABLED(CONFIG_BOARD_INTEL_COFFEELAKE_RVPU))
		memcpy(dqs_map_ptr, dqs_map_u, sizeof(dqs_map_u));
	else
		memcpy(dqs_map_ptr, dqs_map_y, sizeof(dqs_map_y));
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
			80, 40, 40, 40, 30 };

	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}
