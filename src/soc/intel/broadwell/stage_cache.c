/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <cbmem.h>
#include <soc/smm.h>
#include <stage_cache.h>
#include <stdint.h>

void stage_cache_external_region(void **base, size_t *size)
{
	/* The ramstage cache lives in the TSEG region.
	 * The top of ram is defined to be the TSEG base address. */
	u32 offset = smm_region_size();
	offset -= CONFIG_IED_REGION_SIZE;
	offset -= CONFIG_SMM_RESERVED_SIZE;

	*base = (void *)(cbmem_top() + offset);
	*size = CONFIG_SMM_RESERVED_SIZE;
}
