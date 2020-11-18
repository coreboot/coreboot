/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <smbios.h>

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
