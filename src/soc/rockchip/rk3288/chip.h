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

#include <soc/gpio.h>

struct soc_rockchip_rk3288_config {
	u32 vop_id;
	gpio_t lcd_bl_pwm_gpio;
	gpio_t lcd_bl_en_gpio;
	u32 bl_power_on_udelay;
	u32 bl_pwm_to_enable_udelay;
	u32 framebuffer_bits_per_pixel;
};

#endif /* __SOC_ROCKCHIP_RK3288_CHIP_H__ */
