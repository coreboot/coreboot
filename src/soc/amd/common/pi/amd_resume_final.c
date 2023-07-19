/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootstate.h>
#include <amdblocks/agesawrapper_call.h>

static void agesawrapper_s3finalrestore(void *unused)
{
	/* Needed since running on BS_OS_RESUME_CHECK to ensure execution before SMM lock */
	if (acpi_is_wakeup_s3())
		do_agesawrapper(AMD_S3FINAL_RESTORE, "amds3finalrestore");
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_EXIT, agesawrapper_s3finalrestore, NULL);
