/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <arch/cpuid.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <smbios.h>
#include <soc/iomap.h>
#include <types.h>

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

static uint32_t get_sme_reserved_address_bits(void)
{
	if (rdmsr(SYSCFG_MSR).raw & SYSCFG_MSR_SMEE)
		return (cpuid_ebx(CPUID_EBX_MEM_ENCRYPT) &
			CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_MASK) >>
			CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_SHIFT;
	else
		return 0;
}

uint32_t get_usable_physical_address_bits(void)
{
	return cpu_phys_address_size() - get_sme_reserved_address_bits();
}
