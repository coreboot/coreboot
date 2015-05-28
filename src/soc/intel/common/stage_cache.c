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

#include <cbmem.h>
#include <soc/intel/common/memmap.h>
#include <soc/smm.h>
#include <stage_cache.h>

void stage_cache_external_region(void **base, size_t *size)
{
	char *smm_base;
	size_t smm_size;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	/*
	 * The ramstage cache lives in the TSEG region.
	 * The top of ram is defined to be the TSEG base address.
	 */
	smm_region((void **)&smm_base, &smm_size);
	*size = cache_size;
	*base = (void *)(&smm_base[smm_size - cache_size]);
}
