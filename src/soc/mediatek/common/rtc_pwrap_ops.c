/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/pmic_wrap_common.h>
#include <soc/rtc.h>

void rtc_read(u16 addr, u16 *rdata)
{
	s32 ret;

	*rdata = 0;
	ret = pwrap_read(addr, rdata);
	assert(ret == 0);
}

void rtc_write(u16 addr, u16 wdata)
{
	s32 ret;

	ret = pwrap_write(addr, wdata);
	assert(ret == 0);
}
