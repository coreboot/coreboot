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
#include <southbridge/intel/common/pmutils.h>

void pch_log_state(void)
{
	u16 pm1_sts, gen_pmcon_3, tco2_sts;
	u32 gpe0_sts, gpe0_en;
	u8 gen_pmcon_2;
	int i;
	struct device *lpc = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	if (!lpc)
		return;

	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);
	gpe0_sts = inl(DEFAULT_PMBASE + GPE0_STS);
	gpe0_en = inl(DEFAULT_PMBASE + GPE0_EN);
	tco2_sts = inw(DEFAULT_PMBASE + TCO2_STS);
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
				    acpi_is_wakeup_s3() ? 3 : 5);

	/*
	 * Wake sources
	 */

	/* RTC */
	if (pm1_sts & (1 << 10))
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	/* PCI Express (TODO: determine wake device) */
	if (pm1_sts & (1 << 14))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	/* PME (TODO: determine wake device) */
	if (gpe0_sts & (1 << 13))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* Internal PME (TODO: determine wake device) */
	if (gpe0_sts & (1 << 13))
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);

	/* GPIO 0-15 */
	for (i = 0; i < 16; i++) {
		if ((gpe0_sts & (1 << (16+i))) && (gpe0_en & (1 << (16+i))))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i);
	}

	/* SMBUS Wake */
	if (gpe0_sts & (1 << 7))
		elog_add_event_wake(ELOG_WAKE_SOURCE_SMBUS, 0);
}
