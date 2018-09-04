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

struct mca_bank {
	msr_t ctl;
	msr_t sts;
	msr_t addr;
	msr_t misc;
	msr_t cmask;
};

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
	msr_t cap;
	struct mca_bank mci;
	int num_banks;

	cap = rdmsr(MCG_CAP);
	num_banks = cap.lo & MCA_BANKS_MASK;

	if (is_warm_reset()) {
		for (i = 0 ; i < num_banks ; i++) {
			if (i == 3) /* Reserved in Family 15h */
				continue;

			mci.sts = rdmsr(MC0_STATUS + (i * 4));
			if (mci.sts.hi || mci.sts.lo) {
				int core = cpuid_ebx(1) >> 24;

				printk(BIOS_WARNING, "#MC Error: core %d, bank %d %s\n",
						core, i, mca_bank_name[i]);

				printk(BIOS_WARNING, "   MC%d_STATUS =   %08x_%08x\n",
						i, mci.sts.hi, mci.sts.lo);
				mci.addr = rdmsr(MC0_ADDR + (i * 4));
				printk(BIOS_WARNING, "   MC%d_ADDR =     %08x_%08x\n",
						i, mci.addr.hi, mci.addr.lo);
				mci.misc = rdmsr(MC0_MISC + (i * 4));
				printk(BIOS_WARNING, "   MC%d_MISC =     %08x_%08x\n",
						i, mci.misc.hi, mci.misc.lo);
				mci.ctl = rdmsr(MC0_CTL + (i * 4));
				printk(BIOS_WARNING, "   MC%d_CTL =      %08x_%08x\n",
						i, mci.ctl.hi, mci.ctl.lo);
				mci.cmask = rdmsr(MC0_CTL_MASK + i);
				printk(BIOS_WARNING, "   MC%d_CTL_MASK = %08x_%08x\n",
						i, mci.cmask.hi, mci.cmask.lo);
			}
		}
	}

	/* zero the machine check error status registers */
	mci.sts.lo = 0;
	mci.sts.hi = 0;
	for (i = 0 ; i < num_banks ; i++)
		wrmsr(MC0_STATUS + (i * 4), mci.sts);
}
