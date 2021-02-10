/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/smm.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>

/*
 * For data stored in TSEG, ensure TValid is clear so R/W access can reach
 * the DRAM when not in SMM.
 */
void clear_tvalid(void)
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
