/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/biosram.h>
#include <console/console.h>
#include <device/device.h>
#include <arch/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>

/* These will likely move to some device node or cbmem. */
static uint64_t amd_topmem = 0;
static uint64_t amd_topmem2 = 0;

uint64_t bsp_topmem(void)
{
	return amd_topmem;
}

uint64_t bsp_topmem2(void)
{
	return amd_topmem2;
}

/* Take a copy of BSP CPUs TOP_MEM and TOP_MEM2 registers,
 * so they can be distributed to AP CPUs. Not strictly MTRRs,
 * but this is not that bad a place to have this code.
 */
void setup_bsp_ramtop(void)
{
	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk(BIOS_INFO,
	    "%s, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk(BIOS_INFO,
	    "%s, TOP MEM2: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr2.lo, msr2.hi);

	amd_topmem = (uint64_t) msr.hi << 32 | msr.lo;
	amd_topmem2 = (uint64_t) msr2.hi << 32 | msr2.lo;
}

void add_uma_resource_below_tolm(struct device *nb, int idx)
{
	uint32_t topmem = bsp_topmem();
	uint32_t top_of_cacheable = restore_top_of_low_cacheable();

	if (top_of_cacheable == topmem)
		return;

	uint32_t uma_base = top_of_cacheable;
	uint32_t uma_size = topmem - top_of_cacheable;

	printk(BIOS_INFO, "%s: uma size 0x%08x, memory start 0x%08x\n",
			__func__, uma_size, uma_base);

	uma_resource(nb, idx, uma_base / KiB, uma_size / KiB);
}
