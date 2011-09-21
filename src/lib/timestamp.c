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

#include <stdint.h>
#include <console/console.h>
#include <cbmem.h>
#include <timestamp.h>

#define MAX_TIMESTAMPS 30

#ifndef __PRE_RAM__
static struct timestamp_table* ts_table;
#endif

static uint64_t tsc_to_uint64(tsc_t tstamp)
{
	return (((uint64_t)tstamp.hi) << 32) + tstamp.lo;
}

void timestamp_init(tsc_t base)
{
	struct timestamp_table* tst;

	tst = cbmem_add(CBMEM_ID_TIMESTAMP,
			sizeof(struct timestamp_table) +
			MAX_TIMESTAMPS * sizeof(struct timestamp_entry));

	if (!tst) {
		printk(BIOS_ERR, "ERROR: failed to allocate timstamp table\n");
		return;
	}

	tst->base_time = tsc_to_uint64(base);
	tst->max_entries = MAX_TIMESTAMPS;
	tst->num_entries = 0;
}

void timestamp_add(enum timestamp_id id, tsc_t ts_time)
{
	struct timestamp_entry *tse;
#ifdef __PRE_RAM__
	struct timestamp_table *ts_table = cbmem_find(CBMEM_ID_TIMESTAMP);
#else
	if (!ts_table)
		ts_table = cbmem_find(CBMEM_ID_TIMESTAMP);
#endif
	if (!ts_table || (ts_table->num_entries == ts_table->max_entries))
		return;

	tse = &ts_table->entries[ts_table->num_entries++];
	tse->entry_id = id;
	tse->entry_stamp = tsc_to_uint64(ts_time) - ts_table->base_time;
}

void timestamp_add_now(enum timestamp_id id)
{
	timestamp_add(id, rdtsc());
}
