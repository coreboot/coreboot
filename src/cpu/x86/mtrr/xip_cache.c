/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <program_loading.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>

/* For now this is a good lowest common denominator for the total CPU cache.
   TODO: fetch the total amount of cache from CPUID leaf2. */
#define MAX_CPU_CACHE (256 * KiB)

/* This makes the 'worst' case assumption that all cachelines covered by
   the MTRR, no matter the caching type, are filled and not overlapping. */
static uint32_t max_cache_used(void)
{
	int i, total_mtrrs = get_var_mtrr_count();
	uint32_t total_cache = 0;

	for (i = 0; i < total_mtrrs; i++) {
		msr_t mtrr = rdmsr(MTRR_PHYS_MASK(i));
		if (!(mtrr.lo & MTRR_PHYS_MASK_VALID))
			continue;
		total_cache += ~(mtrr.lo & 0xfffff000) + 1;
	}
	return total_cache;
}

void platform_prog_run(struct prog *prog)
{
	const uint32_t base = (uintptr_t)prog_start(prog);
	const uint32_t size = prog_size(prog);
	const uint32_t end = base + size;
	const uint32_t cache_used = max_cache_used();
	/* This will accumulate MTRR's as XIP stages are run.
	   For now this includes bootblock which sets ups its own
	   caching elsewhere, verstage and romstage */
	int mtrr_num = get_free_var_mtrr();
	uint32_t mtrr_base;
	uint32_t mtrr_size = 4 * KiB;
	struct cpuinfo_x86 cpu_info;

	get_fms(&cpu_info, cpuid_eax(1));
	/*
	 * An unidentified combination of speculative reads and branch
	 * predictions inside WRPROT-cacheable memory can cause invalidation
	 * of cachelines and loss of stack on models based on NetBurst
	 * microarchitecture. Therefore disable WRPROT region entirely for
	 * all family F models.
	 */
	if (cpu_info.x86 == 0xf) {
		printk(BIOS_NOTICE,
		       "PROG_RUN: CPU does not support caching ROM\n"
		       "The next stage will run slowly!\n");
		return;
	}

	if (mtrr_num == -1) {
		printk(BIOS_NOTICE,
		       "PROG_RUN: No MTRR available to cache ROM!\n"
		       "The next stage will run slowly!\n");
		return;
	}

	if (cache_used + mtrr_size > MAX_CPU_CACHE) {
		printk(BIOS_NOTICE,
		       "PROG_RUN: No more cache available for the next stage\n"
		       "The next stage will run slowly!\n");
		return;
	}

	while (1) {
		if (ALIGN_DOWN(base, mtrr_size) + mtrr_size >= end)
			break;
		if (cache_used + mtrr_size * 2 > MAX_CPU_CACHE)
			break;
		mtrr_size *= 2;
	}

	mtrr_base = ALIGN_DOWN(base, mtrr_size);
	if (mtrr_base + mtrr_size < end) {
		printk(BIOS_NOTICE, "PROG_RUN: Limiting XIP cache to %uKiB!\n",
		       mtrr_size / KiB);
		/* Check if we can cover a bigger range by aligning up. */
		const uint32_t alt_base = ALIGN_UP(base, mtrr_size);
		const uint32_t lower_coverage = mtrr_base + mtrr_size - base;
		const uint32_t upper_coverage = MIN(alt_base + mtrr_size, end) - alt_base;
		if (upper_coverage > lower_coverage)
			mtrr_base = alt_base;
	}

	printk(BIOS_DEBUG,
	       "PROG_RUN: Setting MTRR to cache XIP stage. base: 0x%08x, size: 0x%08x\n",
	       mtrr_base, mtrr_size);

	set_var_mtrr(mtrr_num, mtrr_base, mtrr_size, MTRR_TYPE_WRPROT);
}
