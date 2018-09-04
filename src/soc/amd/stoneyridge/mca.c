/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/x86/msr.h>
#include <arch/acpi.h>
#include <cpu/amd/amdfam15.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <console/console.h>

static const char *const mca_bank_name[] = {
	"Load-store unit",
	"Instruction fetch unit",
	"Combined unit",
	"Reserved",
	"Northbridge",
	"Execution unit",
	"Floating point unit"
};

void check_mca(void)
{
	int i;
	msr_t msr;
	int num_banks;

	msr = rdmsr(MCG_CAP);
	num_banks = msr.lo & MCA_BANKS_MASK;

	if (is_warm_reset()) {
		for (i = 0 ; i < num_banks ; i++) {
			if (i == 3) /* Reserved in Family 15h */
				continue;

			msr = rdmsr(MC0_STATUS + (i * 4));
			if (msr.hi || msr.lo) {
				int core = cpuid_ebx(1) >> 24;

				printk(BIOS_WARNING, "#MC Error: core %d, bank %d %s\n",
						core, i, mca_bank_name[i]);

				printk(BIOS_WARNING, "   MC%d_STATUS =   %08x_%08x\n",
						i, msr.hi, msr.lo);
				msr = rdmsr(MC0_ADDR + (i * 4));
				printk(BIOS_WARNING, "   MC%d_ADDR =     %08x_%08x\n",
						i, msr.hi, msr.lo);
				msr = rdmsr(MC0_MISC + (i * 4));
				printk(BIOS_WARNING, "   MC%d_MISC =     %08x_%08x\n",
						i, msr.hi, msr.lo);
				msr = rdmsr(MC0_CTL + (i * 4));
				printk(BIOS_WARNING, "   MC%d_CTL =      %08x_%08x\n",
						i, msr.hi, msr.lo);
				msr = rdmsr(MC0_CTL_MASK + i);
				printk(BIOS_WARNING, "   MC%d_CTL_MASK = %08x_%08x\n",
						i, msr.hi, msr.lo);
			}
		}
	}

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0 ; i < num_banks ; i++)
		wrmsr(MC0_STATUS + (i * 4), msr);
}
