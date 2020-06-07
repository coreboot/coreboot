/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <romstage_handoff.h>
#include <smbios.h>

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

/* Default mapping to ACPI FADT preferred_pm_profile field. */
uint8_t acpi_get_preferred_pm_profile(void)
{
	switch (smbios_mainboard_enclosure_type()) {
	case SMBIOS_ENCLOSURE_LAPTOP:
	case SMBIOS_ENCLOSURE_CONVERTIBLE:
		return PM_MOBILE;
	case SMBIOS_ENCLOSURE_DETACHABLE:
	case SMBIOS_ENCLOSURE_TABLET:
		return PM_TABLET;
	case SMBIOS_ENCLOSURE_DESKTOP:
		return PM_DESKTOP;
	case SMBIOS_ENCLOSURE_UNKNOWN:
	default:
		return PM_UNSPECIFIED;
	}
}
