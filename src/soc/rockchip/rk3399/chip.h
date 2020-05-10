/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3399_CHIP_H__
#define __SOC_ROCKCHIP_RK3399_CHIP_H__

#include <soc/gpio.h>
#include <soc/vop.h>	/* for vop_modes enum used in devicetree.cb */

struct soc_rockchip_rk3399_config {
	u32 framebuffer_bits_per_pixel;
	u32 vop_mode;
};

#endif /* __SOC_ROCKCHIP_RK3399_CHIP_H__ */
