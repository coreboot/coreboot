/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_pm.h>
#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <smbios.h>

static const char *pm_fetch_failure_msg[PS_CLAIMER_MAX] = {
	[PS_CLAIMER_ELOG]	= "no event recorded in ELOG.",
	[PS_CLAIMER_RTC]	= "RTC init aborted.",
	[PS_CLAIMER_WAKE]	= "wake source unknown.",
};

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

int acpi_fetch_pm_state(const struct chipset_power_state **ps,
			enum power_state_claimer ps_claimer)
{
	assert(ps_claimer < PS_CLAIMER_MAX);
	*ps = acpi_get_pm_state();

	if (!*ps) {
		printk(BIOS_ERR, "No CBMEM_ID_POWER_STATE entry, %s\n",
		       ps_claimer < PS_CLAIMER_MAX ?
		       pm_fetch_failure_msg[ps_claimer] : "unknown claimer.");
		return -1;
	}
	return 0;
}
