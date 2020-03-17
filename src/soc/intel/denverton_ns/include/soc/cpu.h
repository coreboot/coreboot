/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _CPU_INTEL_DENVERTON_NS_H
#define _CPU_INTEL_DENVERTON_NS_H

int get_cpu_count(void);

#ifndef __ASSEMBLER__
#include <device/device.h>

void denverton_init_cpus(struct device *dev);
#endif

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Denverton-NS CPUID */
#define CPUID_DENVERTON_A0_A1 0x506f0
#define CPUID_DENVERTON_B0 0x506f1

#define MSR_CORE_THREAD_COUNT 0x35
#define CORE_BIT_MSK 0x1
#define MCH_BAR_CORE_EXISTS_MASK 0x7164
#define MCH_BAR_CORE_DISABLE_MASK 0x7168

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

#endif /* __ACPI__ */

#endif /* _CPU_INTEL_DENVERTON_NS_H */
