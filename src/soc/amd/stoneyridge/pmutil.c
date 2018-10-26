/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <arch/acpi.h>
#include <soc/southbridge.h>
#include <security/vboot/vboot_common.h>
#include <security/vboot/vbnv.h>
#include <pc80/mc146818rtc.h>

int vbnv_cmos_failed(void)
{
	/* If CMOS power has failed, the century will be set to 0xff */
	return cmos_read(RTC_CLK_ALTCENTURY) == 0xff;
}

int vboot_platform_is_resuming(void)
{
	if (!(acpi_read16(MMIO_ACPI_PM1_STS) & WAK_STS))
		return 0;

	uint16_t pm_cnt = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	return acpi_sleep_from_pm1(pm_cnt) == ACPI_S3;
}

/* If vboot requests a system reset, modify the PM1 register so it will never be
 * misinterpreted as an S3 resume. */
void vboot_platform_prepare_reboot(void)
{
	uint16_t pm1;

	pm1 = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	pm1 &= ~SLP_TYP;
	pm1 |= SLP_TYP_S5 << SLP_TYP_SHIFT;
	acpi_write16(MMIO_ACPI_PM1_CNT_BLK, pm1);
}
