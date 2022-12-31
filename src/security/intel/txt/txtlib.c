/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <security/intel/txt/txt.h>
#include <security/tpm/tis.h>
#include <timer.h>

#include "txtlib.h"
#include "txt_register.h"

bool is_establishment_bit_asserted(void)
{
	struct stopwatch timer;
	uint8_t access;

	/* Spec says no less than 30 milliseconds */
	stopwatch_init_msecs_expire(&timer, 50);

	while (true) {
		access = read8((void *)TPM_ACCESS_REG);

		/* Register returns all ones if TPM is missing */
		if (access == 0xff)
			return false;

		if (access & TPM_ACCESS_VALID)
			break;

		/* On timeout, assume that the TPM is not working */
		if (stopwatch_expired(&timer))
			return false;
	}

	/* This bit uses inverted logic: if cleared, establishment is asserted */
	return !(access & TPM_ACCESS_ESTABLISHMENT);
}

bool is_txt_cpu(void)
{
	const uint32_t ecx = cpu_get_feature_flags_ecx();

	return (ecx & (CPUID_SMX | CPUID_VMX)) == (CPUID_SMX | CPUID_VMX);
}

static void unlock_txt_memory(void)
{
	msr_t msrval = {0};

	wrmsr(IA32_LT_UNLOCK_MEMORY, msrval);
}

void disable_intel_txt(void)
{
	/* Return if the CPU doesn't support TXT */
	if (!is_txt_cpu()) {
		printk(BIOS_DEBUG, "Abort disabling TXT, as CPU is not TXT capable.\n");
		return;
	}

	/*
	 * Memory is supposed to be locked if system is TXT capable
	 * As per TXT BIOS spec Section 6.2.5 unlock memory
	 * when security (TPM) is set and TXT is not enabled.
	 */
	if (!is_establishment_bit_asserted()) {
		unlock_txt_memory();
		printk(BIOS_INFO, "TXT disabled successfully - Unlocked memory\n");
	}
}
