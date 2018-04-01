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
	if (!(inw(pm_acpi_pm_evt_blk()) & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(inw(pm_acpi_pm_cnt_blk())) == ACPI_S3;
}
