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
#include <arch/early_variables.h>
#include <cpu/x86/lapic.h>

#define MAX_TIMESTAMPS 30

static struct timestamp_table* ts_table_p CAR_GLOBAL = NULL;
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

	car_set_var(ts_table_p, tst);
}

void timestamp_add(enum timestamp_id id, tsc_t ts_time)
{
	struct timestamp_entry *tse;
	struct timestamp_table *ts_table = NULL;

	if (!boot_cpu())
		return;

	ts_table = car_get_var(ts_table_p);
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
 * timestamp_stash() allows to temporarily cache timestamps.
 * This is needed when timestamping before the CBMEM area
 * is initialized. The function timestamp_do_sync() is used to
 * write the timestamps to the CBMEM area and this is done as
 * part of CAR migration for romstage, and in ramstage main().
 */

static void timestamp_stash(enum timestamp_id id, tsc_t ts_time)
{
	struct timestamp_cache *ts_cache = car_get_var(timestamp_cache);
	int ts_entries = car_get_var(timestamp_entries);

	if (ts_entries >= MAX_TIMESTAMP_CACHE) {
		printk(BIOS_ERR, "ERROR: failed to add timestamp to cache\n");
		return;
	}
	ts_cache[ts_entries].id = id;
	ts_cache[ts_entries].time = ts_time;
	car_set_var(timestamp_entries, ++ts_entries);
}

static void timestamp_do_sync(void)
{
	struct timestamp_cache *ts_cache = car_get_var(timestamp_cache);
	int ts_entries = car_get_var(timestamp_entries);

	int i;
	for (i = 0; i < ts_entries; i++)
		timestamp_add(ts_cache[i].id, ts_cache[i].time);
	car_set_var(timestamp_entries, 0);
}

void timestamp_init(tsc_t base)
{
	if (!boot_cpu())
		return;

#ifdef __PRE_RAM__
	/* Copy of basetime, it is too early for CBMEM. */
	car_set_var(ts_basetime, base);
#else
	struct timestamp_table* tst;

	/* Locate and use an already existing table. */
	tst = cbmem_find(CBMEM_ID_TIMESTAMP);
	if (tst) {
		car_set_var(ts_table_p, tst);
		return;
	}

	/* Copy of basetime, may be too early for CBMEM. */
	car_set_var(ts_basetime, base);
	timestamp_real_init(base);
#endif
}

void timestamp_reinit(void)
{
	if (!boot_cpu())
		return;

#ifdef __PRE_RAM__
	timestamp_real_init(car_get_var(ts_basetime));
#else
	if (!car_get_var(ts_table_p))
		timestamp_init(car_get_var(ts_basetime));
#endif
	if (car_get_var(ts_table_p))
		timestamp_do_sync();
}

/* Call timestamp_reinit at CAR migration time. */
CAR_MIGRATE(timestamp_reinit)
