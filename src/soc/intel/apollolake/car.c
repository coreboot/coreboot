/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <arch/cpu.h>
#include <program_loading.h>
#include <soc/cpu.h>

/*
 * This file supports the necessary hoops one needs to jump through since
 * early FSP component and early stages are running from cache-as-ram.
 */

static void flush_l1d_to_l2(void)
{
	msr_t msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= (1 << 8);
	wrmsr(MSR_POWER_MISC, msr);
}

void platform_segment_loaded(uintptr_t start, size_t size, int flags)
{
	/* TODO: filter on address to see if L1D flushing required. */

	/* Flush L1D cache to L2 on final segment loaded */
	if (flags & SEG_FINAL)
		flush_l1d_to_l2();
}
