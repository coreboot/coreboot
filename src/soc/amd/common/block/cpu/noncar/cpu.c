/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/mca.h>
#include <arch/cpuid.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/microcode.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <smbios.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <types.h>

#define UNSUPPORTED 0

#define UNINITIALIZED ((uint64_t)-1)

static union pstate_msr get_pstate0_msr(void)
{
	static union pstate_msr pstate_reg = { .raw = UNINITIALIZED };

	struct cpuid_result res;

	/* Check if we have already determined P-state support */
	if (pstate_reg.raw == UNINITIALIZED) {
		if (cpu_cpuid_extended_level() < CPUID_EXT_PM) {
			pstate_reg.pstate_en = UNSUPPORTED;
			return pstate_reg;
		}

		res = cpuid(CPUID_EXT_PM);

		if (!(res.edx & BIT(7))) { /* Hardware P-state control */
			pstate_reg.pstate_en = UNSUPPORTED;
			return pstate_reg;
		}

		pstate_reg.raw = rdmsr(PSTATE_MSR(get_pstate_0_reg())).raw;
	}

	return pstate_reg;
}

unsigned int smbios_cpu_get_current_speed_mhz(void)
{
	union pstate_msr pstate_reg = get_pstate0_msr();

	if (!pstate_reg.pstate_en)
		return 0;

	return get_pstate_core_freq(pstate_reg);
}

unsigned int smbios_cpu_get_voltage(void)
{
	union pstate_msr pstate_reg = get_pstate0_msr();

	if (!pstate_reg.pstate_en)
		return 0;

	return get_pstate_core_uvolts(pstate_reg) / 100000; /* uV to (10 * V) */
}

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

unsigned int smbios_processor_external_clock(void)
{
	return 100; /* 100 MHz */
}

void set_cstate_io_addr(void)
{
	msr_t cst_addr;

	cst_addr.hi = 0;
	cst_addr.lo = ACPI_CSTATE_CONTROL;
	wrmsr(MSR_CSTATE_ADDRESS, cst_addr);
}

/* Number of most significant physical address bits reserved for secure memory encryption */
unsigned int get_reserved_phys_addr_bits(void)
{
	if (!(rdmsr(SYSCFG_MSR).raw & SYSCFG_MSR_SMEE))
		return 0;

	return (cpuid_ebx(CPUID_EBX_MEM_ENCRYPT) & CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_MASK) >>
			CPUID_EBX_MEM_ENCRYPT_ADDR_BITS_SHIFT;
}

void amd_cpu_init(struct device *dev)
{
	if (CONFIG(SOC_AMD_COMMON_BLOCK_MCA_COMMON))
		check_mca();

	set_cstate_io_addr();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_UCODE))
		amd_apply_microcode_patch();
}
