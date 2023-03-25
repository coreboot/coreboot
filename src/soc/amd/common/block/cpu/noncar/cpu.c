/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <smbios.h>
#include <soc/iomap.h>

uint32_t get_pstate_0_reg(void)
{
	return 0;
}

uint32_t get_pstate_latency(void)
{
	return 0;
}

unsigned int smbios_processor_family(struct cpuid_result res)
{
	return 0x6b; /* Zen */
}

void set_cstate_io_addr(void)
{
	msr_t cst_addr;

	cst_addr.hi = 0;
	cst_addr.lo = ACPI_CSTATE_CONTROL;
	wrmsr(MSR_CSTATE_ADDRESS, cst_addr);
}
