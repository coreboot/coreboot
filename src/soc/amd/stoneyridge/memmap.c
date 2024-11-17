/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/smm.h>
#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <soc/northbridge.h>
#include <soc/iomap.h>
#include <amdblocks/biosram.h>

uintptr_t cbmem_top_chipset(void)
{
	if (!get_top_of_mem_below_4gb())
		return 0;

	/* 8MB alignment to keep MTRR usage low */
	return ALIGN_DOWN(restore_top_of_low_cacheable() - CONFIG_SMM_TSEG_SIZE, 8 * MiB);
}

static uintptr_t smm_region_start(void)
{
	return cbmem_top();
}

static size_t smm_region_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void smm_region(uintptr_t *start, size_t *size)
{
	static int once;

	*start = smm_region_start();
	*size = smm_region_size();

	if (!once) {
		clear_tvalid();
		once = 1;
	}
}
