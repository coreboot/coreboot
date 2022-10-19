/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/common/common.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/msr.h>
#include <intelblocks/sgx.h>
#include <intelblocks/systemagent.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>

int is_sgx_supported(void)
{
	struct cpuid_result cpuid_regs;
	msr_t msr;

	cpuid_regs = cpuid_ext(0x7, 0x0); /* EBX[2] is feature capability */
	msr = rdmsr(MTRR_CAP_MSR); /* Bit 12 is PRMRR enablement */
	return ((cpuid_regs.ebx & SGX_SUPPORTED) && (msr.lo & MTRR_CAP_PRMRR));
}

void prmrr_core_configure(void)
{
	msr_t prmrr_base, prmrr_mask;

	/*
	 * Software Developer's Manual Volume 4:
	 * Order Number: 335592-068US
	 * Chapter 2.16.1
	 * MSR_PRMRR_PHYS_MASK is in scope "Core"
	 * MSR_PRMRR_PHYS_BASE is in scope "Core"
	 * Return if Hyper-Threading is enabled and not thread 0
	 */
	if (!is_sgx_supported() || intel_ht_sibling())
		return;

	/* PRMRR_PHYS_MASK is in scope "Core" */
	prmrr_mask = rdmsr(MSR_PRMRR_PHYS_MASK);
	/* If it is locked don't attempt to write PRMRR MSRs. */
	if (prmrr_mask.lo & PRMRR_PHYS_MASK_LOCK)
		return;

	/* PRMRR base and mask are read from the UNCORE PRMRR MSRs
	 * that are already set in FSP-M. */
	if (soc_get_uncore_prmmr_base_and_mask(&prmrr_base.raw,
						&prmrr_mask.raw) < 0) {
		printk(BIOS_ERR, "SGX: Failed to get PRMRR base and mask\n");
		return;
	}

	if (!prmrr_base.lo) {
		printk(BIOS_ERR, "SGX Error: Uncore PRMRR is not set!\n");
		return;
	}

	printk(BIOS_INFO, "SGX: prmrr_base = 0x%llx\n", prmrr_base.raw);
	printk(BIOS_INFO, "SGX: prmrr_mask = 0x%llx\n", prmrr_mask.raw);

	/* Program core PRMRR MSRs.
	 * - Set cache writeback mem attrib in PRMRR base MSR
	 * - Clear the valid bit in PRMRR mask MSR
	 * - Lock PRMRR MASK MSR */
	prmrr_base.lo |= MTRR_TYPE_WRBACK;
	wrmsr(MSR_PRMRR_PHYS_BASE, prmrr_base);
	prmrr_mask.lo &= ~PRMRR_PHYS_MASK_VALID;
	prmrr_mask.lo |= PRMRR_PHYS_MASK_LOCK;
	wrmsr(MSR_PRMRR_PHYS_MASK, prmrr_mask);
}

static int is_prmrr_set(void)
{
	msr_t prmrr_base, prmrr_mask;
	prmrr_base = rdmsr(MSR_PRMRR_PHYS_BASE);
	prmrr_mask = rdmsr(MSR_PRMRR_PHYS_MASK);

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

	/*
	 * Intel 64 and IA-32 ArchitecturesSoftware Developer's ManualVolume 3C
	 * Order Number:  326019-060US
	 * Chapter 35.10.2 "Additional MSRs Supported by Intel"
	 * IA32_FEATURE_CONTROL is in scope "Thread"
	 */
	msr = rdmsr(IA32_FEATURE_CONTROL);
	/* Only enable it when it is not locked */
	if ((msr.lo & FEATURE_CONTROL_LOCK_BIT) == 0) {
		msr.lo |= SGX_GLOBAL_ENABLE; /* Enable it */
		wrmsr(IA32_FEATURE_CONTROL, msr);
	}
}

static void lock_sgx(void)
{
	msr_t msr;

	/*
	 * Intel 64 and IA-32 ArchitecturesSoftware Developer's ManualVolume 3C
	 * Order Number:  326019-060US
	 * Chapter 35.10.2 "Additional MSRs Supported by Intel"
	 * IA32_FEATURE_CONTROL is in scope "Thread"
	 */
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
	msr_t msr = { .raw = 0 };

	/* SGX_OWNEREPOCH is in scope "Package" */
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
	msr = rdmsr(MSR_PRMRR_PHYS_MASK);
	if (msr.lo & PRMRR_PHYS_MASK_VALID) {
		printk(BIOS_INFO, "SGX: MCHECK approved SGX PRMRR\n");
		return 1;
	}

	printk(BIOS_INFO, "SGX: MCHECK did not approve SGX PRMRR\n");
	return 0;
}

/*
 * Configures SGX according to "Intel Software Guard Extensions Technology"
 * Document Number: 565432
 */
void sgx_configure(void *unused)
{

	if (!is_sgx_supported() || !is_prmrr_set()) {
		printk(BIOS_ERR, "SGX: not supported or pre-conditions not met\n");
		return;
	}

	/* Enable the SGX feature on all threads. */
	enable_sgx();

	/* Update the owner epoch value */
	if (owner_epoch_update() < 0)
		return;

	/* Ensure to lock memory before reloading microcode patch */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_LOCK_MEMORY))
		cpu_lt_lock_memory();

	/*
	 * Update just on the first CPU in the core. Other siblings
	 * get the update automatically according to Document: 253668-060US
	 * Intel SDM Chapter 9.11.6.3
	 * "Update in a System Supporting Intel Hyper-Threading Technology"
	 * Intel Hyper-Threading Technology has implications on the loading of the
	 * microcode update. The update must be loaded for each core in a physical
	 * processor. Thus, for a processor supporting Intel Hyper-Threading
	 * Technology, only one logical processor per core is required to load the
	 * microcode update. Each individual logical processor can independently
	 * load the update. However, MP initialization must provide some mechanism
	 * (e.g. a software semaphore) to force serialization of microcode update
	 * loads and to prevent simultaneous load attempts to the same core.
	 */
	if (!intel_ht_sibling()) {
		const void *microcode_patch = intel_microcode_find();
		intel_microcode_load_unlocked(microcode_patch);
	}

	/* Lock the SGX feature on all threads. */
	lock_sgx();

	/* Activate the SGX feature, if PRMRR config was approved by MCHECK */
	if (is_prmrr_approved())
		activate_sgx();
}
