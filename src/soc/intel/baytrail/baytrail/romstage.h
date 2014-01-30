/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BAYTRAIL_ROMSTAGE_H_
#define _BAYTRAIL_ROMSTAGE_H_

#if !defined(__PRE_RAM__)
#error "Don't include romstage.h from a ramstage compilation unit!"
#endif

#include <stdint.h>
#include <arch/cpu.h>
#include <baytrail/mrc_wrapper.h>

#define NUM_ROMSTAGE_TS 4
struct romstage_timestamps {
	uint64_t times[NUM_ROMSTAGE_TS];
	int count;
};

struct romstage_params {
	struct romstage_timestamps ts;
	unsigned long bist;
	struct mrc_params *mrc_params;
};

void mainboard_romstage_entry(struct romstage_params *params);
void romstage_common(struct romstage_params *params);
void * asmlinkage romstage_main(unsigned long bist, uint32_t tsc_lo,
                                uint32_t tsc_high);
void asmlinkage romstage_after_car(void);
void raminit(struct mrc_params *mp, int prev_sleep_state);
void gfx_init(void);

#if CONFIG_ENABLE_BUILTIN_COM1
void byt_config_com1_and_enable(void);
#else
static inline void byt_config_com1_and_enable(void) { }
#endif

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
