/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <romstage_handoff.h>

/* This is filled with acpi_handoff_wakeup_s3() call early in ramstage. */
static int acpi_slp_type = -1;

static void acpi_handoff_wakeup(void)
{
	if (acpi_slp_type < 0) {
		if (romstage_handoff_is_resume()) {
			printk(BIOS_DEBUG, "S3 Resume\n");
			acpi_slp_type = ACPI_S3;
		} else {
			printk(BIOS_DEBUG, "Normal boot\n");
			acpi_slp_type = ACPI_S0;
		}
	}
}

int acpi_handoff_wakeup_s3(void)
{
	acpi_handoff_wakeup();
	return (acpi_slp_type == ACPI_S3);
}

void __weak mainboard_suspend_resume(void)
{
}
