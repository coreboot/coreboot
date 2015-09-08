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

#include <commonlib/timestamp_serialized.h>

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
