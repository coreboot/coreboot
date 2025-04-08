/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Phoenix */

#include <amdblocks/cpu.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/cpu.h>

_Static_assert(CONFIG_MAX_CPUS == 16, "Do not override MAX_CPUS. To reduce the number of "
	"available cores, use the downcore_mode and disable_smt devicetree settings instead.");

static struct device_operations cpu_dev_ops = {
	.init = amd_cpu_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, PHOENIX_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, PHOENIX2_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver zen_2_3 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
