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

#ifndef __SOC_ROCKCHIP_RK3399_CHIP_H__
#define __SOC_ROCKCHIP_RK3399_CHIP_H__

#include <soc/gpio.h>
#include <soc/vop.h>	/* for vop_modes enum used in devicetree.cb */

struct soc_rockchip_rk3399_config {
	gpio_t lcd_bl_pwm_gpio;
	gpio_t lcd_bl_en_gpio;
	u32 bl_power_on_udelay;
	u32 bl_pwm_to_enable_udelay;
	u32 framebuffer_bits_per_pixel;
	u32 vop_mode;
	u32 panel_pixel_clock;	/* below only be considered for MIPI displays */
	u32 panel_refresh;
	u32 panel_ha;
	u32 panel_hbl;
	u32 panel_hso;
	u32 panel_hspw;
	u32 panel_va;
	u32 panel_vbl;
	u32 panel_vso;
	u32 panel_vspw;
	u32 panel_display_on_mdelay;
	u32 panel_video_mode_mdelay;
};

#endif /* __SOC_ROCKCHIP_RK3399_CHIP_H__ */
