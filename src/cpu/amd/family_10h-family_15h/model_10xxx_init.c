/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>
#include <northbridge/amd/amdfam10/amdfam10.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/multicore.h>
#include <cpu/amd/msr.h>

#define MC0_STATUS 0x401

static inline uint8_t is_gt_rev_d(void)
{
	uint8_t fam15h = 0;
	uint8_t rev_gte_d = 0;
	uint32_t family;
	uint32_t model;

	family = model = cpuid_eax(0x80000001);
	model = ((model & 0xf0000) >> 12) | ((model & 0xf0) >> 4);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	if ((model >= 0x8) || fam15h)
		/* Revision D or later */
		rev_gte_d = 1;

	return rev_gte_d;
}

static volatile uint8_t fam15h_startup_flags[MAX_NODES_SUPPORTED][MAX_CORES_SUPPORTED] = {{ 0 }};

static void model_10xxx_init(struct device *dev)
{
	u8 i;
	msr_t msr;
	int num_banks;
	struct node_core_id id;
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	u32 siblings;
#endif
	uint8_t delay_start;

	id = get_node_core_id(read_nb_cfg_54());	/* nb_cfg_54 can not be set */
	printk(BIOS_DEBUG, "nodeid = %02d, coreid = %02d\n", id.nodeid, id.coreid);

	if (is_fam15h())
		delay_start = !!(id.coreid & 0x1);
	else
		delay_start = 0;

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	if (!delay_start) {
		/* Initialize all variable MTRRs except the first pair.
		 * This prevents Linux from having to correct an inconsistent
		 * MTRR setup, which would crash Family 15h CPUs due to the
		 * compute unit structure sharing MTRR MSRs between AP cores.
		 */
		msr.hi = 0x00000000;
		msr.lo = 0x00000000;

		disable_cache();

		for (i = 0x2; i < 0x10; i++) {
			wrmsr(0x00000200 | i, msr);
		}

		enable_cache();

		/* Set up other MTRRs */
		amd_setup_mtrrs();
	} else {
		while (!fam15h_startup_flags[id.nodeid][id.coreid - 1]) {
			/* Wait for CU first core startup */
		}
	}

	x86_mtrr_check();

	disable_cache();

	/* zero the machine check error status registers */
	msr = rdmsr(MCG_CAP);
	num_banks = msr.lo & MCA_BANKS_MASK;
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0; i < num_banks; i++)
		wrmsr(MC0_STATUS + (i * 4), msr);

	enable_cache();

	/* Enable the local CPU APICs */
	setup_lapic();

	/* Set the processor name string */
	init_processor_name();

#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
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

	/* Set bus unit configuration */
	if (is_fam15h()) {
		uint32_t f5x80;
		uint8_t enabled;
		uint8_t compute_unit_count = 0;
		f5x80 = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18 + id.nodeid, 5)), 0x80);
		enabled = f5x80 & 0xf;
		if (enabled == 0x1)
			compute_unit_count = 1;
		if (enabled == 0x3)
			compute_unit_count = 2;
		if (enabled == 0x7)
			compute_unit_count = 3;
		if (enabled == 0xf)
			compute_unit_count = 4;
		msr = rdmsr(BU_CFG2_MSR);
		msr.lo &= ~(0x3 << 6);				/* ThrottleNbInterface[1:0] */
		msr.lo |= (((compute_unit_count - 1) & 0x3) << 6);
		wrmsr(BU_CFG2_MSR, msr);
	} else {
		uint32_t f0x60;
		uint32_t f0x160;
		uint8_t core_count = 0;
		uint8_t node_count = 0;
		f0x60 = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18 + id.nodeid, 0)), 0x60);
		core_count = (f0x60 >> 16) & 0x1f;
		node_count = ((f0x60 >> 4) & 0x7) + 1;
		if (is_gt_rev_d()) {
			f0x160 = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18 + id.nodeid, 0)), 0x160);
			core_count |= ((f0x160 >> 16) & 0x7) << 5;
		}
		core_count++;
		core_count /= node_count;
		msr = rdmsr(BU_CFG2_MSR);
		if (is_gt_rev_d()) {
			msr.hi &= ~(0x3 << (36 - 32));			/* ThrottleNbInterface[3:2] */
			msr.hi |= ((((core_count - 1) >> 2) & 0x3) << (36 - 32));
		}
		msr.lo &= ~(0x3 << 6);				/* ThrottleNbInterface[1:0] */
		msr.lo |= (((core_count - 1) & 0x3) << 6);
		msr.lo &= ~(0x1 << 24);				/* WcPlusDis = 0 */
		wrmsr(BU_CFG2_MSR, msr);
	}

	/* Disable Cf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);

	if (is_fam15h()) {
		msr = rdmsr(BU_CFG3_MSR);
		/* Set CombineCr0Cd */
		msr.hi |= (1 << (49-32));
		wrmsr(BU_CFG3_MSR, msr);
	} else {
		msr = rdmsr(BU_CFG2_MSR);
		/* Clear ClLinesToNbDis */
		msr.lo &= ~(1 << 15);
		/* Clear bit 35 as per Erratum 343 */
		msr.hi &= ~(1 << (35-32));
		wrmsr(BU_CFG2_MSR, msr);
	}

	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		printk(BIOS_DEBUG, "Initializing SMM ASeg memory\n");

		/* Set SMM base address for this CPU */
		msr = rdmsr(SMM_BASE_MSR);
		msr.lo = SMM_BASE - (lapicid() * 0x400);
		wrmsr(SMM_BASE_MSR, msr);

		/* Enable the SMM memory window */
		msr = rdmsr(SMM_MASK_MSR);
		msr.lo |= (1 << 0); /* Enable ASEG SMRAM Range */
		wrmsr(SMM_MASK_MSR, msr);
	} else {
		printk(BIOS_DEBUG, "Disabling SMM ASeg memory\n");

		/* Set SMM base address for this CPU */
		msr = rdmsr(SMM_BASE_MSR);
		msr.lo = SMM_BASE - (lapicid() * 0x400);
		wrmsr(SMM_BASE_MSR, msr);

		/* Disable the SMM memory window */
		msr.hi = 0x0;
		msr.lo = 0x0;
		wrmsr(SMM_MASK_MSR, msr);
	}

	/* Set SMMLOCK to avoid exploits messing with SMM */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);

	fam15h_startup_flags[id.nodeid][id.coreid] = 1;
}

static struct device_operations cpu_dev_ops = {
	.init = model_10xxx_init,
};

static const struct cpu_device_id cpu_table[] = {
//AMD_GH_SUPPORT
	{ X86_VENDOR_AMD, 0x100f00 },		/* SH-F0 L1 */
	{ X86_VENDOR_AMD, 0x100f10 },		/* M2 */
	{ X86_VENDOR_AMD, 0x100f20 },		/* S1g1 */
	{ X86_VENDOR_AMD, 0x100f21 },
	{ X86_VENDOR_AMD, 0x100f2A },
	{ X86_VENDOR_AMD, 0x100f22 },
	{ X86_VENDOR_AMD, 0x100f23 },
	{ X86_VENDOR_AMD, 0x100f40 },		/* RB-C0 */
	{ X86_VENDOR_AMD, 0x100f42 },           /* RB-C2 */
	{ X86_VENDOR_AMD, 0x100f43 },           /* RB-C3 */
	{ X86_VENDOR_AMD, 0x100f52 },           /* BL-C2 */
	{ X86_VENDOR_AMD, 0x100f62 },           /* DA-C2 */
	{ X86_VENDOR_AMD, 0x100f63 },           /* DA-C3 */
	{ X86_VENDOR_AMD, 0x100f80 },           /* HY-D0 */
	{ X86_VENDOR_AMD, 0x100f81 },           /* HY-D1 */
	{ X86_VENDOR_AMD, 0x100f91 },           /* HY-D1 */
	{ X86_VENDOR_AMD, 0x100fa0 },           /* PH-E0 */
	{ X86_VENDOR_AMD, 0x600f12 },           /* OR-B2 */
	{ X86_VENDOR_AMD, 0x600f20 },           /* OR-C0 */
	{ 0, 0 },
};

static const struct cpu_driver model_10xxx __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
