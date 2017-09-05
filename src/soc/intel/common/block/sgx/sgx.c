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

#include <assert.h>
#include <console/console.h>
#include <chip.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/sgx.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>

static int is_sgx_supported(void)
{
	struct cpuid_result cpuid_regs;
	msr_t msr;

	cpuid_regs = cpuid_ext(0x7, 0x0); /* EBX[2] is feature capability */
	msr = rdmsr(MTRR_CAP_MSR); /* Bit 12 is PRMRR enablement */
	return ((cpuid_regs.ebx & SGX_SUPPORTED) && (msr.lo & PRMRR_SUPPORTED));
}

void prmrr_core_configure(void)
{
	msr_t prmrr_base;
	msr_t prmrr_mask;
	msr_t msr;
	device_t dev = SA_DEV_ROOT;
	assert(dev != NULL);
	config_t *conf = dev->chip_info;

	if (!conf) {
		printk(BIOS_ERR, "SGX: failed to get chip_info\n");
		return;
	}

	if (!conf->sgx_enable || !is_sgx_supported())
		return;

	/* PRMRR base and mask are read from the UNCORE PRMRR MSRs
	 * that are already set in FSP-M. */
	prmrr_base = rdmsr(UNCORE_PRMRR_PHYS_BASE_MSR);
	prmrr_mask = rdmsr(UNCORE_PRMRR_PHYS_MASK_MSR);
	if (!prmrr_base.lo) {
		printk(BIOS_ERR, "SGX Error: Uncore PRMRR is not set!\n");
		return;
	}

	msr = rdmsr(PRMRR_PHYS_MASK_MSR);
	/* If it is locked don't attempt to write PRMRR MSRs. */
	if (msr.lo & PRMRR_PHYS_MASK_LOCK)
		return;

	/* Program core PRMRR MSRs */
	prmrr_base.lo |= MTRR_TYPE_WRBACK; /* cache writeback mem attrib */
	wrmsr(PRMRR_PHYS_BASE_MSR, prmrr_base);
	prmrr_mask.lo &= ~PRMRR_PHYS_MASK_VALID; /* Do not set the valid bit */
	prmrr_mask.lo |= PRMRR_PHYS_MASK_LOCK; /* Lock it */
	wrmsr(PRMRR_PHYS_MASK_MSR, prmrr_mask);
}

static int is_prmrr_set(void)
{
	msr_t prmrr_base, prmrr_mask;
	prmrr_base = rdmsr(PRMRR_PHYS_BASE_MSR);
	prmrr_mask = rdmsr(PRMRR_PHYS_MASK_MSR);

	/* If PRMRR base is zero and PRMRR mask is locked
	 * then PRMRR is not set */
	if ((prmrr_base.hi == 0) && (prmrr_base.lo == 0)
		&& (prmrr_mask.lo & PRMRR_PHYS_MASK_LOCK))
		return 0;
	return 1;
}

static void enable_sgx(void)
{
	msr_t msr;

	msr = rdmsr(IA32_FEATURE_CONTROL);
	/* Only enable it when it is not locked */
	if ((msr.lo & FEATURE_CONTROL_LOCK) == 0) {
		msr.lo |= SGX_GLOBAL_ENABLE; /* Enable it */
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
	/* TODO - the Owner Epoch update mechanism is not determined yet,
	 * for PoC just write '0's to the MSRs. */
	msr_t msr = {0, 0};

	wrmsr(MSR_SGX_OWNEREPOCH0, msr);
	wrmsr(MSR_SGX_OWNEREPOCH1, msr);
	return 0;
}

static void activate_sgx(void)
{
	msr_t msr;

	/* Activate SGX feature by writing 1b to MSR 0x7A on all threads.
	 * BIOS must ensure bit 0 is set prior to writing to it, then read it
	 * back and verify the bit is cleared to confirm SGX activation. */
	msr = rdmsr(MSR_BIOS_UPGD_TRIG);
	if (msr.lo & SGX_ACTIVATE_BIT) {
		wrmsr(MSR_BIOS_UPGD_TRIG,
			(msr_t) {.lo = SGX_ACTIVATE_BIT, .hi = 0});
		/* Read back to verify it is activated */
		msr = rdmsr(MSR_BIOS_UPGD_TRIG);
		if (msr.lo & SGX_ACTIVATE_BIT)
			printk(BIOS_ERR, "SGX activation failed.\n");
		else
			printk(BIOS_INFO, "SGX activation was successful.\n");
	} else {
		printk(BIOS_ERR, "SGX feature is deactivated.\n");
	}
}

static int is_prmrr_approved(void)
{
	msr_t msr;
	msr = rdmsr(PRMRR_PHYS_MASK_MSR);
	if (msr.lo & PRMRR_PHYS_MASK_VALID) {
		printk(BIOS_INFO, "SGX: MCHECK aprroved SGX PRMRR\n");
		return 1;
	}

	printk(BIOS_INFO, "SGX: MCHECK did not aprrove SGX PRMRR\n");
	return 0;
}

void sgx_configure(void)
{
	device_t dev = SA_DEV_ROOT;
	assert(dev != NULL);
	config_t *conf = dev->chip_info;
	const void *microcode_patch = intel_mp_current_microcode();

	if (!conf) {
		printk(BIOS_ERR, "SGX: failed to get chip_info\n");
		return;
	}

	if (!conf->sgx_enable || !is_sgx_supported() || !is_prmrr_set()) {
		printk(BIOS_ERR, "SGX: pre-conditions not met\n");
		return;
	}

	/* Enable the SGX feature */
	enable_sgx();

	/* Update the owner epoch value */
	if (owner_epoch_update() < 0)
		return;

	/* Ensure to lock memory before reload microcode patch */
	cpu_lock_sgx_memory();

	/* Reload the microcode patch */
	intel_microcode_load_unlocked(microcode_patch);

	/* Lock the SGX feature */
	lock_sgx();

	/* Activate the SGX feature, if PRMRR config was aprroved by MCHECK */
	if (is_prmrr_approved())
		activate_sgx();
}
