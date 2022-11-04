/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <device/device.h>
#include <cpu/x86/topology.h>

#define CPUID_EXTENDED_CPU_TOPOLOGY 0x0b
#define LEVEL_TYPE_CORE 2
#define LEVEL_TYPE_SMT 1

#define CPUID_CPU_TOPOLOGY(x, val) \
	(((val) >> CPUID_CPU_TOPOLOGY_##x##_SHIFT) & CPUID_CPU_TOPOLOGY_##x##_MASK)

#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SHIFT 0x8
#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_MASK 0xff
#define CPUID_CPU_TOPOLOGY_LEVEL(res) CPUID_CPU_TOPOLOGY(LEVEL_TYPE, (res).ecx)

#define CPUID_CPU_TOPOLOGY_LEVEL_BITS_SHIFT 0x0
#define CPUID_CPU_TOPOLOGY_LEVEL_BITS_MASK 0x1f
#define CPUID_CPU_TOPOLOGY_THREAD_BITS(res) CPUID_CPU_TOPOLOGY(LEVEL_BITS, (res).eax)
#define CPUID_CPU_TOPOLOGY_CORE_BITS(res, threadbits) \
	((CPUID_CPU_TOPOLOGY(LEVEL_BITS, (res).eax)) - threadbits)

/* Get number of bits for core ID and SMT ID */
static enum cb_err get_cpu_core_thread_bits(uint32_t *core_bits, uint32_t *thread_bits)
{
	struct cpuid_result cpuid_regs;
	int level_num, cpu_id_op = 0;
	const uint32_t cpuid_max_func = cpuid_get_max_func();

	/*
	 * Not all CPUs support this, those won't get topology filled in here.
	 * CPU specific code can do this however.
	 */
	if (cpuid_max_func < CPUID_EXTENDED_CPU_TOPOLOGY)
		return CB_ERR;

	cpu_id_op = CPUID_EXTENDED_CPU_TOPOLOGY;

	*core_bits = level_num = 0;
	cpuid_regs = cpuid_ext(cpu_id_op, level_num);

	/*
	 * Sub-leaf index 0 enumerates SMT level, some AMD CPUs leave this CPUID leaf
	 * reserved so bail out. Cpu specific code can fill in the topology later.
	 */
	if (CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs) != LEVEL_TYPE_SMT)
		return CB_ERR;

	*thread_bits = CPUID_CPU_TOPOLOGY_THREAD_BITS(cpuid_regs);
	do {
		level_num++;
		cpuid_regs = cpuid_ext(cpu_id_op, level_num);
		if (CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs) == LEVEL_TYPE_CORE) {
			*core_bits = CPUID_CPU_TOPOLOGY_CORE_BITS(cpuid_regs, *thread_bits);
			break;
		}
	/* Stop when level type is invalid i.e 0 */
	} while (CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs));

	return CB_SUCCESS;
}

static void set_cpu_topology(struct device *cpu, unsigned int node,
		      unsigned int package, unsigned int core,
		      unsigned int thread)
{
	cpu->path.apic.node_id = node;
	cpu->path.apic.package_id = package;
	cpu->path.apic.core_id = core;
	cpu->path.apic.thread_id = thread;
}

void set_cpu_topology_from_leaf_b(struct device *cpu)
{
	static uint32_t core_bits, thread_bits;
	static enum cb_err core_thread_bits_ret;
	static bool done = false;
	if (!done) {
		core_thread_bits_ret = get_cpu_core_thread_bits(&core_bits, &thread_bits);
		done = true;
	}

	const uint32_t apicid = cpu->path.apic.initial_lapicid;
	uint32_t package_id, core_id, thread_id;
	/*
	 *  If leaf_b does not exist set the following best-guess defaults:
	 * - 1 package
	 * - no SMP
	 * - core_id = apicid
	 * CPU specific code can always update these later on.
	 */
	if (core_thread_bits_ret != CB_SUCCESS) {
		package_id = 0;
		core_id = apicid;
		thread_id = 0;
	} else {
		package_id = apicid >> (thread_bits + core_bits);
		core_id = (apicid >> thread_bits) & ((1 << core_bits) - 1);
		thread_id = apicid & ((1 << thread_bits) - 1);
	}

	set_cpu_topology(cpu, 0, package_id, core_id, thread_id);
}
