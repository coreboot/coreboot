/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <stage_cache.h>
#include <types.h>

int __weak smm_subregion(int sub, uintptr_t *base, size_t *size)
{
	return -1;
}

void __weak stage_cache_external_region(void **base, size_t *size)
{
	if (smm_subregion(SMM_SUBREGION_CACHE, (uintptr_t *)base, size)) {
		printk(BIOS_ERR, "ERROR: No cache SMM subregion.\n");
		*base = NULL;
		*size = 0;
	}
}
