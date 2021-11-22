/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_CPU_H__
#define __ARCH_CPU_H__

#include <arch/encoding.h>
#include <device/device.h>

static inline void cpu_relax(void) { }

#define asmlinkage

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct cpuinfo_riscv {
	uint8_t    riscv;            /* CPU family */
	uint8_t    riscv_vendor;     /* CPU vendor */
	uint8_t    riscv_model;
};

static inline int supports_extension(char ext)
{
	return read_csr(misa) & (1 << (ext - 'A'));
}

static inline int machine_xlen(void)
{
	int mxl = (read_csr(misa) >> (__riscv_xlen - 2)) & 3;
	return (1 << mxl) * 16;
}

#endif /* __ARCH_CPU_H__ */
