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

#include <arch/early_variables.h>
#include <cbmem.h>
#include <stage_cache.h>
#include <string.h>

/* Stage cache uses cbmem. */
void stage_cache_add(int stage_id, const struct prog *stage)
{
	struct stage_cache *meta;
	void *c;

	meta = cbmem_add(CBMEM_ID_STAGEx_META + stage_id, sizeof(*meta));
	if (meta == NULL)
		return;
	meta->load_addr = (uintptr_t)prog_start(stage);
	meta->entry_addr = (uintptr_t)prog_entry(stage);

	c = cbmem_add(CBMEM_ID_STAGEx_CACHE + stage_id, prog_size(stage));
	if (c == NULL)
		return;

	memcpy(c, prog_start(stage), prog_size(stage));
}

void stage_cache_load_stage(int stage_id, struct prog *stage)
{
	struct stage_cache *meta;
	const struct cbmem_entry *e;
	void *c;
	size_t size;
	void *load_addr;

	prog_set_entry(stage, NULL, NULL);

	meta = cbmem_find(CBMEM_ID_STAGEx_META + stage_id);
	if (meta == NULL)
		return;

	e = cbmem_entry_find(CBMEM_ID_STAGEx_CACHE + stage_id);

	if (e == NULL)
		return;

	c = cbmem_entry_start(e);
	size = cbmem_entry_size(e);
	load_addr = (void *)(uintptr_t)meta->load_addr;

	memcpy(load_addr, c, size);

	prog_set_area(stage, load_addr, size);
	prog_set_entry(stage, (void *)(uintptr_t)meta->entry_addr, NULL);
}
