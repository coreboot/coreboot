/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3288_CHIP_H__
#define __SOC_ROCKCHIP_RK3288_CHIP_H__

#include <soc/gpio.h>
#include <soc/vop.h>	/* for vop_modes enum used in devicetree.cb */

struct soc_rockchip_rk3288_config {
	u32 vop_id;
	gpio_t lcd_bl_pwm_gpio;
	gpio_t lcd_bl_en_gpio;
	u32 bl_power_on_udelay;
	u32 bl_pwm_to_enable_udelay;
	u32 framebuffer_bits_per_pixel;
	u32 vop_mode;
};

#endif /* __SOC_ROCKCHIP_RK3288_CHIP_H__ */
