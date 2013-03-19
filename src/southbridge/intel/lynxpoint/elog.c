/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/io.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <string.h>
#include <elog.h>
#include "pch.h"

static void pch_log_standard_gpe(u32 gpe0_sts_reg, u32 gpe0_en_reg)
{
	u32 gpe0_en = inl(get_pmbase() + gpe0_en_reg);
	u32 gpe0_sts = inl(get_pmbase() + gpe0_sts_reg) & gpe0_en;

	/* PME (TODO: determine wake device) */
	if (gpe0_sts & (1 << 11))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* Internal PME (TODO: determine wake device) */
	if (gpe0_sts & (1 << 13))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);

	/* SMBUS Wake */
	if (gpe0_sts & (1 << 7))
		elog_add_event_wake(ELOG_WAKE_SOURCE_SMBUS, 0);
}

static void pch_log_gpio_gpe(u32 gpe0_sts_reg, u32 gpe0_en_reg, int start)
{
	/* GPE Bank 1 is GPIO 0-31 */
	u32 gpe0_en = inl(get_pmbase() + gpe0_en_reg);
	u32 gpe0_sts = inl(get_pmbase() + gpe0_sts_reg) & gpe0_en;
	int i;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i + start);
	}
}

static void pch_log_gpe(void)
{
	int i;
	u16 pmbase = get_pmbase();
	u32 gpe0_sts, gpe0_en;
	int gpe0_high_gpios[] = {
		[0] = 27,
		[24] = 17,
		[25] = 19,
		[26] = 21,
		[27] = 22,
		[28] = 43,
		[29] = 56,
		[30] = 57,
		[31] = 60
	};

	pch_log_standard_gpe(GPE0_EN, GPE0_STS);

	/* GPIO 0-15 */
	gpe0_en = inw(pmbase + GPE0_EN + 2);
	gpe0_sts = inw(pmbase + GPE0_STS + 2) & gpe0_en;
	for (i = 0; i <= 15; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i);
	}

	/*
	 * Now check and log upper status bits
	 */

	gpe0_en = inl(pmbase + GPE0_EN_2);
	gpe0_sts = inl(pmbase + GPE0_STS_2) & gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (!gpe0_high_gpios[i])
			continue;
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO,
					    gpe0_high_gpios[i]);
	}
}

static void pch_lp_log_gpe(void)
{
	/* Standard GPE are in GPE set 4 */
	pch_log_standard_gpe(LP_GPE0_STS_4, LP_GPE0_EN_4);

	/* Log GPIO events in set 1-3 */
	pch_log_gpio_gpe(LP_GPE0_STS_1, LP_GPE0_EN_1, 0);
	pch_log_gpio_gpe(LP_GPE0_STS_2, LP_GPE0_EN_2, 32);
	pch_log_gpio_gpe(LP_GPE0_STS_3, LP_GPE0_EN_3, 64);
}

void pch_log_state(void)
{
	u16 pm1_sts, gen_pmcon_3, tco2_sts;
	u8 gen_pmcon_2;
	struct device *lpc = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	if (!lpc)
		return;

	pm1_sts = inw(get_pmbase() + PM1_STS);
	tco2_sts = inw(get_pmbase() + TCO2_STS);
	gen_pmcon_2 = pci_read_config8(lpc, GEN_PMCON_2);
	gen_pmcon_3 = pci_read_config16(lpc, GEN_PMCON_3);

	/* PWR_FLR Power Failure */
	if (gen_pmcon_2 & (1 << 0))
		elog_add_event(ELOG_TYPE_POWER_FAIL);

	/* SUS Well Power Failure */
	if (gen_pmcon_3 & (1 << 14))
		elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);

	/* SYS_PWROK Failure */
	if (gen_pmcon_2 & (1 << 1))
		elog_add_event(ELOG_TYPE_SYS_PWROK_FAIL);

	/* PWROK Failure */
	if (gen_pmcon_2 & (1 << 0))
		elog_add_event(ELOG_TYPE_PWROK_FAIL);

	/* Second TCO Timeout */
	if (tco2_sts & (1 << 1))
		elog_add_event(ELOG_TYPE_TCO_RESET);

	/* Power Button Override */
	if (pm1_sts & (1 << 11))
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

	/* System Reset Status (reset button pushed) */
	if (gen_pmcon_2 & (1 << 4))
		elog_add_event(ELOG_TYPE_RESET_BUTTON);

	/* General Reset Status */
	if (gen_pmcon_3 & (1 << 9))
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);

	/* ACPI Wake */
	if (pm1_sts & (1 << 15))
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
				    acpi_slp_type == 3 ? 3 : 5);

	/*
	 * Wake sources
	 */

	/* Power Button */
	if (pm1_sts & (1 << 8))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	/* RTC */
	if (pm1_sts & (1 << 10))
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	/* PCI Express (TODO: determine wake device) */
	if (pm1_sts & (1 << 14))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	/* GPE */
	if (pch_is_lp())
		pch_lp_log_gpe();
	else
		pch_log_gpe();
}
