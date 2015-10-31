/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <cbmem.h>
#include <stage_cache.h>
#include <soc/smm.h>

void stage_cache_external_region(void **base, size_t *size)
{
	char *smm_base;
	/* 1MiB cache size */
	const long cache_size = CONFIG_SMM_RESERVED_SIZE;

	/* Ramstage cache lives in TSEG region which is the definition of
	 * cbmem_top(). */
	smm_base = cbmem_top();
	*size = cache_size;
	*base = &smm_base[smm_region_size() - cache_size];
}
