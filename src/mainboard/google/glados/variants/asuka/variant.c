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
 */

#include <stdint.h>
#include <string.h>
#include <baseboard/variant.h>
#include <fsp/soc_binding.h>

void variant_memory_init_params(
		MEMORY_INIT_UPD *const memory_params, const int spd_index)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		{ 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		  0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		{ 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		  0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 6, 5, 4, 7 },
		{ 2, 3, 0, 1, 6, 7, 4, 5 } };

	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };

	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 40, 23, 40 };

	memcpy(memory_params->DqByteMapCh0, dq_map,
			sizeof(memory_params->DqByteMapCh0) * 2);
	memcpy(memory_params->DqsMapCpu2DramCh0, dqs_map,
			sizeof(memory_params->DqsMapCpu2DramCh0) * 2);
	memcpy(memory_params->RcompResistor, RcompResistor,
			sizeof(memory_params->RcompResistor));
	memcpy(memory_params->RcompTarget, RcompTarget,
			sizeof(memory_params->RcompTarget));
}

int is_dual_channel(const int spd_index)
{
	/* Per Makefile.inc, dual channel indices 1,3,5 */
	return (spd_index & 0x1);
}
