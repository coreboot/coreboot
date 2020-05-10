/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <stddef.h>
#include <bootstate.h>
#include <security/tpm/tspi.h>

#if CONFIG(HAVE_ACPI_RESUME)
#include <acpi/acpi.h>
#endif

static void init_tpm_dev(void *unused)
{
#if CONFIG(HAVE_ACPI_RESUME)
	int s3resume = acpi_is_wakeup_s3();
	tpm_setup(s3resume);
#else
	/* This can lead to PCR reset attacks but currently there
	   is no generic way to detect resume on other platforms. */
	tpm_setup(false);
#endif
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, init_tpm_dev, NULL);
