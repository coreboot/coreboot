/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3288_DISPLAY_H__
#define __SOC_ROCKCHIP_RK3288_DISPLAY_H__

#include <device/device.h>
#include <stdint.h>

/*
 * this bit select edp phy pll, this bit define different between
 * rk3288 and rk3399 in edp phy, so implement it in soc specific code
 */
#define REF_CLK_24M	(0x0 << 0)
#define REF_CLK_27M	(0x1 << 0)

void rk_display_init(struct device *dev, u32 lcdbase, unsigned long fb_size);

void mainboard_power_on_backlight(void);

#endif
