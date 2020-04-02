/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <string.h>
#include "spd.h"

void mainboard_fill_dq_map_data(void *dq_map_ch0, void *dq_map_ch1)
{
	/* DQ byte map */
	const u8 dq_map[2][12] = {
		  { 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
		    0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
		  { 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
		    0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 } };
	memcpy(dq_map_ch0, dq_map[0], sizeof(dq_map[0]));
	memcpy(dq_map_ch1, dq_map[1], sizeof(dq_map[1]));
}

void mainboard_fill_dqs_map_data(void *dqs_map_ch0, void *dqs_map_ch1)
{
	/* DQS CPU<>DRAM map */
	const u8 dqs_map[2][8] = {
		{ 0, 1, 3, 2, 4, 5, 6, 7 },
		{ 1, 0, 4, 5, 2, 3, 6, 7 } };
	memcpy(dqs_map_ch0, dqs_map[0], sizeof(dqs_map[0]));
	memcpy(dqs_map_ch1, dqs_map[1], sizeof(dqs_map[1]));
}

void mainboard_fill_rcomp_res_data(void *rcomp_ptr)
{
	/* Rcomp resistor */
	const u16 RcompResistor[3] = { 121, 81, 100 };
	memcpy(rcomp_ptr, RcompResistor, sizeof(RcompResistor));
}

void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr)
{
	/* Rcomp target */
	static const u16 RcompTarget[5] = { 100, 40, 20, 20, 26 };
	memcpy(rcomp_strength_ptr, RcompTarget, sizeof(RcompTarget));
}
