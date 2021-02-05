/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <stdint.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/msr.h>
#include <arch/bert_storage.h>
#include <memrange.h>
#include <fsp/util.h>
#include <FspGuids.h>

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

void bert_reserved_region(void **start, size_t *size)
{
	struct range_entry bert;
	int status;

	*start = NULL;
	*size = 0;

	status = fsp_find_range_hob(&bert, AMD_FSP_BERT_HOB_GUID.b);

	if (status < 0) {
		printk(BIOS_ERR, "Error: unable to find BERT HOB\n");
		return;
	}

	*start = (void *)(uintptr_t)range_entry_base(&bert);
	*size = range_entry_size(&bert);
}
