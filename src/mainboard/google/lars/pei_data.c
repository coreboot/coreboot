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
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include "boardid.h"

/* PCH_MEM_CFG[3:0] */
#define MAX_MEMORY_CONFIG	0x10
#define RCOMP_TARGET_PARAMS	0x5

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0 ,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0 ,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 6, 5, 4, 7 },
		{ 2, 3, 0, 1, 6, 7, 4, 5 } };

	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 200, 81, 162 };

	/* Rcomp target */
	static const u16 RcompTarget[MAX_MEMORY_CONFIG][RCOMP_TARGET_PARAMS] = {
		{ 100, 40, 40, 23, 40 },
		{ 100, 40, 40, 23, 40 },
		{ 100, 40, 40, 23, 40 },
		/*Strengthen the Rcomp Target Ctrl for 8GB K4E6E304EE -EGCF*/
		{ 100, 40, 40, 21, 40 }, };


	memcpy(pei_data->dq_map, dq_map, sizeof(dq_map));
	memcpy(pei_data->dqs_map, dqs_map, sizeof(dqs_map));
	memcpy(pei_data->RcompResistor, RcompResistor,
		 sizeof(RcompResistor));
	memcpy(pei_data->RcompTarget, &RcompTarget[pei_data->mem_cfg_id][0],
		 sizeof(RcompTarget[pei_data->mem_cfg_id]));

}
