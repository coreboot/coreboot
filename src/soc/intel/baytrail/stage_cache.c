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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cbmem.h>
#include <ramstage_cache.h>
#include <baytrail/smm.h>

struct ramstage_cache *ramstage_cache_location(long *size)
{
	char *smm_base;
	/* 1MiB cache size */
	const long cache_size = CONFIG_SMM_RESERVED_SIZE;

	/* Ramstage cache lives in TSEG region which is the definition of
	 * cbmem_top(). */
	smm_base = cbmem_top();
	*size = cache_size;
	return (void *)&smm_base[smm_region_size() - cache_size];
}
