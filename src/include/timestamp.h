/*
 * This file is part of the coreboot project.
 *
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

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <commonlib/timestamp_serialized.h>

#if CONFIG(COLLECT_TIMESTAMPS)
/*
 * timestamp_init() needs to be called once in *one* of the ENV_ROMSTAGE_OR_BEFORE
 * stages (bootblock, romstage, verstage, etc). It's up to the chipset/arch
 * to make the call in the earliest stage, otherwise some timestamps will be lost.
 * For x86 ENV_ROMSTAGE call must be made before CAR is torn down.
 */
void timestamp_init(uint64_t base);
/*
 * Add a new timestamp. For ENV_ROMSTAGE_OR_BEFORE, this timestamp will be stored
 * inside REGION(timestamp) before cbmem comes online. For later stages, timestamps
 * added before cbmem_[recovery|initialize] calls will be lost.
 */
void timestamp_add(enum timestamp_id id, uint64_t ts_time);
/* Calls timestamp_add with current timestamp. */
void timestamp_add_now(enum timestamp_id id);

/* Apply a factor of N/M to all timestamps recorded so far. */
void timestamp_rescale_table(uint16_t N, uint16_t M);

/*
 * Get the time since boot scaled in microseconds. Therefore use the base time
 * of the timestamps to get the initial value which is subtracted from
 * current timestamp at call time. This will provide a more reliable value even
 * if the TSC is not reset on soft reset or warm start.
 */
uint32_t get_us_since_boot(void);

#else
#define timestamp_init(base)
#define timestamp_add(id, time)
#define timestamp_add_now(id)
#define timestamp_rescale_table(N, M)
#define get_us_since_boot() 0
#endif

/**
 * Workaround for guard combination above.
 */
#if CONFIG(COLLECT_TIMESTAMPS)
/* Implemented by the architecture code */
uint64_t timestamp_get(void);
#else
#define timestamp_get() 0
#endif

uint64_t get_initial_timestamp(void);
/* Returns timestamp tick frequency in MHz. */
int timestamp_tick_freq_mhz(void);

#endif
