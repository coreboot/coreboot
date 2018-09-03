/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <baseboard/variants.h>
#include <variant/sku.h>

/* DQ byte map */
static const u8 dq_map[][12] = {
	{ 0x0F, 0xF0, 0x00, 0xF0, 0x0F, 0xF0,
	  0x0F, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
	{ 0x33, 0xCC, 0x00, 0xCC, 0x33, 0xCC,
	  0x33, 0x00, 0xFF, 0x00, 0xFF, 0x00 }
};

/* DQS CPU<>DRAM map */
static const u8 dqs_map[][8] = {
	{ 0, 1, 3, 2, 4, 5, 6, 7 },
	{ 1, 0, 4, 5, 2, 3, 6, 7 },
};

/* Rcomp resistor */
static const u16 rcomp_resistor[] = { 200, 81, 162 };

/* Rcomp target */
static const u16 rcomp_target[] = { 100, 40, 40, 23, 40 };

void variant_memory_params(struct memory_params *p)
{
	p->type = MEMORY_LPDDR3;
	p->dq_map = dq_map;
	p->dq_map_size = sizeof(dq_map);
	p->dqs_map = dqs_map;
	p->dqs_map_size = sizeof(dqs_map);
	p->rcomp_resistor = rcomp_resistor;
	p->rcomp_resistor_size = sizeof(rcomp_resistor);
	p->rcomp_target = rcomp_target;
	p->rcomp_target_size = sizeof(rcomp_target);

	switch (variant_board_sku()) {
	case SKU_0_NAUTILUS:
		/* Bumping VCC_SA voltage offset 75mV to allow a
		 * tolerance to PLLGT on Nautilus-Wifi sku */
		p->enable_sa_oc_support = 1;
		p->sa_voltage_offset_val = 75;
		break;
	}
}
