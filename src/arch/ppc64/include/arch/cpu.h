/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <device/device.h>

static inline void cpu_relax(void) { }

#define asmlinkage

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct cpuinfo_ppc64 {
	uint8_t    ppc64;            /* CPU family */
	uint8_t    ppc64_vendor;     /* CPU vendor */
	uint8_t    ppc64_model;
};

#endif /* __ARCH_CPU_H__ */
