/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cpu/x86/msr.h>
#include <timer.h>
#include <timestamp.h>

#define CU_PTSC_MSR	0xc0010280
#define PTSC_FREQ_MHZ	100

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timestamp_get());
}

uint64_t timestamp_get(void)
{
	unsigned long long val;
	msr_t msr;

	msr = rdmsr(CU_PTSC_MSR);

	val = ((unsigned long long)msr.hi << 32) | msr.lo;

	return val / PTSC_FREQ_MHZ;
}
