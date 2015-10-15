/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <console/console.h>
#include <fsp/memmap.h>
#include <stage_cache.h>

void stage_cache_external_region(void **base, size_t *size)
{
	if (smm_subregion(SMM_SUBREGION_CACHE, base, size)) {
		printk(BIOS_ERR, "ERROR: No cache SMM subregion.\n");
		*base = NULL;
		*size = 0;
	}
}
