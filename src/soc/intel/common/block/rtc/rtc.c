/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <compiler.h>
#include <intelblocks/pcr.h>
#include <intelblocks/rtc.h>
#include <soc/pcr_ids.h>
#include <pc80/mc146818rtc.h>

/* RTC PCR configuration */
#define PCR_RTC_CONF		0x3400
#define PCR_RTC_CONF_UCMOS_EN	(1 << 2)
#define PCR_RTC_CONF_LCMOS_LOCK	(1 << 3)
#define PCR_RTC_CONF_UCMOS_LOCK	(1 << 4)
#define PCR_RTC_CONF_RESERVED	(1 << 31)

void enable_rtc_upper_bank(void)
{
	/* Enable upper 128 bytes of CMOS */
	pcr_or32(PID_RTC, PCR_RTC_CONF, PCR_RTC_CONF_UCMOS_EN);
}

__weak int soc_get_rtc_failed(void)
{
	return 0;
}

void rtc_init(void)
{
	/* Ensure the date is set including century byte. */
	cmos_check_update_date();

	cmos_init(soc_get_rtc_failed());
}
