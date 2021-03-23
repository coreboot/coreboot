/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic.h>
#include <cpu/intel/common/common.h>
#include <arch/cpu.h>
#include <types.h>

bool intel_ht_supported(void)
{
	/* Is HyperThreading supported? */
	return !!(cpuid_edx(1) & CPUID_FEAURE_HTT);
}

/*
 * Return true if running thread does not have the smallest lapic ID
 * within a CPU core.
 */
bool intel_ht_sibling(void)
{
	struct cpuid_result result;
	unsigned int core_ids, apic_ids, threads;

	if (!intel_ht_supported())
		return false;

	if (is_x2apic_mode()) {
		if (cpuid_eax(0) >= 0xb) {
			result = cpuid_ext(0xb, 0);
			const uint32_t div = 1 << (result.eax & 0x1f);
			return result.edx % div > 0;
		}
	}

	apic_ids = 1;
	if (cpuid_eax(0) >= 1)
		apic_ids = (cpuid_ebx(1) >> 16) & 0xff;
	if (apic_ids == 0)
		apic_ids = 1;

	core_ids = 1;
	if (cpuid_eax(0) >= 4) {
		result = cpuid_ext(4, 0);
		core_ids += (result.eax >> 26) & 0x3f;
	}

	threads = (apic_ids / core_ids);
	return !!(lapicid() & (threads - 1));
}
