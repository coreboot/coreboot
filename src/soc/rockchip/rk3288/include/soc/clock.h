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

#ifndef __SOC_ROCKCHIP_RK3288_CLOCK_H__
#define __SOC_ROCKCHIP_RK3288_CLOCK_H__

#include <soc/addressmap.h>

#define OSC_HZ		(24*MHz)

#define GPLL_HZ		(594*MHz)
#define CPLL_HZ		(384*MHz)
#define NPLL_HZ		(384*MHz)

enum apll_frequencies {
	APLL_1800_MHZ,
	APLL_1416_MHZ,
	APLL_600_MHZ,
};

/* The SRAM is clocked off aclk_bus, so we want to max it out for boot speed. */
#define PD_BUS_ACLK_HZ	(297000*KHz)
#define PD_BUS_HCLK_HZ	(148500*KHz)
#define PD_BUS_PCLK_HZ	(74250*KHz)

#define PERI_ACLK_HZ	(148500*KHz)
#define PERI_HCLK_HZ	(148500*KHz)
#define PERI_PCLK_HZ	(74250*KHz)

void rkclk_init(void);
void rkclk_configure_spi(unsigned int bus, unsigned int hz);
void rkclk_ddr_reset(u32 ch, u32 ctl, u32 phy);
void rkclk_ddr_phy_ctl_reset(u32 ch, u32 n);
void rkclk_configure_ddr(unsigned int hz);
void rkclk_configure_i2s(unsigned int hz);
void rkclk_configure_cpu(enum apll_frequencies apll_freq);
void rkclk_configure_crypto(unsigned int hz);
void rkclk_configure_tsadc(unsigned int hz);
void rkclk_configure_vop_aclk(u32 vop_id, u32 aclk_hz);
int rkclk_configure_vop_dclk(u32 vop_id, u32 dclk_hz);
void rkclk_configure_edp(void);
void rkclk_configure_hdmi(void);
int rkclk_was_watchdog_reset(void);
#endif	/* __SOC_ROCKCHIP_RK3288_CLOCK_H__ */
