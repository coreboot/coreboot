/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <types.h>
#include "mca_common_defs.h"

bool mca_skip_check(void)
{
	return !is_warm_reset();
}

void mca_print_error(unsigned int bank)
{
	msr_t msr;

	printk(BIOS_WARNING, "#MC Error: core %u, bank %u %s\n", initial_lapicid(), bank,
		mca_get_bank_name(bank));

	msr = rdmsr(IA32_MC_STATUS(bank));
	printk(BIOS_WARNING, "   MC%u_STATUS =   %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_ADDR(bank));
	printk(BIOS_WARNING, "   MC%u_ADDR =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_MISC(bank));
	printk(BIOS_WARNING, "   MC%u_MISC =     %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(IA32_MC_CTL(bank));
	printk(BIOS_WARNING, "   MC%u_CTL =      %08x_%08x\n", bank, msr.hi, msr.lo);
	msr = rdmsr(MC_CTL_MASK(bank));
	printk(BIOS_WARNING, "   MC%u_CTL_MASK = %08x_%08x\n", bank, msr.hi, msr.lo);
}
