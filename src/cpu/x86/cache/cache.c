/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <program_loading.h>
#include <types.h>

bool clflush_supported(void)
{
	return (cpuid_edx(1) >> CPUID_FEATURE_CLFLUSH_BIT) & 1;
}

void clflush_region(const uintptr_t start, const size_t size)
{
	uintptr_t addr;
	const size_t cl_size = ((cpuid_ebx(1) >> 8) & 0xff) * 8;

	printk(BIOS_SPEW, "CLFLUSH [0x%lx, 0x%lx]\n", start, start + size);

	for (addr = ALIGN_DOWN(start, cl_size); addr < start + size; addr += cl_size)
		clflush((void *)addr);
}

/*
 * For each segment of a program loaded this function is called
 * to invalidate caches for the addresses of the loaded segment
 */
void arch_segment_loaded(uintptr_t start, size_t size, int flags)
{
	/* INVD is only called in postcar stage so we only need
	   to make sure that our code hits dram during romstage. */
	if (!ENV_CACHE_AS_RAM)
		return;
	if (!ENV_RAMINIT)
		return;
	if (!CONFIG(POSTCAR_STAGE))
		return;
	if (!CONFIG(X86_CLFLUSH_CAR))
		return;
	if (flags != SEG_FINAL)
		return;

	/*
	 * The assumption is made here that DRAM is only ready after cbmem
	 *  is initialized, to avoid flushing when loading earlier things (e.g. FSP, ...)
	 */
	if (!cbmem_online())
		return;

	if (!self_snooping_supported()) {
		if (clflush_supported())
			clflush_region(start, size);
		else
			printk(BIOS_DEBUG, "Not flushing cache to RAM, CLFLUSH not supported\n");
	}
}
