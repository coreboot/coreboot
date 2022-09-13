/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <amdblocks/smm.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/amd/microcode.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <smp/node.h>

static void model_16_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Model 16 Init.\n");

	msr_t msr;
	u32 siblings;

	/* zero the machine check error status registers */
	mca_clear_status();

	if (CONFIG(LOGICAL_CPUS)) {
		siblings = get_cpu_count() - 1; // minus BSP

		if (siblings > 0) {
			msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
			msr.lo |= 1 << 28;
			wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

			msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
			msr.hi |= 1 << (33 - 32);
			wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
		}
		printk(BIOS_DEBUG, "siblings = %02d, ", siblings);
	}

	/* DisableCf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);

	/* Write protect SMM space with SMMLOCK. */
	lock_smm();

	amd_update_microcode_from_cbfs();

	display_mtrrs();
}

static struct device_operations cpu_dev_ops = {
	.init = model_16_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x730F00 },
	{ X86_VENDOR_AMD, 0x730F01 },
	{ 0, 0 },
};

static const struct cpu_driver model_16 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
