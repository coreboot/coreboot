/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpi_pm.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <string.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/intel/broadwell/chip.h>

/* Save bit index for PM1_STS and GPE_STS for ACPI _SWS */
static void pm_fill_gnvs(struct global_nvs *gnvs, const struct chipset_power_state *ps)
{
	uint16_t pm1;
	int gpe_reg;

	pm1 = ps->pm1_sts & ps->pm1_en;

	/* Scan for first set bit in PM1 */
	for (gnvs->pm1i = 0; gnvs->pm1i < 16; gnvs->pm1i++) {
		if (pm1 & 1)
			break;
		pm1 >>= 1;
	}

	/* If unable to determine then return -1 */
	if (gnvs->pm1i >= 16)
		gnvs->pm1i = -1;

	/* Scan for first set bit in GPE registers */
	gnvs->gpei = -1;
	for (gpe_reg = 0; gpe_reg < GPE0_REG_MAX; gpe_reg++) {
		u32 gpe = ps->gpe0_sts[gpe_reg] & ps->gpe0_en[gpe_reg];
		int start = gpe_reg * GPE0_REG_SIZE;
		int end = start + GPE0_REG_SIZE;

		if (gpe == 0) {
			if (!gnvs->gpei)
				gnvs->gpei = end;
			continue;
		}

		for (gnvs->gpei = start; gnvs->gpei < end; gnvs->gpei++) {
			if (gpe & 1)
				break;
			gpe >>= 1;
		}
	}

	/* If unable to determine then return -1 */
	if (gnvs->gpei >= (GPE0_REG_MAX * GPE0_REG_SIZE))
		gnvs->gpei = -1;

	printk(BIOS_DEBUG, "ACPI _SWS is PM1 Index %lld GPE Index %lld\n",
	       gnvs->pm1i, gnvs->gpei);
}

static void acpi_save_wake_source(void *unused)
{
	const struct chipset_power_state *ps;
	struct global_nvs *gnvs = acpi_get_gnvs();
	if (!gnvs)
		return;

	if (acpi_pm_state_for_wake(&ps) < 0)
		return;

	pm_fill_gnvs(gnvs, ps);
}

BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, acpi_save_wake_source, NULL);
