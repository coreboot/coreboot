/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#ifndef __SOC_ROCKCHIP_RK3288_DISPLAY_H__
#define __SOC_ROCKCHIP_RK3288_DISPLAY_H__

/*
 * this bit select edp phy pll, this bit define different between
 * rk3288 and rk3399 in edp phy, so implement it in soc specific code
 */
#define REF_CLK_24M	(0x0 << 0)
#define REF_CLK_27M	(0x1 << 0)

void rk_display_init(struct device *dev, u32 lcdbase, unsigned long fb_size);

void mainboard_power_on_backlight(void);

#endif
