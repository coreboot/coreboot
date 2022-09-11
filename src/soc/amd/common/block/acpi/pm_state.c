/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <amdblocks/acpi.h>
#include <bootstate.h>
#include <soc/acpi.h>
#include <soc/nvs.h>
#include <soc/southbridge.h>
#include <types.h>

static int get_index_bit(uint32_t value, uint16_t limit)
{
	uint16_t i;
	uint32_t t;

	if (limit > TOTAL_BITS(uint32_t))
		return -1;

	/* get a mask of valid bits. Ex limit = 3, set bits 0-2 */
	t = (1ULL << limit) - 1;
	if ((value & t) == 0)
		return -1;
	t = 1;
	for (i = 0; i < limit; i++) {
		if (value & t)
			break;
		t <<= 1;
	}
	return i;
}

static void pm_fill_gnvs(struct global_nvs *gnvs, const struct acpi_pm_gpe_state *state)
{
	int index;

	index = get_index_bit(state->pm1_sts & state->pm1_en, PM1_LIMIT);
	if (index >= 0)
		gnvs->pm1i = index;

	index = get_index_bit(state->gpe0_sts & state->gpe0_en, GPE0_LIMIT);
	if (index >= 0)
		gnvs->gpei = index;
}

static void acpi_save_wake_source(void *unused)
{
	const struct chipset_power_state *ps;
	struct global_nvs *gnvs;

	if (acpi_reset_gnvs_for_wake(&gnvs) < 0)
		return;
	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_WAKE) < 0)
		return;

	pm_fill_gnvs(gnvs, &ps->gpe_state);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, acpi_save_wake_source, NULL);
