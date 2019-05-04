/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#define K4E6E304EB_MEM_ID	0x5

#define MEM_SINGLE_CHAN0	0x0
#define MEM_SINGLE_CHAN3	0x3
#define MEM_SINGLE_CHAN4	0x4
#define MEM_SINGLE_CHAN7	0x7
#define MEM_SINGLE_CHANB	0xb
#define MEM_SINGLE_CHANC	0xc

void variant_memory_init_params(
		MEMORY_INIT_UPD *const params, const int spd_index)
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

	/*Strengthen the Rcomp Target Ctrl for 8GB K4E6E304EB -EGCF*/
	const u16 StrengthendRcompTarget[5] = { 100, 40, 40, 21, 40 };

	/* Default Rcomp Target assignment */
	const u16 *targeted_rcomp = RcompTarget;

	/* Override Rcomp Target assignment for specific SKU(s) */
	if (spd_index == K4E6E304EB_MEM_ID)
		targeted_rcomp = StrengthendRcompTarget;

	memcpy(params->DqByteMapCh0, dq_map,
			sizeof(params->DqByteMapCh0) * 2);
	memcpy(params->DqsMapCpu2DramCh0, dqs_map,
			sizeof(params->DqsMapCpu2DramCh0) * 2);
	memcpy(params->RcompResistor, RcompResistor,
			sizeof(params->RcompResistor));
	memcpy(params->RcompTarget, targeted_rcomp,
			sizeof(params->RcompTarget));
}

int is_dual_channel(const int spd_index)
{
	return (spd_index != MEM_SINGLE_CHAN0
		&& spd_index != MEM_SINGLE_CHAN3
		&& spd_index != MEM_SINGLE_CHAN4
		&& spd_index != MEM_SINGLE_CHAN7
		&& spd_index != MEM_SINGLE_CHANB
		&& spd_index != MEM_SINGLE_CHANC);
}
