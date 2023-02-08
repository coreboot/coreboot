/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>

static void model_f4x_init(struct device *cpu)
{
	/* Turn on caching if we haven't already */
	enable_cache();
};

static struct device_operations cpu_dev_ops = {
	.init = model_f4x_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f41, CPUID_EXACT_MATCH_MASK }, /* Xeon */
	{ X86_VENDOR_INTEL, 0x0f43, CPUID_EXACT_MATCH_MASK }, /* Not tested */
	{ X86_VENDOR_INTEL, 0x0f44, CPUID_EXACT_MATCH_MASK }, /* Not tested */
	{ X86_VENDOR_INTEL, 0x0f47, CPUID_EXACT_MATCH_MASK },
	{ X86_VENDOR_INTEL, 0x0f48, CPUID_EXACT_MATCH_MASK }, /* Not tested */
	{ X86_VENDOR_INTEL, 0x0f49, CPUID_EXACT_MATCH_MASK }, /* Not tested */
	{ X86_VENDOR_INTEL, 0x0f4a, CPUID_EXACT_MATCH_MASK }, /* Not tested */
	CPU_TABLE_END
};

static const struct cpu_driver model_f4x __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
