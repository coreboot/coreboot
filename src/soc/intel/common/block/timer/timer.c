/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2019 Intel Corporation.
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

#include <arch/cpu.h>
#include <arch/intel-family.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <intelblocks/msr.h>

static int get_processor_model(void)
{
	struct cpuinfo_x86 c;

	get_fms(&c, cpuid_eax(1));

	return c.x86_model;
}

/*
 * Nominal TSC frequency = "core crystal clock frequency" * EBX/EAX
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
 * Refer to Intel SDM Jan 2019 Vol 3B Section 18.7.3
 */
unsigned long tsc_freq_mhz(void)
{
	unsigned int core_crystal_nominal_freq_khz;
	struct cpuid_result cpuidr;

	/* CPUID 15H TSC/Crystal ratio, plus optionally Crystal Hz */
	cpuidr = cpuid(0x15);

	if (!cpuidr.ebx || !cpuidr.eax)
		return 0;

	core_crystal_nominal_freq_khz = cpuidr.ecx / 1000;

	if (!core_crystal_nominal_freq_khz) {
		switch (get_processor_model()) {
		case CPU_MODEL_INTEL_SKYLAKE_MOBILE:
		case CPU_MODEL_INTEL_SKYLAKE_DESKTOP:
		case CPU_MODEL_INTEL_KABYLAKE_MOBILE:
		case CPU_MODEL_INTEL_KABYLAKE_DESKTOP:
		case CPU_MODEL_INTEL_CANNONLAKE_MOBILE:
		case CPU_MODEL_INTEL_ICELAKE_MOBILE:
			core_crystal_nominal_freq_khz = 24000;
			break;
		case CPU_MODEL_INTEL_ATOM_DENVERTON:
			core_crystal_nominal_freq_khz = 25000;
			break;
		case CPU_MODEL_INTEL_ATOM_GOLDMONT:
		case CPU_MODEL_INTEL_ATOM_GEMINI_LAKE:
			core_crystal_nominal_freq_khz = 19200;
			break;
		}
	}

	return (core_crystal_nominal_freq_khz * cpuidr.ebx / cpuidr.eax) /
			1000;
}
