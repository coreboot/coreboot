/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/cache.h>
#include <arm_tf.h>
#include <assert.h>
#include <soc/addressmap.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>

typedef struct bl31_plat_params {
	uint32_t tzdram_size;
} bl31_plat_params_t;

static bl31_plat_params_t t210_plat_params;

void *soc_get_bl31_plat_params(bl31_params_t *params)
{
	uintptr_t tz_base_mib;
	size_t tz_size_mib;

	carveout_range(CARVEOUT_TZ, &tz_base_mib, &tz_size_mib);

	assert(tz_size_mib < 4096);
	t210_plat_params.tzdram_size = tz_size_mib * MiB;

	dcache_clean_by_mva(&t210_plat_params, sizeof(t210_plat_params));

	return &t210_plat_params;
}
