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

#include <arch/early_variables.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <imd.h>
#include <rules.h>
#include <stage_cache.h>
#include <string.h>

static struct imd imd_stage_cache CAR_GLOBAL = { };

static inline struct imd *imd_get(void)
{
	return car_get_var_ptr(&imd_stage_cache);
}

static void stage_cache_create_empty(void)
{
	struct imd *imd;
	void *base;
	size_t size;

	imd = imd_get();
	stage_cache_external_region(&base, &size);
	imd_handle_init(imd, (void *)(size + (uintptr_t)base));

	printk(BIOS_DEBUG, "External stage cache:\n");
	imd_create_tiered_empty(imd, 4096, 4096, 1024, 32);
	if (imd_limit_size(imd, size))
		printk(BIOS_DEBUG, "Could not limit stage cache size.\n");
}

static void stage_cache_recover(void)
{
	struct imd *imd;
	void *base;
	size_t size;

	imd = imd_get();
	stage_cache_external_region(&base, &size);
	imd_handle_init(imd, (void *)(size + (uintptr_t)base));
	if (imd_recover(imd))
		printk(BIOS_DEBUG, "Unable to recover external stage cache.\n");
}

void stage_cache_add(int stage_id, const struct prog *stage)
{
	struct imd *imd;
	const struct imd_entry *e;
	struct stage_cache *meta;
	void *c;

	imd = imd_get();
	e = imd_entry_add(imd, CBMEM_ID_STAGEx_META + stage_id, sizeof(*meta));

	if (e == NULL)
		return;

	meta = imd_entry_at(imd, e);

	meta->load_addr = (uintptr_t)prog_start(stage);
	meta->entry_addr = (uintptr_t)prog_entry(stage);

	e = imd_entry_add(imd, CBMEM_ID_STAGEx_CACHE + stage_id,
				prog_size(stage));

	if (e == NULL)
		return;

	c = imd_entry_at(imd, e);

	memcpy(c, prog_start(stage), prog_size(stage));
}

void stage_cache_load_stage(int stage_id, struct prog *stage)
{
	struct imd *imd;
	struct stage_cache *meta;
	const struct imd_entry *e;
	void *c;
	size_t size;

	imd = imd_get();
	e = imd_entry_find(imd, CBMEM_ID_STAGEx_META + stage_id);
	if (e == NULL)
		return;

	meta = imd_entry_at(imd, e);

	e = imd_entry_find(imd, CBMEM_ID_STAGEx_CACHE + stage_id);

	if (e == NULL)
		return;

	c = imd_entry_at(imd, e);
	size = imd_entry_size(imd, e);

	memcpy((void *)(uintptr_t)meta->load_addr, c, size);

	prog_set_area(stage, (void *)(uintptr_t)meta->load_addr, size);
	prog_set_entry(stage, (void *)(uintptr_t)meta->entry_addr, NULL);
}

static void stage_cache_setup(int is_recovery)
{
	if (is_recovery)
		stage_cache_recover();
	else
		stage_cache_create_empty();
}

ROMSTAGE_CBMEM_INIT_HOOK(stage_cache_setup)
RAMSTAGE_CBMEM_INIT_HOOK(stage_cache_setup)
