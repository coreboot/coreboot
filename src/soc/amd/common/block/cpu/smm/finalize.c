/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <bootstate.h>
#include <console/console.h>
#include <amdblocks/acpi.h>

static void per_core_finalize(void *unused)
{
	msr_t hwcr, mask;

	/* Finalize SMM settings */
	hwcr = rdmsr(HWCR_MSR);
	if (hwcr.lo & SMM_LOCK) /* Skip if already locked, avoid GPF */
		return;

	if (CONFIG(HAVE_SMI_HANDLER)) {
		mask = rdmsr(SMM_MASK_MSR);
		mask.lo |= SMM_TSEG_VALID;
		wrmsr(SMM_MASK_MSR, mask);
	}

	hwcr.lo |= SMM_LOCK;
	wrmsr(HWCR_MSR, hwcr);
}

static void finalize_cores(void)
{
	int r;
	printk(BIOS_SPEW, "Lock SMM configuration\n");

	r = mp_run_on_all_cpus(per_core_finalize, NULL);
	if (r)
		printk(BIOS_WARNING, "Failed to finalize all cores\n");
}

static void soc_finalize(void *unused)
{
	finalize_cores();

	if (!acpi_is_wakeup_s3()) {
		if (CONFIG(HAVE_SMI_HANDLER))
			acpi_disable_sci();
		else
			acpi_enable_sci();
	}

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
