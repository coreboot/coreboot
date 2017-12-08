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
#include <string.h>

/* Rcomp resistor */
static const u16 rcomp_resistor_ddp[] = { 121, 81, 100 };
static const u16 rcomp_resistor_sdp[] = { 200, 81, 100 };

/* Rcomp target */
static const u16 rcomp_target[] = { 100, 40, 20, 20, 26 };

/* Memory ids are 1-indexed, so subtract 1 to use 0-indexed values in bitmap. */
#define MEM_ID(x)	(1 << ((x) - 1))

/* Bitmap to indicate which memory ids are using DDP. */
static const uint16_t ddp_bitmap = MEM_ID(4);

void variant_memory_params(struct memory_params *p)
{
	memset(p, 0, sizeof(*p));
	p->type = MEMORY_DDR4;

	/* Rcomp resistor values are different for SDP and DDP. */
	if (ddp_bitmap & MEM_ID(variant_memory_sku())) {
		p->rcomp_resistor = rcomp_resistor_ddp;
		p->rcomp_resistor_size = sizeof(rcomp_resistor_ddp);
	} else {
		p->rcomp_resistor = rcomp_resistor_sdp;
		p->rcomp_resistor_size = sizeof(rcomp_resistor_sdp);
	}

	p->rcomp_target = rcomp_target;
	p->rcomp_target_size = sizeof(rcomp_target);
}
