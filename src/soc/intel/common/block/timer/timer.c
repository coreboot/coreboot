/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <cpu/x86/tsc.h>
#include <intelblocks/msr.h>

/* Goldmont Microserver */
#define CPU_MODEL_INTEL_ATOM_DENVERTON 0x5F

static int get_processor_model(void)
{
	struct cpuinfo_x86 c;

	get_fms(&c, cpuid_eax(1));

	return c.x86_model;
}

static unsigned long get_hardcoded_crystal_freq(void)
{
	unsigned long core_crystal_nominal_freq_khz = 0;

	/*
	 * Denverton SoCs don't report crystal clock, and also don't support
	 * CPUID.0x16, so hardcode the 25MHz crystal clock.
	 */
	switch (get_processor_model()) {
	case CPU_MODEL_INTEL_ATOM_DENVERTON:
		core_crystal_nominal_freq_khz = 25000;
		break;
	}

	return core_crystal_nominal_freq_khz;
}

/*
 * Nominal TSC frequency = "core crystal clock frequency" *
 *                               CPUID_15h.EBX/CPUID_15h.EAX
 *
 * Time Stamp Counter
 * CPUID Initial EAX value = 0x15
 * EAX Bit 31-0 : An unsigned integer which is the denominator of the
 * TSC/"core crystal clock" ratio
 * EBX Bit 31-0 : An unsigned integer which is the numerator of the
 * TSC/"core crystal clock" ratio
 * ECX Bit 31-0 : An unsigned integer which is the nominal frequency of the
 * core crystal clock in Hz.
 * EDX Bit 31-0 : Reserved = 0
 *
 */
static unsigned long calculate_tsc_freq_from_core_crystal(void)
{
	unsigned long core_crystal_nominal_freq_khz;
	struct cpuid_result cpuidr_15h;

	if (cpuid_get_max_func() < 0x15)
		return 0;

	/* CPUID 15H TSC/Crystal ratio, plus optionally Crystal Hz */
	cpuidr_15h = cpuid(0x15);

	if (!cpuidr_15h.ebx || !cpuidr_15h.eax)
		return 0;

	core_crystal_nominal_freq_khz = cpuidr_15h.ecx / 1000;

	if (!core_crystal_nominal_freq_khz)
		core_crystal_nominal_freq_khz = get_hardcoded_crystal_freq();

	return (core_crystal_nominal_freq_khz * cpuidr_15h.ebx /
			cpuidr_15h.eax) / 1000;
}

/*
 * Processor Frequency Information
 * CPUID Initial EAX value = 0x16
 * EAX Bit 31-0 : An unsigned integer which has the processor base frequency
 * information
 * EBX Bit 31-0 : An unsigned integer which has maximum frequency information
 * ECX Bit 31-0 : An unsigned integer which has bus frequency information
 * EDX Bit 31-0 : Reserved = 0
 *
 * Refer to Intel SDM Jan 2019 Vol 3B Section 18.7.3
 */
static unsigned long get_freq_from_cpuid16h(void)
{
	if (cpuid_get_max_func() < 0x16)
		return 0;

	return cpuid_eax(0x16);
}

unsigned long tsc_freq_mhz(void)
{
	unsigned long tsc_freq;

	tsc_freq = calculate_tsc_freq_from_core_crystal();

	if (tsc_freq)
		return tsc_freq;

	/*
	 * Some Intel SoCs like Skylake, Kabylake and Cometlake don't report
	 * the crystal clock, in that case return bus frequency using CPUID.16h
	 */
	return get_freq_from_cpuid16h();
}
