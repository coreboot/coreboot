/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/common/common.h>
#include <cpu/x86/cr.h>
#include <device/mmio.h>
#include <southbridge/intel/common/pmbase.h>
#include <types.h>

#include "txt.h"
#include "txtlib.h"
#include "txt_register.h"
#include "txt_getsec.h"

static bool is_txt_chipset(void)
{
	uint32_t eax;

	const bool success = getsec_capabilities(&eax);

	return success && eax & 1;
}

/* Print the bad news */
static void print_memory_is_locked(void)
{
	if (!CONFIG(INTEL_TXT_LOGGING))
		return;

	printk(BIOS_EMERG, "FATAL: Cannot run SCLEAN. Memory will remain locked.\n");
	printk(BIOS_EMERG, "\n");
	printk(BIOS_EMERG, "If you still want to boot, your options are:\n");
	printk(BIOS_EMERG, "\n");
	printk(BIOS_EMERG, "   1. Flash a coreboot image with a valid BIOS ACM.\n");
	printk(BIOS_EMERG, "      Then, try again and hope it works this time.\n");
	printk(BIOS_EMERG, "\n");
	printk(BIOS_EMERG, "   2. If possible, remove the TPM from the system.\n");
	printk(BIOS_EMERG, "      Reinstalling the TPM might lock memory again.\n");
	printk(BIOS_EMERG, "\n");
	printk(BIOS_EMERG, "   3. Disconnect all power sources, and RTC battery.\n");
	printk(BIOS_EMERG, "      This may not work on all TXT-enabled platforms.\n");
	printk(BIOS_EMERG, "\n");
}

void intel_txt_romstage_init(void)
{
	/* Bail early if the CPU doesn't support TXT */
	if (!is_txt_cpu()) {
		printk(BIOS_ERR, "TEE-TXT: CPU not TXT capable.\n");
		return;
	}

	/*
	 * We need to use GETSEC here, so enable it.
	 * CR4_SMXE is all we need to be able to call GETSEC[CAPABILITIES]
	 * or GETSEC[ENTERACCS] for SCLEAN.
	 */
	write_cr4(read_cr4() | CR4_SMXE);

	if (!is_txt_chipset()) {
		printk(BIOS_ERR, "TEE-TXT: Chipset not TXT capable.\n");
		return;
	}

	const uint8_t txt_ests = read8((void *)TXT_ESTS);

	const bool establishment = is_establishment_bit_asserted();
	const bool is_wake_error = !!(txt_ests & TXT_ESTS_WAKE_ERROR_STS);

	if (CONFIG(INTEL_TXT_LOGGING)) {

		printk(BIOS_INFO, "TEE-TXT: TPM established: %s\n",
		       establishment ? "true" : "false");
	}

	if (establishment && is_wake_error) {

		printk(BIOS_ERR, "TEE-TXT: Secrets remain in memory. SCLEAN is required.\n");

		if (txt_ests & TXT_ESTS_TXT_RESET_STS) {
			printk(BIOS_ERR, "TEE-TXT: TXT_RESET bit set, doing global reset!\n");
			txt_reset_platform();
		}

		/* FIXME: Clear SLP_TYP# */
		write_pmbase32(4, read_pmbase32(4) & ~(0x7 << 10));

		intel_txt_run_sclean();

		/* If running the BIOS ACM is impossible, manual intervention is required */
		print_memory_is_locked();

		/* FIXME: vboot A/B could be used to recover, but has not been tested */
		die("Could not execute BIOS ACM to unlock the memory.\n");
	}
}
