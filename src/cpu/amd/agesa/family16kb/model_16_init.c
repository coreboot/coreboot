/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <cpu/amd/amdfam16.h>
#include <arch/acpi.h>
#if CONFIG_HAVE_ACPI_RESUME
#include <cpu/amd/agesa/s3_resume.h>
#endif

static void model_16_init(device_t dev)
{
	printk(BIOS_DEBUG, "Model 16 Init.\n");

	u8 i;
	msr_t msr;
	int msrno;
#if CONFIG_LOGICAL_CPUS
	u32 siblings;
#endif

	//x86_enable_cache();
	//amd_setup_mtrrs();
	//x86_mtrr_check();
	disable_cache ();
	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	// BSP: make a0000-bffff UC, c0000-fffff WB, same as OntarioApMtrrSettingsList for APs
	msr.lo = msr.hi = 0;
	wrmsr (0x259, msr);
	msr.lo = msr.hi = 0x1e1e1e1e;
	wrmsr(0x250, msr);
	wrmsr(0x258, msr);
	for (msrno = 0x268; msrno <= 0x26f; msrno++)
		wrmsr (msrno, msr);

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
	printk(BIOS_DEBUG, "siblings = %02d, ", siblings);
#endif

	/* DisableCf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);


	/* Write protect SMM space with SMMLOCK. */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);
}

static struct device_operations cpu_dev_ops = {
	.init = model_16_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x700f00 },	  /* KB-A0 */
	{ 0, 0 },
};

static const struct cpu_driver model_15 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
