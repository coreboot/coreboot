/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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
#include <chip.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>

static int is_sgx_supported(void)
{
	struct cpuid_result cpuid_regs;
	msr_t msr;

	cpuid_regs = cpuid_ext(0x7, 0x0); /* EBX[2] is feature capability */
	msr = rdmsr(MTRR_CAP_MSR); /* Bit 12 is PRMRR enablement */
	return ((cpuid_regs.ebx & 0x4) && (msr.lo & PRMRR_SUPPORTED));
}

static int configure_core_prmrr(void)
{
	msr_t prmrr_base;
	msr_t prmrr_mask;
	msr_t msr;

	/*
	 * PRMRR base and mask are read from the UNCORE PRMRR MSRs
	 * that are already set in FSP-M.
	 */
	prmrr_base = rdmsr(UNCORE_PRMRR_PHYS_BASE_MSR);
	prmrr_mask = rdmsr(UNCORE_PRMRR_PHYS_MASK_MSR);
	if (!prmrr_base.lo) {
		printk(BIOS_ERR, "SGX Error: Uncore PRMRR is not set!\n");
		return -1;
	}

	msr = rdmsr(PRMRR_PHYS_MASK_MSR);
	/* If it is locked don't attempt to write PRMRR MSRs. */
	if (msr.lo & PRMRR_PHYS_MASK_LOCK)
		return 0;

	/* Program core PRMRR MSRs */
	prmrr_base.lo |= 0x6; /* Set memory attribute to cache writeback */
	wrmsr(PRMRR_PHYS_BASE_MSR, prmrr_base);
	prmrr_mask.lo &= ~PRMRR_PHYS_MASK_VALID; /* Do not set the valid bit */
	prmrr_mask.lo |= PRMRR_PHYS_MASK_LOCK; /* Lock it */
	wrmsr(PRMRR_PHYS_MASK_MSR, prmrr_mask);
	return 0;
}

static void enable_sgx(void)
{
	msr_t msr;

	msr = rdmsr(IA32_FEATURE_CONTROL);
	/* Only enable it when it is not locked */
	if ((msr.lo & 1) == 0) {
		msr.lo |= (1 << 18); /* Enable it */
		wrmsr(IA32_FEATURE_CONTROL, msr);
	}
}

static void lock_sgx(void)
{
	msr_t msr;

	msr = rdmsr(IA32_FEATURE_CONTROL);
	/* If it is locked don't attempt to lock it again. */
	if ((msr.lo & 1) == 0) {
		msr.lo |= 1; /* Lock it */
		wrmsr(IA32_FEATURE_CONTROL, msr);
	}
}

static int owner_epoch_update(void)
{
	/*
	 * TODO - the Owner Epoch update mechanism is not determined yet,
	 * for PoC just write '0's to the MSRs.
	 */
	msr_t msr = {0, 0};

	wrmsr(MSR_SGX_OWNEREPOCH0, msr);
	wrmsr(MSR_SGX_OWNEREPOCH1, msr);
	return 0;
}

static void activate_sgx(void)
{
	msr_t msr;

	/*
	 * Activate SGX feature by writing 1b to MSR 0x7A on all threads.
	 * BIOS must ensure bit 0 is set prior to writing to it, then read it
	 * back and verify the bit is cleared to confirm SGX activation.
	 */
	msr = rdmsr(MSR_BIOS_UPGD_TRIG);
	if (msr.lo & 0x1) {
		wrmsr(MSR_BIOS_UPGD_TRIG, (msr_t) {.lo = 0x1, .hi = 0});
		/* Read back to verify it is activated */
		msr = rdmsr(MSR_BIOS_UPGD_TRIG);
		if (msr.lo & 0x1)
			printk(BIOS_ERR, "SGX activation failed.\n");
		else
			printk(BIOS_INFO, "SGX activation was successful.\n");
	} else {
		printk(BIOS_ERR, "SGX feature is deactivated.\n");
	}
}

void configure_sgx(const void *microcode_patch)
{
	device_t dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;
	msr_t msr;

	if (!conf->sgx_enable || !is_sgx_supported())
		return;

	/* Initialize PRMRR core MSRs */
	if (configure_core_prmrr() < 0)
		return;

	/* Enable the SGX feature */
	enable_sgx();

	/* Update the owner epoch value */
	if (owner_epoch_update() < 0)
		return;

	/* Ensure to lock memory before reload microcode patch */
	msr = rdmsr(MSR_LT_LOCK_MEMORY);
	if ((msr.lo & 1) == 0) {
		msr.lo |= 1; /* Lock it */
		wrmsr(MSR_LT_LOCK_MEMORY, msr);
	}

	/* Reload the microcode patch */
	intel_microcode_load_unlocked(microcode_patch);

	/* Lock the SGX feature */
	lock_sgx();

	/* Activate the SGX feature */
	activate_sgx();
}
