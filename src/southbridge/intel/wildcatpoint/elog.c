/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <bootstate.h>
#include <stdint.h>
#include <elog.h>
#include <soc/lpc.h>
#include <soc/pm.h>

static void pch_log_gpio_gpe(u32 gpe0_sts, u32 gpe0_en, int start)
{
	int i;

	gpe0_sts &= gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i + start);
	}
}

static void pch_log_wake_source(const struct chipset_power_state *ps)
{
	/* Power Button */
	if (ps->pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	/* RTC */
	if (ps->pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	/* PCI Express (TODO: determine wake device) */
	if (ps->pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	/* PME (TODO: determine wake device) */
	if (ps->gpe0_sts[GPE_STD] & PME_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* Internal PME (TODO: determine wake device) */
	if (ps->gpe0_sts[GPE_STD] & PME_B0_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);

	/* SMBUS Wake */
	if (ps->gpe0_sts[GPE_STD] & SMB_WAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_SMBUS, 0);

	/* GPIO27 */
	if (ps->gpe0_sts[GPE_STD] & GP27_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, 27);

	/* Log GPIO events in set 1-3 */
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_31_0], ps->gpe0_en[GPE_31_0], 0);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_63_32], ps->gpe0_en[GPE_63_32], 32);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_94_64], ps->gpe0_en[GPE_94_64], 64);
}

static void pch_log_power_and_resets(const struct chipset_power_state *ps)
{
	/* Thermal Trip Status */
	if (ps->gen_pmcon2 & THERMTRIP_STS)
		elog_add_event(ELOG_TYPE_THERM_TRIP);

	/* PWR_FLR Power Failure */
	if (ps->gen_pmcon2 & PWROK_FLR)
		elog_add_event(ELOG_TYPE_POWER_FAIL);

	/* SUS Well Power Failure */
	if (ps->gen_pmcon3 & SUS_PWR_FLR)
		elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);

	/* SYS_PWROK Failure */
	if (ps->gen_pmcon2 & SYSPWR_FLR)
		elog_add_event(ELOG_TYPE_SYS_PWROK_FAIL);

	/* PWROK Failure */
	if (ps->gen_pmcon2 & PWROK_FLR)
		elog_add_event(ELOG_TYPE_PWROK_FAIL);

	/* TCO Timeout */
	if (ps->prev_sleep_state != ACPI_S3 &&
	    ps->tco2_sts & TCO2_STS_SECOND_TO)
		elog_add_event(ELOG_TYPE_TCO_RESET);

	/* Power Button Override */
	if (ps->pm1_sts & PRBTNOR_STS)
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

	/* RTC reset */
	if (ps->gen_pmcon3 & RTC_BATTERY_DEAD)
		elog_add_event(ELOG_TYPE_RTC_RESET);

	/* System Reset Status (reset button pushed) */
	if (ps->gen_pmcon2 & SYSTEM_RESET_STS)
		elog_add_event(ELOG_TYPE_RESET_BUTTON);

	/* General Reset Status */
	if (ps->gen_pmcon3 & GEN_RST_STS)
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);

	/* ACPI Wake Event */
	if (ps->prev_sleep_state != ACPI_S0)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE, ps->prev_sleep_state);
}

static void pch_log_state(void *unused)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_ELOG) < 0)
		return;

	/* Power and Reset */
	pch_log_power_and_resets(ps);

	/* Wake Sources */
	pch_log_wake_source(ps);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, pch_log_state, NULL);
