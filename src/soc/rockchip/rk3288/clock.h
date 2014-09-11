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

#ifndef __SOC_ROCKCHIP_RK3288_CLOCK_H__
#define __SOC_ROCKCHIP_RK3288_CLOCK_H__

#include "addressmap.h"

#define APLL_HZ		816000000
#define GPLL_HZ		594000000
#define CPLL_HZ		384000000

void rkclk_init(void);
void rkclk_configure_spi(unsigned int bus, unsigned int hz);
void rkclk_ddr_reset(u32 ch, u32 ctl, u32 phy);
void rkclk_ddr_phy_ctl_reset(u32 ch, u32 n);
void rkclk_configure_ddr(unsigned int hz);
void rkclk_configure_i2s(unsigned int hz);

#endif	/* __SOC_ROCKCHIP_RK3288_CLOCK_H__ */
