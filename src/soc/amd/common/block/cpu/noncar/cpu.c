/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <soc/iomap.h>

int get_cpu_count(void)
{
	return 1 + (cpuid_ecx(0x80000008) & 0xff);
}

void set_cstate_io_addr(void)
{
	msr_t cst_addr;

	cst_addr.hi = 0;
	cst_addr.lo = ACPI_CPU_CONTROL;
	wrmsr(MSR_CSTATE_ADDRESS, cst_addr);
}
