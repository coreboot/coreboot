/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/mca.h>
#include <cpu/amd/microcode.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <soc/cpu.h>

_Static_assert(CONFIG_MAX_CPUS == 8, "Do not override MAX_CPUS. To reduce the number of "
	"available cores, use the downcore_mode and disable_smt devicetree settings instead.");

static void model_17_init(struct device *dev)
{
	check_mca();
	set_cstate_io_addr();

	amd_apply_microcode_patch();
}

static struct device_operations cpu_dev_ops = {
	.init = model_17_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, RAVEN1_B0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, PICASSO_B0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	{ X86_VENDOR_AMD, RAVEN2_A0_CPUID, CPUID_ALL_STEPPINGS_MASK },
	CPU_TABLE_END
};

static const struct cpu_driver model_17 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
