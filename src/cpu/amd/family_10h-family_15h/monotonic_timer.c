/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2013 Google, Inc.
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
#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <timer.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <northbridge/amd/amdht/AsPsDefs.h>
#include <cpu/amd/msr.h>

static struct monotonic_counter {
	int initialized;
	uint32_t core_frequency;
	struct mono_time time;
	uint64_t last_value;
} mono_counter;

static inline uint64_t read_counter_msr(void)
{
	msr_t counter_msr;

	counter_msr = rdmsr(TSC_MSR);

	return ((uint64_t)counter_msr.hi << 32) | (uint64_t)counter_msr.lo;
}

static void init_timer(void)
{
	uint8_t model;
	uint32_t cpuid_fms;
	uint8_t cpufid;
	uint8_t cpudid;
	uint8_t boost_capable = 0;

	/* Get CPU model */
	cpuid_fms = cpuid_eax(0x80000001);
	model = ((cpuid_fms & 0xf0000) >> 16) | ((cpuid_fms & 0xf0) >> 4);

	/* Get boost capability */
	if ((model == 0x8) || (model == 0x9)) {	/* revision D */
		boost_capable = (pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 4)), 0x15c) & 0x4) >> 2;
	}

	/* Set up TSC (BKDG v3.62 section 2.9.4)*/
	msr_t msr = rdmsr(HWCR_MSR);
	msr.lo |= 0x1000000;
	wrmsr(HWCR_MSR, msr);

	/* Get core Pstate 0 frequency in MHz */
	msr = rdmsr(0xC0010064 + boost_capable);
	cpufid = (msr.lo & 0x3f);
	cpudid = (msr.lo & 0x1c0) >> 6;
	mono_counter.core_frequency = (100 * (cpufid + 0x10)) / (0x01 << cpudid);

	mono_counter.last_value = read_counter_msr();
	mono_counter.initialized = 1;
}

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t current_tick;
	uint32_t usecs_elapsed = 0;

	if (!mono_counter.initialized)
		init_timer();

	current_tick = read_counter_msr();
	if (mono_counter.core_frequency != 0)
		usecs_elapsed = (current_tick - mono_counter.last_value) / mono_counter.core_frequency;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter.time, usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
