/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/memmap.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/smm.h>
#include <fsp/util.h>
#include <FspGuids.h>
#include <memrange.h>
#include <stdint.h>

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

/*
 * For data stored in TSEG, ensure TValid is clear so R/W access can reach
 * the DRAM when not in SMM.
 */
static void clear_tvalid(void)
{
	msr_t hwcr = rdmsr(HWCR_MSR);
	msr_t mask = rdmsr(SMM_MASK_MSR);
	int tvalid = !!(mask.lo & SMM_TSEG_VALID);

	if (hwcr.lo & SMM_LOCK) {
		if (!tvalid) /* not valid but locked means still accessible */
			return;

		printk(BIOS_ERR, "Error: can't clear TValid, already locked\n");
		return;
	}

	mask.lo &= ~SMM_TSEG_VALID;
	wrmsr(SMM_MASK_MSR, mask);
}

void smm_region(uintptr_t *start, size_t *size)
{
	static int once;
	struct range_entry tseg;
	int status;

	*start = 0;
	*size = 0;

	status = fsp_find_range_hob(&tseg, AMD_FSP_TSEG_HOB_GUID.b);

	if (status < 0) {
		printk(BIOS_ERR, "Error: unable to find TSEG HOB\n");
		return;
	}

	*start = (uintptr_t)range_entry_base(&tseg);
	*size = range_entry_size(&tseg);

	if (!once) {
		clear_tvalid();
		once = 1;
	}
}
