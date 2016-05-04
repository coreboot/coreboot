/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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

#ifndef __SOC_ROCKCHIP_RK3399_DISPLAY_H__
#define __SOC_ROCKCHIP_RK3399_DISPLAY_H__

#define REF_CLK_24M (0x1 << 0)

void rk_display_init(device_t dev, uintptr_t lcdbase,
		     unsigned long fb_size);
void mainboard_power_on_backlight(void);

#endif
