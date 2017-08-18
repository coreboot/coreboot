/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 * Copyright (C) 2017 Purism SPC.
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
#include "pei_data.h"

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
	const u16 RcompResistor[3] = { 121, 81, 100 };
	memcpy(rcomp_ptr, RcompResistor,
		 sizeof(RcompResistor));
}

void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	/* Rcomp target */
	const u16 RcompTarget[5] = { 100, 40, 20, 20, 26 };
	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	mainboard_fill_dq_map_data(&pei_data->dq_map);
	mainboard_fill_dqs_map_data(&pei_data->dqs_map);
	mainboard_fill_rcomp_res_data(&pei_data->RcompResistor);
	mainboard_fill_rcomp_strength_data(&pei_data->RcompTarget);
}
