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

#ifndef __SOC_ROCKCHIP_RK3288_GRF_H__
#define __SOC_ROCKCHIP_RK3288_GRF_H__
#include "addressmap.h"

struct rk3288_grf_gpio_lh {
	u32 gpiol;
	u32 gpioh;
};

struct rk3288_grf_gpio_pe {
	u32 gpioa;
	u32 gpiob;
	u32 gpioc;
	u32 gpiod;
};

struct rk3288_grf_regs {
	u32 reserved[3];
	u32 gpio1d_iomux;
	u32 gpio2a_iomux;
	u32 gpio2b_iomux;
	u32 gpio2c_iomux;
	u32 reserved2;
	u32 gpio3a_iomux;
	u32 gpio3b_iomux;
	u32 gpio3c_iomux;
	u32 gpio3dl_iomux;
	u32 gpio3dh_iomux;
	u32 gpio4al_iomux;
	u32 gpio4ah_iomux;
	u32 gpio4bl_iomux;
	u32 reserved3;
	u32 gpio4c_iomux;
	u32 gpio4d_iomux;
	u32 reserved4;
	u32 gpio5b_iomux;
	u32 gpio5c_iomux;
	u32 gpio6a_iomux;
	u32 reserved5;
	u32 gpio6b_iomux;
	u32 gpio6c_iomux;
	u32 reserved6;
	u32 gpio7a_iomux;
	u32 gpio7b_iomux;
	u32 gpio7cl_iomux;
	u32 gpio7ch_iomux;
	u32 reserved7;
	u32 gpio8a_iomux;
	u32 gpio8b_iomux;
	u32 reserved8[30];
	struct rk3288_grf_gpio_lh gpio_sr[8];
	struct rk3288_grf_gpio_pe gpio_p[8];
	struct rk3288_grf_gpio_pe gpio_e[8];
	u32 gpio_smt;
	u32 soc_con[15];
	u32 soc_status[22];
	u32 reserved9[2];
	u32 peridmac_con[4];
	u32 ddrc0_con0;
	u32 ddrc1_con0;
	u32 cpu_con[5];
	u32 reserved10[3];
	u32 cpu_status0;
	u32 reserved11;
	u32 uoc0_con[5];
	u32 uoc1_con[5];
	u32 uoc2_con[4];
	u32 uoc3_con[2];
	u32 uoc4_con[2];
	u32 pvtm_con[3];
	u32 pvtm_status[3];
	u32 io_vsel;
	u32 saradc_testbit;
	u32 tsadc_testbit_l;
	u32 tsadc_testbit_h;
	u32 os_reg[4];
	u32 reserved12;
	u32 soc_con15;
	u32 soc_con16;
};
check_member(rk3288_grf_regs, soc_con16, 0x3a8);

static struct rk3288_grf_regs * const rk3288_grf = (void *)GRF_BASE;
#endif
