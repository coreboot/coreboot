/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
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
	unsigned int core_ids, apic_ids;
	unsigned int max_leaf;
	uint32_t initial_lapicid, threads;

	if (!intel_ht_supported())
		return false;

	max_leaf = cpuid_get_max_func();

	/* Detect from 32-bit X2APIC ID. */
	if (max_leaf >= 0xb) {
		result = cpuid_ext(0xb, 0);
		threads = 1 << (result.eax & 0x1f);
		initial_lapicid = result.edx;
		return initial_lapicid % threads > 0;
	}

	/* Detect from 8-bit XAPIC ID. */
	result = cpuid_ext(0x1, 0);
	initial_lapicid = result.ebx >> 24;
	apic_ids = (result.ebx >> 16) & 0xff;
	if (apic_ids == 0)
		apic_ids = 1;

	core_ids = 1;
	if (max_leaf >= 4) {
		result = cpuid_ext(4, 0);
		core_ids += (result.eax >> 26) & 0x3f;
	}

	threads = (apic_ids / core_ids);
	return initial_lapicid % threads > 0;
}
