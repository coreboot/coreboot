/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ACPI_PM_H
#define ACPI_PM_H

struct chipset_power_state;
struct chipset_power_state *acpi_get_pm_state(void);
int acpi_pm_state_for_elog(const struct chipset_power_state **ps);
int acpi_pm_state_for_rtc(const struct chipset_power_state **ps);
int acpi_pm_state_for_wake(const struct chipset_power_state **ps);

#endif
