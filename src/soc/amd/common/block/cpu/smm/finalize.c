/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/smm.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <types.h>

static void soc_finalize(void *unused)
{
	if (!acpi_is_wakeup_s3()) {
		acpi_clear_pm_gpe_status();

		if (CONFIG(HAVE_SMI_HANDLER))
			acpi_disable_sci();
		else
			acpi_enable_sci();
	}

	post_code(POST_OS_BOOT);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, soc_finalize, NULL);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, soc_finalize, NULL);
