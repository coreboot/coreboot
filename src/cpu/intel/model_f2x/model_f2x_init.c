/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/cache.h>

static void model_f2x_init(struct device *cpu)
{
	/* Turn on caching if we haven't already */
	enable_cache();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_f2x_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f22, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f24, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f25, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f26, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f27, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f29, CPUID_EXACT_MATCH_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
