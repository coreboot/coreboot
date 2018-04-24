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
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <intelblocks/sgx.h>
#include <intelblocks/systemagent.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <string.h>

static bool sgx_param_valid;
static struct sgx_param g_sgx_param;

static inline uint64_t sgx_resource(uint32_t low, uint32_t high)
{
	uint64_t val;
	val = (uint64_t)(high & SGX_RESOURCE_MASK_HI) << 32;
	val |= low & SGX_RESOURCE_MASK_LO;
	return val;
}

static const struct sgx_param *get_sgx_param(void)
{
	if (sgx_param_valid)
		return &g_sgx_param;

	memset(&g_sgx_param, 0, sizeof(g_sgx_param));
	if (soc_fill_sgx_param(&g_sgx_param) < 0) {
		printk(BIOS_ERR, "SGX : Failed to get soc sgx param\n");
		return NULL;
	}
	sgx_param_valid = true;
	printk(BIOS_INFO, "SGX : param.enable = %d\n", g_sgx_param.enable);

	return &g_sgx_param;
}

static int soc_sgx_enabled(void)
{
	const struct sgx_param *sgx_param = get_sgx_param();
	return sgx_param ? sgx_param->enable : 0;
}

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
	union {
		uint64_t data64;
		struct {
			uint32_t lo;
			uint32_t hi;
		} data32;
	} prmrr_base, prmrr_mask;
	msr_t msr;

	if (!soc_sgx_enabled() || !is_sgx_supported())
		return;

	msr = rdmsr(PRMRR_PHYS_MASK_MSR);
	/* If it is locked don't attempt to write PRMRR MSRs. */
	if (msr.lo & PRMRR_PHYS_MASK_LOCK)
		return;

	/* PRMRR base and mask are read from the UNCORE PRMRR MSRs
	 * that are already set in FSP-M. */
	if (soc_get_uncore_prmmr_base_and_mask(&prmrr_base.data64,
						&prmrr_mask.data64) < 0) {
		printk(BIOS_ERR, "SGX: Failed to get PRMRR base and mask\n");
		return;
	}

	if (!prmrr_base.data32.lo) {
		printk(BIOS_ERR, "SGX Error: Uncore PRMRR is not set!\n");
		return;
	}

	printk(BIOS_INFO, "SGX: prmrr_base = 0x%llx", prmrr_base.data64);
	printk(BIOS_INFO, "SGX: prmrr_mask = 0x%llx", prmrr_mask.data64);

	/* Program core PRMRR MSRs.
	 * - Set cache writeback mem attrib in PRMRR base MSR
	 * - Clear the valid bit in PRMRR mask MSR
	 * - Lock PRMRR MASK MSR */
	prmrr_base.data32.lo |= MTRR_TYPE_WRBACK;
	wrmsr(PRMRR_PHYS_BASE_MSR, (msr_t) {.lo = prmrr_base.data32.lo,
					.hi = prmrr_base.data32.hi});
	prmrr_mask.data32.lo &= ~PRMRR_PHYS_MASK_VALID;
	prmrr_mask.data32.lo |= PRMRR_PHYS_MASK_LOCK;
	wrmsr(PRMRR_PHYS_MASK_MSR, (msr_t) {.lo = prmrr_mask.data32.lo,
					.hi = prmrr_mask.data32.hi});
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
		printk(BIOS_INFO, "SGX: MCHECK approved SGX PRMRR\n");
		return 1;
	}

	printk(BIOS_INFO, "SGX: MCHECK did not approve SGX PRMRR\n");
	return 0;
}

void sgx_configure(void *unused)
{
	const void *microcode_patch = intel_mp_current_microcode();

	if (!soc_sgx_enabled() || !is_sgx_supported() || !is_prmrr_set()) {
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

	/* Activate the SGX feature, if PRMRR config was approved by MCHECK */
	if (is_prmrr_approved())
		activate_sgx();
}

void sgx_fill_gnvs(global_nvs_t *gnvs)
{
	struct cpuid_result cpuid_regs;

	if (!soc_sgx_enabled() || !is_sgx_supported()) {
		printk(BIOS_DEBUG,
			"SGX: not enabled or not supported. skip gnvs fill\n");
		return;
	}

	/* Get EPC base and size.
	 * Intel SDM: Table 36-6. CPUID Leaf 12H, Sub-Leaf Index 2 or
	 * Higher for enumeration of SGX Resources. Same Table mentions
	 * about return values of the CPUID */
	cpuid_regs = cpuid_ext(SGX_RESOURCE_ENUM_CPUID_LEAF,
				SGX_RESOURCE_ENUM_CPUID_SUBLEAF);

	if (cpuid_regs.eax & SGX_RESOURCE_ENUM_BIT) {
		/* EPC section enumerated */
		gnvs->ecps = 1;
		gnvs->emna = sgx_resource(cpuid_regs.eax, cpuid_regs.ebx);
		gnvs->elng = sgx_resource(cpuid_regs.ecx, cpuid_regs.edx);
	}

	printk(BIOS_DEBUG,
		"SGX: gnvs ECP status = %d base = 0x%llx len = 0x%llx\n",
			gnvs->ecps, gnvs->emna, gnvs->elng);
}
