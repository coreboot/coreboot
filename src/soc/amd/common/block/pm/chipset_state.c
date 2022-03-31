/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/gpio.h>
#include <amdblocks/pmlib.h>
#include <cbmem.h>
#include <elog.h>
#include <string.h>

static struct chipset_power_state chipset_state;

void fill_chipset_state(void)
{
	acpi_fill_pm_gpe_state(&chipset_state.gpe_state);
	gpio_fill_wake_state(&chipset_state.gpio_state);
}

void elog_gsmi_cb_platform_log_wake_source(void)
{
	fill_chipset_state();
	acpi_log_events(&chipset_state);
}

static void add_chipset_state_cbmem(int unused)
{
	struct chipset_power_state *state;

	state = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*state));

	if (state)
		memcpy(state, &chipset_state, sizeof(*state));
}

CBMEM_CREATION_HOOK(add_chipset_state_cbmem);
