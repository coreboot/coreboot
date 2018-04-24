/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Advanced Micro Devices, Inc.
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

#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/amdfam15.h>
#include <bootstate.h>
#include <timer.h>
#include <console/console.h>

static void per_core_finalize(void *unused)
{
	msr_t hwcr, mask;

	/* Finalize SMM settings */
	hwcr = rdmsr(HWCR_MSR);
	if (hwcr.lo & SMM_LOCK) /* Skip if already locked, avoid GPF */
		return;

	if (IS_ENABLED(CONFIG_SMM_TSEG)) {
		mask = rdmsr(MSR_SMM_MASK);
		mask.lo |= SMM_TSEG_VALID;
		wrmsr(MSR_SMM_MASK, mask);
	}

	hwcr.lo |= SMM_LOCK;
	wrmsr(HWCR_MSR, hwcr);
}

static void finalize_cores(void)
{
	int r;
	printk(BIOS_SPEW, "Lock SMM configuration\n");

	r = mp_run_on_all_cpus(per_core_finalize, NULL, 10 * USECS_PER_MSEC);
	if (r)
		printk(BIOS_WARNING, "Failed to finalize all cores\n");
}

static void soc_finalize(void *unused)
{
	finalize_cores();

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
