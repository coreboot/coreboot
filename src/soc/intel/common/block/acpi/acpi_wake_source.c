/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/acpi_wake_source.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <stdint.h>

/* Save wake source data for ACPI _SWS methods in NVS */
static void pm_fill_gnvs(struct global_nvs *gnvs, const struct chipset_power_state *ps)
{
	uint32_t pm1, *gpe0;
	int index, gpe_reg, gpe_reg_count;
	int reg_size = sizeof(uint32_t) * 8;

	gpe_reg_count = soc_fill_acpi_wake(ps, &pm1, &gpe0);
	if (gpe_reg_count < 0)
		return;

	/* Scan for first set bit in PM1 */
	for (index = 0; index < reg_size; index++) {
		if (pm1 & 1)
			break;
		pm1 >>= 1;
	}

	if (index < 16)
		gnvs->pm1i = index;

	/* Scan for first set bit in GPE registers */
	for (gpe_reg = 0; gpe_reg < gpe_reg_count; gpe_reg++) {
		uint32_t gpe = gpe0[gpe_reg];
		int start = gpe_reg * reg_size;
		int end = start + reg_size;

		for (index = start; index < end; index++) {
			if (gpe & 1)
				break;
			gpe >>= 1;
		}
	}

	if (index < gpe_reg_count * reg_size)
		gnvs->gpei = index;
}

static void acpi_save_wake_source(void *unused)
{
	const struct chipset_power_state *ps;
	struct global_nvs *gnvs;

	if (acpi_reset_gnvs_for_wake(&gnvs) < 0)
		return;
	if (acpi_pm_state_for_wake(&ps) < 0)
		return;

	pm_fill_gnvs(gnvs, ps);

	printk(BIOS_DEBUG, "ACPI _SWS is PM1 Index %lld GPE Index %lld\n",
	       (long long)gnvs->pm1i, (long long)gnvs->gpei);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, acpi_save_wake_source, NULL);
