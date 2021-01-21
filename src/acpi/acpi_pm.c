/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_pm.h>
#include <cbmem.h>
#include <console/console.h>
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

struct chipset_power_state *acpi_get_pm_state(void)
{
	static struct chipset_power_state *acpi_pm_state;
	if (acpi_pm_state)
		return acpi_pm_state;

	acpi_pm_state = cbmem_find(CBMEM_ID_POWER_STATE);
	return acpi_pm_state;
}

int acpi_pm_state_for_elog(const struct chipset_power_state **ps)
{
	*ps = acpi_get_pm_state();
	if (!*ps) {
		printk(BIOS_ERR, "No CBMEM_ID_POWER_STATE entry, no event recorded in ELOG.\n");
		return -1;
	}
	return 0;
}

int acpi_pm_state_for_rtc(const struct chipset_power_state **ps)
{
	*ps = acpi_get_pm_state();
	if (!*ps) {
		printk(BIOS_ERR, "No CBMEM_ID_POWER_STATE entry, RTC init aborted.\n");
		return -1;
	}
	return 0;
}

int acpi_pm_state_for_wake(const struct chipset_power_state **ps)
{
	*ps = acpi_get_pm_state();
	if (!*ps) {
		printk(BIOS_ERR, "No CBMEM_ID_POWER_STATE entry, wake source unknown.\n");
		return -1;
	}
	return 0;
}
