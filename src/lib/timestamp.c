/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>
#include <console/console.h>
#include <cbmem.h>
#include <symbols.h>
#include <timer.h>
#include <timestamp.h>
#include <smp/node.h>

#define MAX_TIMESTAMPS 192

/* This points to the active timestamp_table and can change within a stage
   as CBMEM comes available. */
static struct timestamp_table *glob_ts_table;

static void timestamp_cache_init(struct timestamp_table *ts_cache,
				 uint64_t base)
{
	ts_cache->num_entries = 0;
	ts_cache->base_time = base;
	ts_cache->max_entries = (REGION_SIZE(timestamp) -
		offsetof(struct timestamp_table, entries))
		/ sizeof(struct timestamp_entry);
}

static struct timestamp_table *timestamp_cache_get(void)
{
	struct timestamp_table *ts_cache = NULL;

	if (!ENV_ROMSTAGE_OR_BEFORE)
		return NULL;

	if (REGION_SIZE(timestamp) < sizeof(*ts_cache)) {
		BUG();
	} else {
		ts_cache = (void *)_timestamp;
	}

	return ts_cache;
}

static struct timestamp_table *timestamp_alloc_cbmem_table(void)
{
	struct timestamp_table *tst;

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
	/*
	 * Only check boot_cpu() in other stages than
	 * ENV_PAYLOAD_LOADER on x86.
	 */
	if ((!ENV_PAYLOAD_LOADER && ENV_X86) && !boot_cpu())
		return 0;

	return 1;
}

static struct timestamp_table *timestamp_table_get(void)
{
	if (glob_ts_table)
		return glob_ts_table;

	glob_ts_table = timestamp_cache_get();

	return glob_ts_table;
}

static void timestamp_table_set(struct timestamp_table *ts)
{
	glob_ts_table = ts;
}

static const char *timestamp_name(enum timestamp_id id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(timestamp_ids); i++) {
		if (timestamp_ids[i].id == id)
			return timestamp_ids[i].name;
	}

	return "Unknown timestamp ID";
}

static void timestamp_add_table_entry(struct timestamp_table *ts_table,
				      enum timestamp_id id, int64_t ts_time)
{
	struct timestamp_entry *tse;

	if (ts_table->num_entries >= ts_table->max_entries)
		return;

	tse = &ts_table->entries[ts_table->num_entries++];
	tse->entry_id = id;
	tse->entry_stamp = ts_time;

	if (ts_table->num_entries == ts_table->max_entries)
		printk(BIOS_ERR, "Timestamp table full\n");
}

void timestamp_add(enum timestamp_id id, int64_t ts_time)
{
	struct timestamp_table *ts_table;

	if (!timestamp_should_run())
		return;

	ts_table = timestamp_table_get();

	if (!ts_table) {
		printk(BIOS_ERR, "No timestamp table found\n");
		return;
	}

	ts_time -= ts_table->base_time;
	timestamp_add_table_entry(ts_table, id, ts_time);

	if (CONFIG(TIMESTAMPS_ON_CONSOLE))
		printk(BIOS_INFO, "Timestamp - %s: %lld\n", timestamp_name(id), ts_time);
}

void timestamp_add_now(enum timestamp_id id)
{
	timestamp_add(id, timestamp_get());
}

void timestamp_init(uint64_t base)
{
	struct timestamp_table *ts_cache;

	assert(ENV_ROMSTAGE_OR_BEFORE);

	if (!timestamp_should_run())
		return;

	ts_cache = timestamp_cache_get();

	if (!ts_cache) {
		printk(BIOS_ERR, "No timestamp cache to init\n");
		return;
	}

	timestamp_cache_init(ts_cache, base);
	timestamp_table_set(ts_cache);
}

static void timestamp_sync_cache_to_cbmem(struct timestamp_table *ts_cbmem_table)
{
	uint32_t i;
	struct timestamp_table *ts_cache_table;

	ts_cache_table = timestamp_table_get();
	if (!ts_cache_table) {
		printk(BIOS_ERR, "No timestamp cache found\n");
		return;
	}

	/*
	 * There's no need to worry about the base_time fields being out of
	 * sync because only the following configuration is used/supported:
	 *
	 *    Timestamps get initialized before ramstage, which implies
	 *    CBMEM initialization in romstage.
	 *    This requires the board to define a TIMESTAMP() region in its
	 *    memlayout.ld (default on x86). The base_time from timestamp_init()
	 *    (usually called from bootblock.c on most non-x86 boards) persists
	 *    in that region until it gets synced to CBMEM in romstage.
	 *    In ramstage, the BSS cache's base_time will be 0 until the second
	 *    sync, which will adjust the timestamps in there to the correct
	 *    base_time (from CBMEM) with the timestamp_add_table_entry() below.
	 *
	 * If you try to initialize timestamps before ramstage but don't define
	 * a TIMESTAMP region, all operations will fail (safely), and coreboot
	 * will behave as if timestamps collection was disabled.
	 */

	/* Inherit cache base_time. */
	ts_cbmem_table->base_time = ts_cache_table->base_time;

	for (i = 0; i < ts_cache_table->num_entries; i++) {
		struct timestamp_entry *tse = &ts_cache_table->entries[i];
		timestamp_add_table_entry(ts_cbmem_table, tse->entry_id,
					  tse->entry_stamp);
	}

	/* Cache no longer required. */
	ts_cache_table->num_entries = 0;
}

static void timestamp_reinit(int is_recovery)
{
	struct timestamp_table *ts_cbmem_table;

	if (!timestamp_should_run())
		return;

	/* First time into romstage we make a clean new table. For platforms that travel
	   through this path on resume, ARCH_X86 S3, timestamps are also reset. */
	if (ENV_CREATES_CBMEM) {
		ts_cbmem_table = timestamp_alloc_cbmem_table();
	} else {
		/* Find existing table in cbmem. */
		ts_cbmem_table = cbmem_find(CBMEM_ID_TIMESTAMP);
	}

	if (ts_cbmem_table == NULL) {
		printk(BIOS_ERR, "No timestamp table allocated\n");
		timestamp_table_set(NULL);
		return;
	}

	if (ENV_CREATES_CBMEM)
		timestamp_sync_cache_to_cbmem(ts_cbmem_table);

	/* Seed the timestamp tick frequency in ENV_PAYLOAD_LOADER. */
	if (ENV_PAYLOAD_LOADER)
		ts_cbmem_table->tick_freq_mhz = timestamp_tick_freq_mhz();

	timestamp_table_set(ts_cbmem_table);
}

void timestamp_rescale_table(uint16_t N, uint16_t M)
{
	uint32_t i;
	struct timestamp_table *ts_table;

	if (!timestamp_should_run())
		return;

	if (N == 0 || M == 0)
		return;

	ts_table = timestamp_table_get();

	/* No timestamp table found */
	if (ts_table == NULL) {
		printk(BIOS_ERR, "No timestamp table found\n");
		return;
	}

	ts_table->base_time /= M;
	ts_table->base_time *= N;
	for (i = 0; i < ts_table->num_entries; i++) {
		struct timestamp_entry *tse = &ts_table->entries[i];
		tse->entry_stamp /= M;
		tse->entry_stamp *= N;
	}
}

/*
 * Get the time in microseconds since boot (or more precise: since timestamp
 * table was initialized).
 */
uint32_t get_us_since_boot(void)
{
	struct timestamp_table *ts = timestamp_table_get();

	if (ts == NULL || ts->tick_freq_mhz == 0)
		return 0;
	return (timestamp_get() - ts->base_time) / ts->tick_freq_mhz;
}

CBMEM_READY_HOOK(timestamp_reinit);

/* Provide default timestamp implementation using monotonic timer. */
uint64_t  __weak timestamp_get(void)
{
	struct mono_time t1, t2;

	if (!CONFIG(HAVE_MONOTONIC_TIMER))
		return 0;

	mono_time_set_usecs(&t1, 0);
	timer_monotonic_get(&t2);

	return mono_time_diff_microseconds(&t1, &t2);
}

/* Like timestamp_get() above this matches up with microsecond granularity. */
int __weak timestamp_tick_freq_mhz(void)
{
	return 1;
}
