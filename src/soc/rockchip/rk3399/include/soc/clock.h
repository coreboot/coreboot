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

#ifndef __SOC_ROCKCHIP_RK3399_CLOCK_H__
#define __SOC_ROCKCHIP_RK3399_CLOCK_H__

#include <soc/addressmap.h>
#include <types.h>

struct rk3399_pmucru_reg {
	u32 ppll_con[6];
	u32 reserved[0x1a];
	u32 pmucru_clksel[6];
	u32 pmucru_clkfrac_con[2];
	u32 reserved2[0x18];
	u32 pmucru_clkgate_con[3];
	u32 reserved3;
	u32 pmucru_softrst_con[2];
	u32 reserved4[2];
	u32 pmucru_rstnhold_con[2];
	u32 reserved5[2];
	u32 pmucru_gatedis_con[2];
};
check_member(rk3399_pmucru_reg, pmucru_gatedis_con[1], 0x134);

struct rk3399_cru_reg {
	u32 apll_l_con[6];
	u32 reserved[2];
	u32 apll_b_con[6];
	u32 reserved1[2];
	u32 dpll_con[6];
	u32 reserved2[2];
	u32 cpll_con[6];
	u32 reserved3[2];
	u32 gpll_con[6];
	u32 reserved4[2];
	u32 npll_con[6];
	u32 reserved5[2];
	u32 vpll_con[6];
	u32 reserved6[0x0a];
	u32 clksel_con[108];
	u32 reserved7[0x14];
	u32 clkgate_con[35];
	u32 reserved8[0x1d];
	u32 softrst_con[21];
	u32 reserved9[0x2b];
	u32 glb_srst_fst_value;
	u32 glb_srst_snd_value;
	u32 glb_cnt_th;
	u32 misc_con;
	u32 glb_rst_con;
	u32 glb_rst_st;
	u32 reserved10[0x1a];
	u32 sdmmc_con[2];
	u32 sdio0_con[2];
	u32 sdio1_con[2];
};
check_member(rk3399_cru_reg, sdio1_con[1], 0x594);

static struct rk3399_pmucru_reg * const pmucru_ptr = (void *)PMUCRU_BASE;
static struct rk3399_cru_reg * const cru_ptr = (void *)CRU_BASE;

#define OSC_HZ		(24*MHz)
#define APLL_HZ		(600*MHz)
#define GPLL_HZ		(594*MHz)
#define CPLL_HZ		(384*MHz)
#define PPLL_HZ		(594*MHz)

#define PMU_PCLK_HZ	(99*MHz)

#define ACLKM_CORE_HZ	(300*MHz)
#define ATCLK_CORE_HZ	(300*MHz)
#define PCLK_DBG_HZ	(100*MHz)

#define PERIHP_ACLK_HZ	(148500*KHz)
#define PERIHP_HCLK_HZ	(148500*KHz)
#define PERIHP_PCLK_HZ	(37125*KHz)

#define PERILP0_ACLK_HZ	(99000*KHz)
#define PERILP0_HCLK_HZ	(99000*KHz)
#define PERILP0_PCLK_HZ	(49500*KHz)

#define PERILP1_HCLK_HZ	(99000*KHz)
#define PERILP1_PCLK_HZ	(49500*KHz)

#define PWM_CLOCK_HZ    PMU_PCLK_HZ

enum apll_l_frequencies {
	APLL_L_1600_MHZ,
	APLL_L_600_MHZ,
};

void rkclk_init(void);
int rkclk_configure_vop_dclk(u32 vop_id, u32 dclk_hz);
void rkclk_configure_cpu(enum apll_l_frequencies apll_l_freq);
void rkclk_configure_ddr(unsigned int hz);
void rkclk_configure_saradc(unsigned int hz);
void rkclk_configure_spi(unsigned int bus, unsigned int hz);
void rkclk_configure_vop_aclk(u32 vop_id, u32 aclk_hz);
void rkclk_ddr_reset(u32 ch, u32 ctl, u32 phy);
uint32_t rkclk_i2c_clock_for_bus(unsigned bus);

#endif	/* __SOC_ROCKCHIP_RK3399_CLOCK_H__ */
