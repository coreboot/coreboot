/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <soc/pmic_wrap_common.h>
#include <soc/rtc.h>

u16 rtc_read(u16 addr)
{
	s32 ret;
	u16 rdata = 0;

	ret = pwrap_read(addr, &rdata);
	assert(ret == 0);
	return rdata;
}

void rtc_write(u16 addr, u16 wdata)
{
	s32 ret;

	ret = pwrap_write(addr, wdata);
	assert(ret == 0);
}
