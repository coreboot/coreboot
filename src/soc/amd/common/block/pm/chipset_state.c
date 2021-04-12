/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/pmlib.h>
#include <cbmem.h>
#include <string.h>

static struct chipset_power_state chipset_state;

void fill_chipset_state(void)
{
	acpi_fill_pm_gpe_state(&chipset_state.gpe_state);
	gpio_fill_wake_state(&chipset_state.gpio_state);
}

static void add_chipset_state_cbmem(int unused)
{
	struct chipset_power_state *state;

	state = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*state));

	if (state)
		memcpy(state, &chipset_state, sizeof(*state));
}

ROMSTAGE_CBMEM_INIT_HOOK(add_chipset_state_cbmem);
