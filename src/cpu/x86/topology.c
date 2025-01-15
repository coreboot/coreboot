/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/topology.h>
#include <device/device.h>

/*
 *  The following code defines functions for populating the topology information of a given CPU
 *  device (struct device *cpu).
 *
 * Reference: Intel Software Developer Manual Extended Topology Enumeration Leaves (CPUID EAX 0xb
 *  and 0x1f).
 */

#define CPUID_EXTENDED_CPU_TOPOLOGY	0x0b
#define CPUID_EXTENDED_CPU_TOPOLOGY2	0x1f

enum  {
	LEVEL_TYPE_PACKAGE,	/* Use unused level zero for package. */
	LEVEL_TYPE_SMT,
	LEVEL_TYPE_CORE,
	LEVEL_TYPE_MODULE,
	LEVEL_TYPE_TILE,
	LEVEL_TYPE_DIE,
	LEVEL_TYPE_DIEGRP,
	LEVEL_TYPE_MAX
};

#define CPUID_CPU_TOPOLOGY(x, val) \
	(((val) >> CPUID_CPU_TOPOLOGY_##x##_SHIFT) & CPUID_CPU_TOPOLOGY_##x##_MASK)

#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_SHIFT	0x8
#define CPUID_CPU_TOPOLOGY_LEVEL_TYPE_MASK	0xff
#define CPUID_CPU_TOPOLOGY_LEVEL(res)		CPUID_CPU_TOPOLOGY(LEVEL_TYPE, (res).ecx)

#define CPUID_CPU_TOPOLOGY_LEVEL_BITS_SHIFT	0x0
#define CPUID_CPU_TOPOLOGY_LEVEL_BITS_MASK	0x1f

struct bitfield_descriptor {
	uint8_t first_bit;
	uint8_t num_bits;
};

/*
 * Fills in the supplied bit field APIC ID descriptor structure array.
 *
 * Per Intel Software Developer Manual recommendation, it prioritizes CPUID EAX=0x1f over CPUID
 * EAX=0xb if available.  The function iterates over the hierarchy levels to extract per-domain
 * (level) APIC bit field information and populates the provided topology array with the
 * bitfield descriptors for each topology level.
 *
 * Returns CB_ERR if:
 * - CPUID EAX=0x1f is not supported.
 * - CPUID EAX=0xb is not supported.
 * - The first topology level is 0.
 *
 * Returns CB_SUCCESS otherwise.
 */
static enum cb_err read_topology_structure(struct bitfield_descriptor *topology)
{
	const uint32_t cpuid_max_func = cpuid_get_max_func();
	struct cpuid_result cpuid_regs;
	int level_num = 0, cpu_id_op;
	unsigned int level;
	uint8_t next_level_first_bit, first_bit = 0;

	/*
	 * Not all CPUs support this, those won't get topology filled in here.  CPU specific
	 * code can do this however.
	 */
	if (cpuid_max_func >= CPUID_EXTENDED_CPU_TOPOLOGY2)
		cpu_id_op = CPUID_EXTENDED_CPU_TOPOLOGY2;
	else if (cpuid_max_func >= CPUID_EXTENDED_CPU_TOPOLOGY)
		cpu_id_op = CPUID_EXTENDED_CPU_TOPOLOGY;
	else
		return CB_ERR;

	cpuid_regs = cpuid_ext(cpu_id_op, level_num);

	/*
	 * Sub-leaf index 0 enumerates SMT level, some AMD CPUs leave this CPUID leaf reserved
	 * so bail out. Cpu specific code can fill in the topology later.
	 */
	if (CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs) != LEVEL_TYPE_SMT)
		return CB_ERR;

	do {
		level = CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs);
		next_level_first_bit = CPUID_CPU_TOPOLOGY(LEVEL_BITS, cpuid_regs.eax);
		if (level < LEVEL_TYPE_MAX) {
			topology[level].num_bits = next_level_first_bit - first_bit;
			topology[level].first_bit = first_bit;
		} else {
			printk(BIOS_ERR, "Unknown processor topology level %d\n", level);
		}
		first_bit = next_level_first_bit;
		level_num++;
		cpuid_regs = cpuid_ext(cpu_id_op, level_num);
	/* Stop when level type is invalid i.e 0. */
	} while (CPUID_CPU_TOPOLOGY_LEVEL(cpuid_regs));

	topology[LEVEL_TYPE_PACKAGE].first_bit = next_level_first_bit;
	return CB_SUCCESS;
}

void set_cpu_topology(struct device *cpu)
{
	const uint32_t apicid = cpu->path.apic.initial_lapicid;
	static struct bitfield_descriptor topology[LEVEL_TYPE_MAX];
	static enum cb_err ret;
	static bool done;
	unsigned int core_id_within_package;
	struct {
		unsigned int level;
		unsigned int *field;
	} apic_fields[] = {
		{ LEVEL_TYPE_SMT, &cpu->path.apic.thread_id },
		{ LEVEL_TYPE_CORE, &cpu->path.apic.core_id },
		{ LEVEL_TYPE_MODULE, &cpu->path.apic.module_id },
		{ LEVEL_TYPE_PACKAGE, &cpu->path.apic.package_id },
		{ LEVEL_TYPE_PACKAGE, &cpu->path.apic.node_id }
	};

	if (!done) {
		ret = read_topology_structure(topology);
		done = true;
	}

	/*
	 * If no APIC descriptor is found, set the following best-guess defaults:
	 * - 1 package
	 * - core_id = apicid
	 * - no SMP
	 * - 1 node
	 * CPU specific code can always update these later on.
	 */
	if (ret != CB_SUCCESS) {
		cpu->path.apic.package_id = 0;
		cpu->path.apic.core_id = apicid;
		cpu->path.apic.thread_id = 0;
		cpu->path.apic.node_id = 0;
		return;
	}

	for (size_t i = 0; i < ARRAY_SIZE(apic_fields); i++) {
		struct bitfield_descriptor *desc = &topology[apic_fields[i].level];
		if (desc->first_bit || desc->num_bits) {
			unsigned int value = apicid >> desc->first_bit;
			if (desc->num_bits)
				value &= (1 << (desc->num_bits)) - 1;
			*apic_fields[i].field = value;
		}
	}

	core_id_within_package = apicid & ((1 << topology[LEVEL_TYPE_PACKAGE].first_bit) - 1);
	core_id_within_package >>= topology[LEVEL_TYPE_CORE].first_bit;
	cpu->path.apic.core_id_within_package = core_id_within_package;
}
