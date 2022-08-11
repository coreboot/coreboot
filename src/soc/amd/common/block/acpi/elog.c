/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/xhci.h>
#include <elog.h>
#include <soc/southbridge.h>

static void elog_pm1_status(const struct acpi_pm_gpe_state *state)
{
	uint16_t pm1_sts = state->pm1_sts;

	if (pm1_sts & WAK_STS)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE, state->previous_sx_state);

	if (pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	if (pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	if (pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);
}

static void elog_gpe_events(const struct acpi_pm_gpe_state *state)
{
	int i;
	uint32_t valid_gpe = state->gpe0_sts & state->gpe0_en;

	if (!ENV_SMM)
		return;

	for (i = 0; i <= 31; i++) {
		if (valid_gpe & (1U << i)) {
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i);

			if (CONFIG(SOC_AMD_COMMON_BLOCK_XHCI_ELOG) && i == XHCI_GEVENT)
				soc_xhci_log_wake_events();
		}
	}
}

void acpi_log_events(const struct chipset_power_state *ps)
{
	elog_pm1_status(&ps->gpe_state);
	elog_gpe_events(&ps->gpe_state);
}
