/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <console/console.h>
#include <soc/bdb.h>

void set_bdb_pointers(u8 *start_addr, struct bdb_pointer *bdb_in)
{
	bdb_in->bdb_h
		= (struct bdb_header *)start_addr;

	bdb_in->bdb_k
		= (struct bdb_key *)(start_addr
			+ (bdb_in->bdb_h->struct_size));

	bdb_in->bdb_oem_0
		= (u8 *)((u32)(bdb_in->bdb_k)
			+ (bdb_in->bdb_k->struct_size));

	bdb_in->sub_k
		= (struct bdb_key *)((u32)(bdb_in->bdb_oem_0)
			+ (bdb_in->bdb_h->oem_area_0_size));

	bdb_in->bdb_h_s
		= (struct bdb_sig *)((u32)(bdb_in->bdb_oem_0)
			+ (bdb_in->bdb_h->signed_size));

	bdb_in->bdb_d
		= (struct bdb_data *)((u32)(bdb_in->bdb_h_s)
			+ (bdb_in->bdb_h_s->struct_size));

	bdb_in->oem_1
		= (u8 *)((u32)(bdb_in->bdb_d)
			+ (bdb_in->bdb_d->struct_size));

	bdb_in->bdb_hash
		= (struct bdb_hash *)((u32)(bdb_in->oem_1)
			+ (bdb_in->bdb_d->oem_area_1_size));

	bdb_in->bdb_s
		= (struct bdb_sig *)((u32)(bdb_in->bdb_d)
			+ (bdb_in->bdb_d->signed_size));
}

struct bdb_hash *find_bdb_image(struct bdb_pointer *bdb_info, u32 image_type)
{
	int i;

	if (!bdb_info) {
		printk(BIOS_DEBUG, "can't find BDB\n");
		return NULL;
	}

	for (i = 0; i < bdb_info->bdb_d->num_hashes; i++) {
		if (bdb_info->bdb_hash[i].type == image_type)
			return &bdb_info->bdb_hash[i];
	}

	printk(BIOS_DEBUG, "can't find image with type %d in the BDB\n",
	       image_type);
	return NULL;
}
