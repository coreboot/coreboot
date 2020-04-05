/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/systemagent.h>

u32 cpu_family_model(void)
{
	return cpuid_eax(1) & 0x0fff0ff0;
}

u32 cpu_stepping(void)
{
	return cpuid_eax(1) & 0xf;
}

/* Dynamically determine if the part is ULT. */
int cpu_is_ult(void)
{
	static int ult = -1;

	if (ult < 0) {
		u32 fm = cpu_family_model();
		if (fm == BROADWELL_FAMILY_ULT || fm == HASWELL_FAMILY_ULT)
			ult = 1;
		else
			ult = 0;
	}

	return ult;
}
