/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <device/pci_type.h>
#include <elog.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xhci.h>
#include <soc/pm.h>
#include <soc/pci_devs.h>
#include <soc/smbus.h>
#include <stdint.h>

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
	const struct xhci_wake_info xhci_wake_info[] = {
		{ PCH_DEVFN_XHCI, ELOG_WAKE_SOURCE_PME_XHCI },
	};

	/* Power Button */
	if (ps->pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	/* RTC */
	if (ps->pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	/* PCI Express (TODO: determine wake device) */
	if (ps->pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	/* PME */
	if (ps->gpe0_sts[GPE0_A] & CSE_PME_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* XHCI */
	if (ps->gpe0_sts[GPE0_A] & XHCI_PME_STS)
		xhci_update_wake_event(xhci_wake_info,
				       ARRAY_SIZE(xhci_wake_info));

	/* SMBUS Wake */
	if (ps->gpe0_sts[GPE0_A] & SMB_WAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_SMBUS, 0);

	/* ACPI Wake Event - Log prev sleep state only if it was not S0. */
	if (ps->prev_sleep_state != ACPI_S0)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE, ps->prev_sleep_state);

	/* Log GPIO events in set A-D */
	pch_log_gpio_gpe(ps->gpe0_sts[GPE0_A], ps->gpe0_en[GPE0_A], 0);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE0_B], ps->gpe0_en[GPE0_B], 32);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE0_C], ps->gpe0_en[GPE0_C], 64);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE0_D], ps->gpe0_en[GPE0_D], 96);
}

static void pch_log_power_and_resets(const struct chipset_power_state *ps)
{
	/* RTC Reset */
	if (ps->gen_pmcon1 & RPS)
		elog_add_event(ELOG_TYPE_RTC_RESET);

	/* System Reset */
	if (ps->gen_pmcon1 & WARM_RESET_STS)
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);

	/* TCO Timeout */
	if (ps->prev_sleep_state != ACPI_S3 &&
	    ps->tco1_sts & TCO_TIMEOUT)
		elog_add_event(ELOG_TYPE_TCO_RESET);

	/* Power Button Override */
	if (ps->pm1_sts & PRBTNOR_STS)
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

}

void pch_log_state(void)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_ELOG) < 0)
		return;

	/* Power and Reset */
	pch_log_power_and_resets(ps);

	/* Wake Sources */
	if (ps->prev_sleep_state > ACPI_S0)
		pch_log_wake_source(ps);
}

void elog_gsmi_cb_platform_log_wake_source(void)
{
	struct chipset_power_state ps;

	pmc_fill_pm_reg_info(&ps);
	pch_log_wake_source(&ps);
}
