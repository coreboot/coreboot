/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <stddef.h>
#include <stdint.h>
#include <console/console.h>
#include <cbmem.h>
#include <timestamp.h>
#include <cpu/x86/car.h>

#define MAX_TIMESTAMPS 30

static struct timestamp_table* ts_table CAR_GLOBAL = NULL;
static tsc_t ts_basetime CAR_GLOBAL = { .lo = 0, .hi =0 };

static void timestamp_stash(enum timestamp_id id, tsc_t ts_time);

static uint64_t tsc_to_uint64(tsc_t tstamp)
{
	return (((uint64_t)tstamp.hi) << 32) + tstamp.lo;
}

static void timestamp_real_init(tsc_t base)
{
	struct timestamp_table* tst;

	tst = cbmem_add(CBMEM_ID_TIMESTAMP,
			sizeof(struct timestamp_table) +
			MAX_TIMESTAMPS * sizeof(struct timestamp_entry));

	if (!tst) {
		printk(BIOS_ERR, "ERROR: failed to allocate timestamp table\n");
		return;
	}

	tst->base_time = tsc_to_uint64(base);
	tst->max_entries = MAX_TIMESTAMPS;
	tst->num_entries = 0;

	ts_table = tst;
}

void timestamp_add(enum timestamp_id id, tsc_t ts_time)
{
	struct timestamp_entry *tse;

	if (!ts_table) {
		timestamp_stash(id, ts_time);
		return;
	}
	if (ts_table->num_entries == ts_table->max_entries)
		return;

	tse = &ts_table->entries[ts_table->num_entries++];
	tse->entry_id = id;
	tse->entry_stamp = tsc_to_uint64(ts_time) - ts_table->base_time;
}

void timestamp_add_now(enum timestamp_id id)
{
	timestamp_add(id, rdtsc());
}

#define MAX_TIMESTAMP_CACHE 8
struct timestamp_cache {
	enum timestamp_id id;
	tsc_t time;
} timestamp_cache[MAX_TIMESTAMP_CACHE] CAR_GLOBAL;

static int timestamp_entries CAR_GLOBAL = 0;

/**
 * timestamp_stash() allows to temporarily cache time stamps.
 * This is needed when time stamping before the CBMEM area
 * is initialized. The function timestamp_sync() is used to
 * write the time stamps to the CBMEM area. This is done in
 * ram stage main()
 */

static void timestamp_stash(enum timestamp_id id, tsc_t ts_time)
{
	if (timestamp_entries >= MAX_TIMESTAMP_CACHE) {
		printk(BIOS_ERR, "ERROR: failed to add timestamp to cache\n");
		return;
	}
	timestamp_cache[timestamp_entries].id = id;
	timestamp_cache[timestamp_entries].time = ts_time;
	timestamp_entries++;
}

static void timestamp_do_sync(void)
{
	int i;
	for (i = 0; i < timestamp_entries; i++)
		timestamp_add(timestamp_cache[i].id, timestamp_cache[i].time);
	timestamp_entries = 0;
}

void timestamp_init(tsc_t base)
{
#ifndef __PRE_RAM__
	struct timestamp_table* tst;

	/* Locate and use an already existing table. */
	tst = cbmem_find(CBMEM_ID_TIMESTAMP);
	if (tst) {
		ts_table = tst;
		return;
	}
#endif

	timestamp_real_init(base);
	if (ts_table)
		timestamp_do_sync();
	else
		ts_basetime = base;
}

#ifndef __PRE_RAM__
void timestamp_sync(void)
{
	if (!ts_table)
		timestamp_init(ts_basetime);
}
#endif
