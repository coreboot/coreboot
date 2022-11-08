/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
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
	{ X86_VENDOR_INTEL, 0x0f22 },
	{ X86_VENDOR_INTEL, 0x0f24 },
	{ X86_VENDOR_INTEL, 0x0f25 },
	{ X86_VENDOR_INTEL, 0x0f26 },
	{ X86_VENDOR_INTEL, 0x0f27 },
	{ X86_VENDOR_INTEL, 0x0f29 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

/* Parallel MP initialization support. */
static void pre_mp_init(void)
{
	const void *patch = intel_microcode_find();
	intel_microcode_load_unlocked(patch);

	/* Setup MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	return CONFIG_MAX_CPUS;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = !intel_ht_supported();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_microcode_info = get_microcode_info,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	mp_init_with_smm(cpu_bus, &mp_ops);
}
