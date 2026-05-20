/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <types.h>

#define CPUID_EXTENDED_CPU_TOPOLOGY		0x0b
#define CPUID_EXTENDED_CPU_TOPOLOGY2		0x1f
#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SHIFT	8
#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_MASK	0xff
#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SMT	1
#define CPUID_CPU_TOPOLOGY_LEVEL(res) \
	(((res).ecx >> CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SHIFT) & \
	 CPUID_CPU_TOPOLOGY_LEVEL_TYPE_MASK)

bool intel_ht_or_mc_supported(void)
{
	/* Is Hyper-Threading or multi-core supported? */
	return !!(cpuid_edx(1) & CPUID_FEATURE_HTT);
}

static bool topology_leaf_smt_threads(unsigned int leaf, unsigned int *threads)
{
	struct cpuid_result result;

	if (cpuid_get_max_func() < leaf)
		return false;

	result = cpuid_ext(leaf, 0);
	if (CPUID_CPU_TOPOLOGY_LEVEL(result) != CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SMT)
		return false;

	*threads = result.ebx & 0xffff;
	return *threads != 0;
}

bool intel_ht_supported(void)
{
	struct cpuid_result result;
	unsigned int logical_processors;
	unsigned int threads;
	unsigned int cores;

	if (!cpu_have_cpuid())
		return false;

	if (topology_leaf_smt_threads(CPUID_EXTENDED_CPU_TOPOLOGY2, &threads) ||
	    topology_leaf_smt_threads(CPUID_EXTENDED_CPU_TOPOLOGY, &threads))
		return threads > 1;

	if (!intel_ht_or_mc_supported())
		return false;

	result = cpuid(1);
	logical_processors = (result.ebx >> 16) & 0xff;
	if (logical_processors <= 1)
		return false;

	if (cpuid_get_max_func() < 4)
		return false;

	result = cpuid_ext(4, 0);
	cores = ((result.eax >> 26) & 0x3f) + 1;

	return logical_processors > cores;
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
