/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 */

#include <arch/io.h>
#include <timestamp.h>
#include <cpu/x86/tsc.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include "pch.h"
#include <arch/acpi.h>
#include <console/console.h>
#include <rules.h>
#include <security/vboot/vbnv.h>

#if ENV_ROMSTAGE
uint64_t get_initial_timestamp(void)
{
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
	return tsc_to_uint64(base_time);
}

int southbridge_detect_s3_resume(void)
{
	u32 pm1_cnt;
	u16 pm1_sts;

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);

	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
		if (acpi_s3_resume_allowed()) {
			printk(BIOS_DEBUG, "Resume from S3 detected.\n");
			/* Clear SLP_TYPE. This will break stage2 but
			 * we care for that when we get there.
			 */
			outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + PM1_CNT);
			return 1;
		} else {
			printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
		}
	}

	return 0;
}
#endif

int rtc_failure(void)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
#else
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
#endif
	return !!(pci_read_config8(dev, GEN_PMCON_3) & RTC_BATTERY_DEAD);
}

int vbnv_cmos_failed(void)
{
	return rtc_failure();
}
