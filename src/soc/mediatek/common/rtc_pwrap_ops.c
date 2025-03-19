/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pmic_wrap_common.h>
#include <soc/rtc.h>

s32 rtc_read(u16 addr, u16 *rdata)
{
	s32 ret;

	ret = pwrap_read(addr, rdata);
	if (ret < 0)
		rtc_info("pwrap_read failed: ret=%d\n", ret);

	return ret;
}

s32 rtc_write(u16 addr, u16 wdata)
{
	s32 ret;

	ret = pwrap_write(addr, wdata);
	if (ret < 0)
		rtc_info("pwrap_write failed: ret=%d\n", ret);

	return ret;
}
