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
 */

#ifndef __SOC_ROCKCHIP_RK3288_PMU_H__
#define __SOC_ROCKCHIP_RK3288_PMU_H__

#include <soc/addressmap.h>

struct rk3288_pmu_regs {
	u32 wakeup_cfg[2];
	u32 pwrdn_con;
	u32 pwrdn_st;
	u32 idle_req;
	u32 idle_st;
	u32 pwrmode_con;
	u32 pwr_state;
	u32 osc_cnt;
	u32 pll_cnt;
	u32 stabl_cnt;
	u32 ddr0io_pwron_cnt;
	u32 ddr1io_pwron_cnt;
	u32 core_pwrdn_cnt;
	u32 core_pwrup_cnt;
	u32 gpu_pwrdn_cnt;
	u32 gpu_pwrup_cnt;
	u32 wakeup_rst_clr_cnt;
	u32 sft_con;
	u32 ddr_sref_st;
	u32 int_con;
	u32 int_st;
	u32 boot_addr_sel;
	u32 grf_con;
	u32 gpio_sr;
	u32 gpio0pull[3];
	u32 gpio0drv[3];
	u32 gpio_op;
	u32 gpio0_sel18;
	u32 gpio0a_iomux;
	union {
		u32 gpio0b_iomux;
		u32 iomux_i2c0sda;
		u32 iomux_tsadc_int;
	};
	union {
		u32 gpio0c_iomux;
		u32 iomux_i2c0scl;
	};
	union {
		u32 gpio0d_iomux;
		u32 iomux_lcdc0;
	};
	u32 sys_reg[4];
};
check_member(rk3288_pmu_regs, sys_reg[3], 0x00a0);

static struct rk3288_pmu_regs * const rk3288_pmu = (void *)PMU_BASE;

#define IOMUX_I2C0SDA	(1 << 14)
#define IOMUX_I2C0SCL	(1 << 0)
#define	IOMUX_TSADC_INT	(1 << 4)

#endif
