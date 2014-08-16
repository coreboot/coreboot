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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_ROCKCHIP_RK3288_CHIP_H__
#define __SOC_ROCKCHIP_RK3288_CHIP_H__

struct soc_rockchip_rk3288_config {
	int screen_type;
	int lvds_format;
	int out_face;
	int clock_frequency;
	int hactive;
	int vactive;
	int hback_porch;
	int hfront_porch;
	int vback_porch;
	int vfront_porch;
	int hsync_len;
	int vsync_len;
	int hsync_active;
	int vsync_active;
	int de_active;
	int pixelclk_active;
	int swap_rb;
	int swap_rg;
	int swap_gb;
	int lcd_en_gpio;
	int lcd_cs_gpio;
};

#endif /* __SOC_ROCKCHIP_RK3288_CHIP_H__ */
