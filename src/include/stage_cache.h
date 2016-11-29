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
 */

#ifndef _STAGE_CACHE_H_
#define _STAGE_CACHE_H_

#include <stddef.h>
#include <stdint.h>
#include <program_loading.h>

enum {
	STAGE_RAMSTAGE,
	STAGE_REFCODE,
	STAGE_POSTCAR,
};

/* Cache the loaded stage provided according to the parameters. */
void stage_cache_add(int stage_id, const struct prog *stage);
/* Load the cached stage at given location returning the stage entry point. */
void stage_cache_load_stage(int stage_id, struct prog *stage);
/* Fill in parameters for the external stage cache, if utilized. */
void stage_cache_external_region(void **base, size_t *size);

/* Metadata associated with each stage. */
struct stage_cache {
	uint64_t load_addr;
	uint64_t entry_addr;
};

#endif /* _STAGE_CACHE_H_ */
