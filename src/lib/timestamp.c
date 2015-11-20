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
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <console/console.h>
#include <cbmem.h>
#include <symbols.h>
#include <timer.h>
#include <timestamp.h>
#include <arch/early_variables.h>
#include <rules.h>
#include <smp/node.h>

#define MAX_TIMESTAMPS 60

#define MAX_TIMESTAMP_CACHE 16

struct __attribute__((__packed__)) timestamp_cache {
	uint32_t cache_state;
	struct timestamp_table table;
	/* The struct timestamp_table has a 0 length array as its last field.
	 * The  following 'entries' array serves as the storage space for the
	 * cache. */
	struct timestamp_entry entries[MAX_TIMESTAMP_CACHE];
};

#if (IS_ENABLED(CONFIG_HAS_PRECBMEM_TIMESTAMP_REGION) && defined(__PRE_RAM__))
#define USE_TIMESTAMP_REGION 1
#else
#define USE_TIMESTAMP_REGION 0
#endif

/* The cache location will sit in BSS when in ramstage. */
#define TIMESTAMP_CACHE_IN_BSS ENV_RAMSTAGE

#define HAS_CBMEM (ENV_ROMSTAGE || ENV_RAMSTAGE)

/* Storage of cache entries during ramstage prior to cbmem coming online. */
static struct timestamp_cache timestamp_cache;

enum {
	TIMESTAMP_CACHE_UNINITIALIZED = 0,
	TIMESTAMP_CACHE_INITIALIZED,
	TIMESTAMP_CACHE_NOT_NEEDED,
};

static void timestamp_cache_init(struct timestamp_cache *ts_cache,
				 uint64_t base)
{
	ts_cache->table.num_entries = 0;
	ts_cache->table.max_entries = MAX_TIMESTAMP_CACHE;
	ts_cache->table.base_time = base;
	ts_cache->cache_state = TIMESTAMP_CACHE_INITIALIZED;
}

static struct timestamp_cache *timestamp_cache_get(void)
{
	struct timestamp_cache *ts_cache = NULL;

	if (TIMESTAMP_CACHE_IN_BSS) {
		ts_cache = &timestamp_cache;
	} else if (USE_TIMESTAMP_REGION) {
		if (_timestamp_size < sizeof(*ts_cache))
			BUG();
		ts_cache = car_get_var_ptr((void *)_timestamp);
	}

	if (ts_cache && ts_cache->cache_state == TIMESTAMP_CACHE_UNINITIALIZED)
		timestamp_cache_init(ts_cache, 0);

	return ts_cache;
}

static struct timestamp_table *timestamp_alloc_cbmem_table(void)
{
	struct timestamp_table* tst;

	tst = cbmem_add(CBMEM_ID_TIMESTAMP,
			sizeof(struct timestamp_table) +
			MAX_TIMESTAMPS * sizeof(struct timestamp_entry));

	if (!tst)
		return NULL;

	tst->base_time = 0;
	tst->max_entries = MAX_TIMESTAMPS;
	tst->num_entries = 0;

	return tst;
}

/* Determine if one should proceed into timestamp code. This is for protecting
 * systems that have multiple processors running in romstage -- namely AMD
 * based x86 platforms. */
static int timestamp_should_run(void)
{
	/* Only check boot_cpu() in other stages than ramstage on x86. */
	if ((!ENV_RAMSTAGE && IS_ENABLED(CONFIG_ARCH_X86)) && !boot_cpu())
		return 0;

	return 1;
}

static struct timestamp_table *timestamp_table_get(void)
{
	MAYBE_STATIC struct timestamp_table *ts_table = NULL;
	struct timestamp_cache *ts_cache;

	if (!timestamp_should_run())
		return NULL;

	if (ts_table != NULL)
		return ts_table;

	ts_cache = timestamp_cache_get();

	if (ts_cache == NULL) {
		if (HAS_CBMEM)
			ts_table = cbmem_find(CBMEM_ID_TIMESTAMP);
		return ts_table;
	}

	/* Cache is required. */
	if (ts_cache->cache_state != TIMESTAMP_CACHE_NOT_NEEDED)
		return &ts_cache->table;

	/* Cache shouldn't be used but there's no backing store. */
	if (!HAS_CBMEM)
		return NULL;

	ts_table = cbmem_find(CBMEM_ID_TIMESTAMP);

	return ts_table;
}

static void timestamp_add_table_entry(struct timestamp_table *ts_table,
				      enum timestamp_id id, uint64_t ts_time)
{
	struct timestamp_entry *tse;

	if (ts_table->num_entries >= ts_table->max_entries)
		return;

	tse = &ts_table->entries[ts_table->num_entries++];
	tse->entry_id = id;
	tse->entry_stamp = ts_time - ts_table->base_time;

	if (ts_table->num_entries == ts_table->max_entries)
		printk(BIOS_ERR, "ERROR: Timestamp table full\n");
}

void timestamp_add(enum timestamp_id id, uint64_t ts_time)
{
	struct timestamp_table *ts_table;

	ts_table = timestamp_table_get();

	if (!ts_table) {
		printk(BIOS_ERR, "ERROR: No timestamp table found\n");
		return;
	}

	timestamp_add_table_entry(ts_table, id, ts_time);
}

void timestamp_add_now(enum timestamp_id id)
{
	timestamp_add(id, timestamp_get());
}

void timestamp_init(uint64_t base)
{
	struct timestamp_cache *ts_cache;

	if (!timestamp_should_run())
		return;

	ts_cache = timestamp_cache_get();

	if (!ts_cache) {
		printk(BIOS_ERR, "ERROR: No timestamp cache to init\n");
		return;
	}

	/* In the EARLY_CBMEM_INIT case timestamps could have already been
	 * recovered. In those circumstances honor the cache which sits in BSS
	 * as it has already been initialized. */
	if (ENV_RAMSTAGE && IS_ENABLED(CONFIG_EARLY_CBMEM_INIT) &&
	    ts_cache->cache_state != TIMESTAMP_CACHE_UNINITIALIZED)
		return;

	timestamp_cache_init(ts_cache, base);
}

static void timestamp_sync_cache_to_cbmem(int is_recovery)
{
	uint32_t i;
	struct timestamp_cache *ts_cache;
	struct timestamp_table *ts_cache_table;
	struct timestamp_table *ts_cbmem_table = NULL;

	if (!timestamp_should_run())
		return;

	ts_cache = timestamp_cache_get();

	/* No timestamp cache found */
	if (ts_cache == NULL) {
		printk(BIOS_ERR, "ERROR: No timestamp cache found\n");
		return;
	}

	ts_cache_table = &ts_cache->table;

	/* cbmem is being recovered. */
	if (is_recovery) {
		/* x86 resume path expects timestamps to be reset. */
		if (IS_ENABLED(CONFIG_ARCH_ROMSTAGE_X86_32) && ENV_ROMSTAGE)
			ts_cbmem_table = timestamp_alloc_cbmem_table();
		else {
			/* Find existing table in cbmem. */
			ts_cbmem_table = cbmem_find(CBMEM_ID_TIMESTAMP);
			/* No existing timestamp table. */
			if (ts_cbmem_table == NULL)
				ts_cbmem_table = timestamp_alloc_cbmem_table();
		}
	} else
		/* First time sync. Add new table. */
		ts_cbmem_table = timestamp_alloc_cbmem_table();

	if (ts_cbmem_table == NULL) {
		printk(BIOS_ERR, "ERROR: No timestamp table allocated\n");
		return;
	}

	/*
	 * There's no need to worry about the base_time fields being out of
	 * sync because the following configurations are used/supported:
	 *
	 * 1. CONFIG_HAS_PRECBMEM_TIMESTAMP_REGION is enabled. This
	 *    implies CONFIG_EARLY_CBMEM_INIT so once cbmem comes
	 *    online we sync the timestamps to the cbmem storage while
	 *    running in romstage. In ramstage the cbmem area is
	 *    recovered and utilized.
	 *
	 * 2. CONFIG_LATE_CBMEM_INIT (!CONFIG_EARLY_CBMEM_INIT) is
	 *    being used. That means the only cache that exists is
	 *    in ramstage. Once cbmem comes online in ramstage those
	 *    values are sync'd over.
	 *
	 * Any other combinations will result in inconsistent base_time
	 * values including bizarre timestamp values.
	 */
	for (i = 0; i < ts_cache_table->num_entries; i++) {
		struct timestamp_entry *tse = &ts_cache_table->entries[i];
		timestamp_add_table_entry(ts_cbmem_table, tse->entry_id,
					  tse->entry_stamp);
	}

	/* Freshly added cbmem table has base_time 0. Inherit cache base_time */
	if (ts_cbmem_table->base_time == 0)
		ts_cbmem_table->base_time = ts_cache_table->base_time;

	/* Seed the timestamp tick frequency in ramstage. */
	if (ENV_RAMSTAGE)
		ts_cbmem_table->tick_freq_mhz = timestamp_tick_freq_mhz();

	/* Cache no longer required. */
	ts_cache_table->num_entries = 0;
	ts_cache->cache_state = TIMESTAMP_CACHE_NOT_NEEDED;
}

ROMSTAGE_CBMEM_INIT_HOOK(timestamp_sync_cache_to_cbmem)
RAMSTAGE_CBMEM_INIT_HOOK(timestamp_sync_cache_to_cbmem)

/* Provide default timestamp implementation using monotonic timer. */
uint64_t  __attribute__((weak)) timestamp_get(void)
{
	struct mono_time t1, t2;

	if (!IS_ENABLED(CONFIG_HAVE_MONOTONIC_TIMER))
		return 0;

	mono_time_set_usecs(&t1, 0);
	timer_monotonic_get(&t2);

	return mono_time_diff_microseconds(&t1, &t2);
}

/* Like timestamp_get() above this matches up with microsecond granularity. */
int __attribute__((weak)) timestamp_tick_freq_mhz(void)
{
	return 1;
}
