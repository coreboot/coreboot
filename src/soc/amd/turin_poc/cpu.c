/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/cpu.h>

static struct device_operations cpu_dev_ops = {
	.init = amd_cpu_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, TURIN_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, TURIN_B0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver model_19 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
