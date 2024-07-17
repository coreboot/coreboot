/* SPDX-License-Identifier: GPL-2.0-only */

#include <security/vboot/vbnv.h>
#include <pc80/mc146818rtc.h>

void vbnv_platform_init_cmos(void)
{
	/* The 0 argument tells cmos_init not to update CMOS unless it is invalid. */
	cmos_init(0);
}

int vbnv_cmos_failed(void)
{
	/* If CMOS power has failed, the century will be set to 0xff */
	return cmos_read(RTC_CLK_ALTCENTURY) == 0xff;
}
