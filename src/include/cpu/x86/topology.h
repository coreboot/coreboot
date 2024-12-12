/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_TOPOLOGY_H
#define CPU_X86_TOPOLOGY_H

#include <device/device.h>

/*
 * Sets the topology information for the given CPU device using the bitfield descriptors
 * obtained from the CPUID leaves. Per Intel Software Developer Manual recommendation, it
 * prioritizes CPUID EAX=0x1f over CPUID EAX=0xb if available.
 *
 * If the topology information cannot be obtained from CPUID, it sets default values.
 */
void set_cpu_topology(struct device *cpu);

#endif
