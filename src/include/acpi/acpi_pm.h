/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPI_PM_H
#define ACPI_PM_H

enum power_state_claimer {
	PS_CLAIMER_ELOG,
	PS_CLAIMER_RTC,
	PS_CLAIMER_WAKE,
	PS_CLAIMER_MAX /* Not a valid value, needs to be last element */
};

struct chipset_power_state;
struct chipset_power_state *acpi_get_pm_state(void);
int acpi_fetch_pm_state(const struct chipset_power_state **ps,
		  enum power_state_claimer ps_claimer);

#endif
