/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/cpu.h>
#include <amdblocks/smm.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <northbridge/amd/agesa/agesa_helper.h>

static void model_14_init(struct device *dev)
{
	msr_t msr;
	int msrno;
#if CONFIG(LOGICAL_CPUS)
	u32 siblings;
#endif
	printk(BIOS_DEBUG, "Model 14 Init.\n");

	disable_cache();
	/*
	 * AGESA sets the MTRRs main MTRRs. The shadow area needs to be set
	 * by coreboot.
	 */

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	/* Set shadow WB, RdMEM, WrMEM */
	msr.lo = msr.hi = 0;
	wrmsr(MTRR_FIX_16K_A0000, msr);
	msr.lo = msr.hi = 0x1e1e1e1e;
	wrmsr(MTRR_FIX_64K_00000, msr);
	wrmsr(MTRR_FIX_16K_80000, msr);
	for (msrno = MTRR_FIX_4K_C0000; msrno <= MTRR_FIX_4K_F8000; msrno++)
		wrmsr(msrno, msr);

	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	if (acpi_is_wakeup_s3())
		restore_mtrr();

	x86_mtrr_check();
	enable_cache();

	/* zero the machine check error status registers */
	mca_clear_status();

#if CONFIG(LOGICAL_CPUS)
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
#endif

	/* DisableCf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);

	/* Write protect SMM space with SMMLOCK. */
	lock_smm();
}

static struct device_operations cpu_dev_ops = {
	.init = model_14_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x500f00 },   /* ON-A0 */
	{ X86_VENDOR_AMD, 0x500f01 },   /* ON-A1 */
	{ X86_VENDOR_AMD, 0x500f10 },   /* ON-B0 */
	{ X86_VENDOR_AMD, 0x500f20 },   /* ON-C0 */
	{ 0, 0 },
};

static const struct cpu_driver model_14 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
