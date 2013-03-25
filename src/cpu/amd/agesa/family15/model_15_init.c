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

#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/amdfam15.h>

static void model_15_init(device_t dev)
{
	printk(BIOS_DEBUG, "Model 15 Init.\n");

	u8 i;
	msr_t msr;
	int msrno;
#if CONFIG_LOGICAL_CPUS
	u32 siblings;
#endif

	disable_cache ();
	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);

	// BSP: make a0000-bffff UC, c0000-fffff WB, same as ApMtrrSettingsList for APs
	msr.lo = msr.hi = 0;
	wrmsr (0x259, msr);
	msr.lo = msr.hi = 0x1e1e1e1e;
	for (msrno = 0x268; msrno <= 0x26f; msrno++)
		wrmsr (msrno, msr);

	msr.lo = 0x04040404; msr.hi = 0x04040404;
	wrmsr(0x259, msr);

	/* disable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);
	enable_cache ();

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
	.init = model_15_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x100F80},    /* HY-D0 */
	{ X86_VENDOR_AMD, 0x100F90},    /* HY-D0 */
	{ X86_VENDOR_AMD, 0x100F81},    /* HY-D1 */
	{ X86_VENDOR_AMD, 0x100F91},    /* HY-D1 */
	{ X86_VENDOR_AMD, 0x600f00 },   /* OR_A0x */
	{ X86_VENDOR_AMD, 0x600f01 },   /* OR_A0x */
	{ X86_VENDOR_AMD, 0x600f10 },   /* OR_B0x */
	{ X86_VENDOR_AMD, 0x600f11 },   /* OR_B1x */
	{ X86_VENDOR_AMD, 0x600f12 },   /* OR_B2x */
	{ X86_VENDOR_AMD, 0x600f13 },   /* OR_B3x */
	{ X86_VENDOR_AMD, 0x600f20 },   /* OR_C0x */
	{ 0, 0 },
};

static const struct cpu_driver model_15 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
