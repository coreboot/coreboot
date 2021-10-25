/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/cache.h>

static void model_f2x_init(struct device *cpu)
{
	/* Turn on caching if we haven't already */
	enable_cache();

	if (!intel_ht_sibling()) {
		/* MTRRs are shared between threads */
		x86_setup_mtrrs();
		x86_mtrr_check();

		/* Update the microcode */
		intel_update_microcode_from_cbfs();
	}

	/* Enable the local CPU APICs */
	setup_lapic();

	/* Start up my CPU siblings */
	intel_sibling_init(cpu);
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
