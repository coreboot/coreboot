/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/memmap.h>
#include <amdblocks/smm.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <memrange.h>
#include <types.h>

void memmap_stash_early_dram_usage(void)
{
	struct memmap_early_dram *e;

	e = cbmem_add(CBMEM_ID_CB_EARLY_DRAM, sizeof(*e));

	if (!e)
		die("ERROR: Failed to stash early dram usage!\n");

	e->base = (uint32_t)(uintptr_t)_early_reserved_dram;
	e->size = REGION_SIZE(early_reserved_dram);
}

const struct memmap_early_dram *memmap_get_early_dram_usage(void)
{
	struct memmap_early_dram *e = cbmem_find(CBMEM_ID_CB_EARLY_DRAM);

	if (!e)
		die("ERROR: Failed to read early dram usage!\n");

	return e;
}

void smm_region(uintptr_t *start, size_t *size)
{
	static int once;

	if (CONFIG(PLATFORM_USES_FSP2_0)) {
		fsp_get_smm_region(start, size);
	} else {
		*start = (uintptr_t)cbmem_top();
		*size = CONFIG_SMM_TSEG_SIZE;
	}

	if (!once) {
		clear_tvalid();
		once = 1;
	}
}
