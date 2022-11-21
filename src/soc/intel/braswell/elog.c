/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_pm.h>
#include <device/device.h>
#include <device/pci.h>
#include <elog.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <stdint.h>

static void log_power_and_resets(const struct chipset_power_state *ps)
{
	if (ps->gen_pmcon1 & PWR_FLR) {
		elog_add_event(ELOG_TYPE_POWER_FAIL);
		elog_add_event(ELOG_TYPE_PWROK_FAIL);
	}

	if (ps->gen_pmcon1 & SUS_PWR_FLR)
		elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);

	if (ps->gen_pmcon1 & RPS)
		elog_add_event(ELOG_TYPE_RTC_RESET);

	if (ps->tco_sts & TCO1_32_STS_SECOND_TO_STS)
		elog_add_event(ELOG_TYPE_TCO_RESET);

	if (ps->pm1_sts & PRBTNOR_STS)
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

	if (ps->gen_pmcon1 & SRS)
		elog_add_event(ELOG_TYPE_RESET_BUTTON);

	if (ps->gen_pmcon1 & GEN_RST_STS)
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);
}

static void log_wake_events(const struct chipset_power_state *ps)
{
	const uint32_t pcie_wake_mask = PCIE_WAKE3_STS | PCIE_WAKE2_STS |
					PCIE_WAKE1_STS | PCIE_WAKE0_STS | PCI_EXP_STS;

	uint32_t gpe0_sts;
	uint32_t gpio_mask;
	int i;

	/* Mask off disabled events. */
	gpe0_sts = ps->gpe0_sts & ps->gpe0_en;

	if (ps->pm1_sts & WAK_STS)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
				    acpi_is_wakeup_s3() ? ACPI_S3 : ACPI_S5);

	if (ps->pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	if (ps->pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	if (gpe0_sts & PME_B0_EN)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);

	if (gpe0_sts & pcie_wake_mask)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	gpio_mask = SUS_GPIO_STS0;
	i = 0;
	while (gpio_mask) {
		if (gpio_mask & gpe0_sts)
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i);
		gpio_mask <<= 1;
		i++;
	}
}

void southcluster_log_state(void)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_ELOG) < 0)
		return;

	log_power_and_resets(ps);
	log_wake_events(ps);
}
