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
 * Foundation, Inc.
 */

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>

struct timestamp_entry {
	uint32_t	entry_id;
	uint64_t	entry_stamp;
} __attribute__((packed));

struct timestamp_table {
	uint64_t	base_time;
	uint16_t	max_entries;
	uint16_t	tick_freq_mhz;
	uint32_t	num_entries;
	struct timestamp_entry entries[0]; /* Variable number of entries */
} __attribute__((packed));

enum timestamp_id {
	TS_START_ROMSTAGE = 1,
	TS_BEFORE_INITRAM = 2,
	TS_AFTER_INITRAM = 3,
	TS_END_ROMSTAGE = 4,
	TS_START_VBOOT = 5,
	TS_END_VBOOT = 6,
	TS_START_COPYRAM = 8,
	TS_END_COPYRAM = 9,
	TS_START_RAMSTAGE = 10,
	TS_START_BOOTBLOCK = 11,
	TS_END_BOOTBLOCK = 12,
	TS_START_COPYROM = 13,
	TS_END_COPYROM = 14,
	TS_START_ULZMA = 15,
	TS_END_ULZMA = 16,
	TS_DEVICE_ENUMERATE = 30,
	TS_DEVICE_CONFIGURE = 40,
	TS_DEVICE_ENABLE = 50,
	TS_DEVICE_INITIALIZE = 60,
	TS_DEVICE_DONE = 70,
	TS_CBMEM_POST = 75,
	TS_WRITE_TABLES = 80,
	TS_LOAD_PAYLOAD = 90,
	TS_ACPI_WAKE_JUMP = 98,
	TS_SELFBOOT_JUMP = 99,

	/* 500+ reserved for vendorcode extensions (500-600: google/chromeos) */
	TS_START_COPYVER = 501,
	TS_END_COPYVER = 502,
	TS_START_TPMINIT = 503,
	TS_END_TPMINIT = 504,
	TS_START_VERIFY_SLOT = 505,
	TS_END_VERIFY_SLOT = 506,
	TS_START_HASH_BODY = 507,
	TS_DONE_LOADING = 508,
	TS_DONE_HASHING = 509,
	TS_END_HASH_BODY = 510,

	/* 950+ reserved for vendorcode extensions (950-999: intel/fsp) */
	TS_FSP_MEMORY_INIT_START = 950,
	TS_FSP_MEMORY_INIT_END = 951,
	TS_FSP_TEMP_RAM_EXIT_START = 952,
	TS_FSP_TEMP_RAM_EXIT_END = 953,
	TS_FSP_SILICON_INIT_START = 954,
	TS_FSP_SILICON_INIT_END = 955,
	TS_FSP_BEFORE_ENUMERATE = 956,
	TS_FSP_AFTER_ENUMERATE = 957,
	TS_FSP_BEFORE_FINALIZE = 958,
	TS_FSP_AFTER_FINALIZE = 959,

	/* 1000+ reserved for payloads (1000-1200: ChromeOS depthcharge) */
};

#if CONFIG_COLLECT_TIMESTAMPS && (CONFIG_EARLY_CBMEM_INIT || !defined(__PRE_RAM__))
/*
 * timestamp_init() needs to be called once for each of these cases:
 *    1. __PRE_RAM__ (bootblock, romstage, verstage, etc) and
 *    2. !__PRE_RAM__ (ramstage)
 * The latter is taken care of by the generic coreboot infrastructure so
 * it's up to the chipset/arch to call timestamp_init() in *one* of
 * the __PRE_RAM__ stages. If multiple calls are made timestamps will be lost.
 */
void timestamp_init(uint64_t base);
/*
 * Add a new timestamp. Depending on cbmem is available or not, this timestamp
 * will be stored to cbmem / timestamp cache.
 */
void timestamp_add(enum timestamp_id id, uint64_t ts_time);
/* Calls timestamp_add with current timestamp. */
void timestamp_add_now(enum timestamp_id id);
#else
#define timestamp_init(base)
#define timestamp_add(id, time)
#define timestamp_add_now(id)
#endif

/* Implemented by the architecture code */
uint64_t timestamp_get(void);
uint64_t get_initial_timestamp(void);
/* Returns timestamp tick frequency in MHz. */
int timestamp_tick_freq_mhz(void);

#endif
