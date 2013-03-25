/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>

#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/amdfam14.h>
#include <arch/acpi.h>
#if CONFIG_HAVE_ACPI_RESUME
#include <cpu/amd/agesa/s3_resume.h>
#endif

#define MCI_STATUS 0x401

static void model_14_init(device_t dev)
{
	u32 i;
	msr_t msr;
#if CONFIG_LOGICAL_CPUS
	u32 siblings;
#endif
	printk(BIOS_DEBUG, "Model 14 Init.\n");

	disable_cache ();
	/*
	 * AGESA sets the MTRRs main MTRRs. The shadow area needs to be set
	 * by coreboot. The amd_setup_mtrrs should work, but needs debug on fam14.
	 * TODO:
	 * amd_setup_mtrrs();
	 */

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	/* Set shadow WB, RdMEM, WrMEM */
	msr.lo = msr.hi = 0;
	wrmsr (0x259, msr);
	msr.hi = msr.lo = 0x1e1e1e1e;
	wrmsr(0x250, msr);
	wrmsr(0x258, msr);
	for (i = 0x268; i <= 0x26f; i++)
		wrmsr(i, msr);

	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_slp_type == 3)
		restore_mtrr();
#endif

	x86_mtrr_check();
	x86_enable_cache();

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0; i < 6; i++) {
		wrmsr(MCI_STATUS + (i * 4), msr);
	}

	/* Enable the local cpu apics */
	setup_lapic();

#if CONFIG_LOGICAL_CPUS
	siblings = cpuid_ecx(0x80000008) & 0xff;

	if (siblings > 0) {
		msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
		msr.lo |= 1 << 28;
		wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

		msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
		msr.hi |= 1 << (33 - 32);
		wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
	}
#endif

	/* DisableCf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);

	/* Write protect SMM space with SMMLOCK. */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);

	printk(BIOS_SPEW, "%s done.\n", __func__);
}

static struct device_operations cpu_dev_ops = {
	.init = model_14_init,
};

static struct cpu_device_id cpu_table[] = {
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
