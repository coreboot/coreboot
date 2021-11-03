/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_DENVERTON_NS_H
#define _CPU_INTEL_DENVERTON_NS_H

#include <cpu/intel/cpu_ids.h>

int get_cpu_count(void);

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

#define MSR_CORE_THREAD_COUNT 0x35
#define MCH_BAR_CORE_EXISTS_MASK 0x7164
#define MCH_BAR_CORE_DISABLE_MASK 0x7168

/* CPU bus clock is fixed at 100MHz */
#define CPU_BCLK 100

#endif /* __ACPI__ */

#endif /* _CPU_INTEL_DENVERTON_NS_H */
