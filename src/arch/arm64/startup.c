/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <arch/cache.h>
#include <arch/lib_helpers.h>
#include <arch/startup.h>
#include <console/console.h>

/* This space is defined in stage_entry.S. */
extern u8 _arm64_startup_data[];

static inline void save_element(size_t index, uint64_t val)
{
	uint64_t *ptr = (uint64_t *)_arm64_startup_data;

	ptr[index] = val;
}

/*
 * startup_save_cpu_data is used to save register values that need to be setup
 * when a CPU starts booting. This is used by secondary CPUs as well as resume
 * path to directly setup MMU and other related registers.
 */
void startup_save_cpu_data(void)
{
	save_element(MAIR_INDEX, raw_read_mair_current());
	save_element(TCR_INDEX, raw_read_tcr_current());
	save_element(TTBR0_INDEX, raw_read_ttbr0_current());
	save_element(VBAR_INDEX, raw_read_vbar_current());
	save_element(CNTFRQ_INDEX, raw_read_cntfrq_el0());
	save_element(CPACR_INDEX, raw_read_cpacr_el1());

	if (get_current_el() == EL3) {
		save_element(SCR_INDEX, raw_read_scr_el3());
		save_element(CPTR_INDEX, raw_read_cptr_el3());
	}

	dcache_clean_by_mva(_arm64_startup_data,
			    NUM_ELEMENTS * PER_ELEMENT_SIZE_BYTES);
}
