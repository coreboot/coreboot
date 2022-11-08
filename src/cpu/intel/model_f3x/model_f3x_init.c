/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>

static void model_f3x_init(struct device *cpu)
{
	/* Turn on caching if we haven't already */
	enable_cache();
};

static struct device_operations cpu_dev_ops = {
	.init = model_f3x_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0f34 }, /* Xeon */
	{ 0, 0 },
};

static const struct cpu_driver model_f3x __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
