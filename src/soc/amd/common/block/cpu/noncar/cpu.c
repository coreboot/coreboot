/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/amd/cpuid.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <smbios.h>
#include <soc/iomap.h>

int get_cpu_count(void)
{
	return 1 + (cpuid_ecx(0x80000008) & 0xff);
}

unsigned int smbios_processor_family(struct cpuid_result res)
{
	return 0x6b; /* Zen */
}

unsigned int get_threads_per_core(void)
{
	return 1 + ((cpuid_ebx(CPUID_EBX_CORE_ID) & CPUID_EBX_THREADS_MASK)
		    >> CPUID_EBX_THREADS_SHIFT);
}

void set_cstate_io_addr(void)
{
	msr_t cst_addr;

	cst_addr.hi = 0;
	cst_addr.lo = ACPI_CPU_CONTROL;
	wrmsr(MSR_CSTATE_ADDRESS, cst_addr);
}
