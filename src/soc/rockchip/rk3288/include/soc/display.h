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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __SOC_ROCKCHIP_RK3288_DISPLAY_H__
#define __SOC_ROCKCHIP_RK3288_DISPLAY_H__

void rk_display_init(device_t dev, u32 lcdbase,
		unsigned long fb_size);

void mainboard_power_on_backlight(void);

#endif
