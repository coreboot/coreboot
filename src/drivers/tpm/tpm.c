/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <security/tpm/tspi.h>

static void init_tpm_dev(void *unused)
{
	int s3resume = acpi_is_wakeup_s3();
	tpm_setup(s3resume);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, init_tpm_dev, NULL);
