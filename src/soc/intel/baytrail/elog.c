/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <arch/acpi.h>
#include <stdint.h>
#include <console/console.h>
#include <cbmem.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <elog.h>
#include <baytrail/iomap.h>
#include <baytrail/pmc.h>

static void log_power_and_resets(const struct chipset_power_state *ps)
{
	if (ps->gen_pmcon1 & PWR_FLR) {
		elog_add_event(ELOG_TYPE_POWER_FAIL);
		elog_add_event(ELOG_TYPE_PWROK_FAIL);
	}

	if (ps->gen_pmcon1 & SUS_PWR_FLR) {
		elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);
	}

	if (ps->tco_sts & SECOND_TO_STS) {
		elog_add_event(ELOG_TYPE_TCO_RESET);
	}

	if (ps->pm1_sts & PRBTNOR_STS) {
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);
	}

	if (ps->gen_pmcon1 & SRS) {
		elog_add_event(ELOG_TYPE_RESET_BUTTON);
	}

	if (ps->gen_pmcon1 & GEN_RST_STS) {
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);
	}
}

static void log_wake_events(const struct chipset_power_state *ps)
{
	const uint32_t pcie_wake_mask = PCI_EXP_STS | PCIE_WAKE3_STS |
					PCIE_WAKE2_STS | PCIE_WAKE1_STS |
					PCIE_WAKE0_STS;
	uint32_t gpe0_sts;
	uint32_t gpio_mask;
	int i;

	/* Mask off disabled events. */
	gpe0_sts = ps->gpe0_sts & ps->gpe0_en;

	if (ps->pm1_sts & WAK_STS) {
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
		                    acpi_slp_type == 3 ? 3 : 5);
	}

	if (ps->pm1_sts & PWRBTN_STS) {
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);
	}

	if (ps->pm1_sts & RTC_STS) {
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);
	}

	if (gpe0_sts & PME_B0_EN) {
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);
	}

	if (gpe0_sts & pcie_wake_mask) {
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);
	}

	gpio_mask = SUS_GPIO_STS0;
	i = 0;
	while (gpio_mask) {
		if (gpio_mask & gpe0_sts) {
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i);
		}
		gpio_mask <<= 1;
		i++;
	}
}

void southcluster_log_state(void)
{
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);

	if (ps == NULL) {
		printk(BIOS_DEBUG, "Not logging power state information. "
			"Power state not found in cbmem.\n");
		return;
	}

	log_power_and_resets(ps);
	log_wake_events(ps);
}
