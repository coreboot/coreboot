/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_TOPOLOGY_H
#define CPU_X86_TOPOLOGY_H

#include <device/device.h>

/* Fill in the topology in struct path APIC based CPUID EAX=0xb.
 * If leaf 0xb is not supported or is not implemented then no topology
 * will be filled in.
 */
void set_cpu_topology_from_leaf_b(struct device *cpu);

/* Fill in the topology node ID in struct path APIC based CPUID EAX=0x1f
 * or CPUID EAX=0xb. If those leaves aren't supported then the node ID
 * won't be updated.
 */
void set_cpu_node_id_leaf_1f_b(struct device *cpu);
#endif
