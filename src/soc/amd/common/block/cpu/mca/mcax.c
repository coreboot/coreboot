/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/msr_zen.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <types.h>
#include "mca_common_defs.h"

bool mca_skip_check(void)
{
	/* On Zen-based CPUs/APUs the MCA(X) status register have a defined state even in the
	   cold boot path, so no need to skip the check */
	return false;
}

void mca_print_error(unsigned int bank)
{
	msr_t msr;

	printk(BIOS_WARNING, "#MC Error: core %u, bank %u %s\n", initial_lapicid(), bank,
		mca_get_bank_name(bank));

	msr = rdmsr(MCAX_STATUS_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_STATUS =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_ADDR_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_ADDR =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_MISC0_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_MISC =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCAX_CTL_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_CTL =      %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MCA_CTL_MASK_MSR(bank));
	printk(BIOS_WARNING, "   MC%u_CTL_MASK = %08x_%08x\n", bank, msr.hi, msr.lo);
}
