/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/addressmap.h>
#include <types.h>

#ifndef __SOC_QUALCOMM_SDM845_CLOCK_H__
#define	__SOC_QUALCOMM_SDM845_CLOCK_H__

#define	QUPV3_WRAP_0_M_AHB_CLK_ENA		6
#define	QUPV3_WRAP_0_S_AHB_CLK_ENA		7
#define	QUPV3_WRAP0_CORE_2X_CLK_ENA		9
#define	QUPV3_WRAP0_CORE_CLK_ENA		8
#define	QUPV3_WRAP1_CORE_2X_CLK_ENA		18
#define	QUPV3_WRAP1_CORE_CLK_ENA		19
#define	QUPV3_WRAP_1_M_AHB_CLK_ENA		20
#define	QUPV3_WRAP_1_S_AHB_CLK_ENA		21
#define	QUPV3_WRAP0_CLK_ENA_S(idx)		(10 + idx)
#define	QUPV3_WRAP1_CLK_ENA_S(idx)		(22 + idx)

#define	GPLL0_EVEN_HZ				(300*MHz)
#define	GPLL0_MAIN_HZ				(600*MHz)
#define	QUP_WRAP_CORE_2X_19_2MHZ		(19200*KHz)

#define SRC_XO_19_2MHZ				0
#define SRC_GPLL0_MAIN_600MHZ			1
#define SRC_GPLL0_EVEN_300MHZ			6

#define	AOP_RESET_SHFT				0
#define RCG_MODE_DUAL_EDGE			2

struct sdm845_rcg {
	u32 cmd;
	u32 cfg;
};

struct sdm845_clock {
	u32 cbcr;
	struct sdm845_rcg rcg;
	u32 m;
	u32 n;
	u32 d_2;
};

struct sdm845_qupv3_clock {
	struct sdm845_clock clk;
	u8 _res[0x130 - 0x18];
};

struct sdm845_gpll {
	u32 mode;
	u32 l_val;
	u32 cal_l_val;
	u32 user_ctl;
	u32 user_ctl_u;
	u32 config_ctl;
	u32 config_ctl_u;
	u32 test_ctl;
	u32 test_ctl_u;
	u8 _res[0x1000 - 0x24];
};

struct sdm845_gcc {
	struct sdm845_gpll gpll0;
	u8 _res0[0xf000 - 0x1000];
	u32 usb30_prim_bcr;
	u8 _res1[0x10000 - 0xf004];
	u32 usb30_sec_bcr;
	u8 _res2[0x12000 - 0x10004];
	u32 qusb2phy_prim_bcr;
	u32 qusb2phy_sec_bcr;
	u8 _res3[0x17000 - 0x12008];
	u32 qup_wrap0_bcr;
	u32 qup_wrap0_m_ahb_cbcr;
	u32 qup_wrap0_s_ahb_cbcr;
	u32 qup_wrap0_core_cbcr;
	u32 qup_wrap0_core_cdivr;
	u32 qup_wrap0_core_2x_cbcr;
	struct sdm845_rcg qup_wrap0_core_2x;
	u8 _res4[0x17030 - 0x17020];
	struct sdm845_qupv3_clock qup_wrap0_s[8];
	u8 _res5[0x18000 - 0x179b0];
	u32 qup_wrap1_bcr;
	u32 qup_wrap1_core_2x_cbcr;
	u32 qup_wrap1_core_cbcr;
	u32 qup_wrap1_m_ahb_cbcr;
	u32 qup_wrap1_s_ahb_cbcr;
	struct sdm845_qupv3_clock qup_wrap1_s[8];
	u32 qup_wrap1_core_cdivr;
	u8 _res6[0x4B000 - 0x18998];
	u32 qspi_cnoc_ahb_cbcr;
	u32 qspi_core_cbcr;
	struct sdm845_rcg qspi_core;
	u8 _res7[0x50000-0x4b010];
	u32 usb3_phy_prim_bcr;
	u32 usb3phy_phy_prim_bcr;
	u32 usb3_dp_phy_prim_bcr;
	u32 usb3_phy_sec_bcr;
	u32 usb3phy_phy_sec_bcr;
	u8 _res8[0x5200c-0x50014];
	u32 apcs_clk_br_en1;
	u8 _res9[0x1000000-0x52010];
};
check_member(sdm845_gcc, usb30_prim_bcr, 0xf000);
check_member(sdm845_gcc, usb30_sec_bcr, 0x10000);
check_member(sdm845_gcc, qusb2phy_prim_bcr, 0x12000);
check_member(sdm845_gcc, qusb2phy_sec_bcr, 0x12004);
check_member(sdm845_gcc, usb3phy_phy_prim_bcr, 0x50004);
check_member(sdm845_gcc, usb3_phy_prim_bcr, 0x50000);
check_member(sdm845_gcc, usb3_phy_sec_bcr, 0x5000c);
check_member(sdm845_gcc, usb3phy_phy_sec_bcr, 0x50010);
check_member(sdm845_gcc, apcs_clk_br_en1, 0x5200c);

enum clk_ctl_gpll_user_ctl {
	CLK_CTL_GPLL_PLLOUT_EVEN_BMSK	= 0x2,
	CLK_CTL_GPLL_PLLOUT_MAIN_SHFT	= 0,
	CLK_CTL_GPLL_PLLOUT_EVEN_SHFT	= 1,
	CLK_CTL_GPLL_PLLOUT_ODD_SHFT	= 2
};

enum clk_ctl_cfg_rcgr {
	CLK_CTL_CFG_HW_CTL_BMSK		= 0x100000,
	CLK_CTL_CFG_HW_CTL_SHFT		= 20,
	CLK_CTL_CFG_MODE_BMSK		= 0x3000,
	CLK_CTL_CFG_MODE_SHFT		= 12,
	CLK_CTL_CFG_SRC_SEL_BMSK	= 0x700,
	CLK_CTL_CFG_SRC_SEL_SHFT	= 8,
	CLK_CTL_CFG_SRC_DIV_BMSK	= 0x1F,
	CLK_CTL_CFG_SRC_DIV_SHFT	= 0
};

enum clk_ctl_cmd_rcgr {
	CLK_CTL_CMD_ROOT_OFF_BMSK	= 0x80000000,
	CLK_CTL_CMD_ROOT_OFF_SHFT	= 31,
	CLK_CTL_CMD_ROOT_EN_BMSK	= 0x2,
	CLK_CTL_CMD_ROOT_EN_SHFT	= 1,
	CLK_CTL_CMD_UPDATE_BMSK		= 0x1,
	CLK_CTL_CMD_UPDATE_SHFT		= 0
};

enum clk_ctl_cbcr {
	CLK_CTL_CBC_CLK_OFF_BMSK	= 0x80000000,
	CLK_CTL_CBC_CLK_OFF_SHFT	= 31,
	CLK_CTL_CBC_CLK_EN_BMSK		= 0x1,
	CLK_CTL_CBC_CLK_EN_SHFT		= 0
};

enum clk_ctl_rcg_mnd {
	CLK_CTL_RCG_MND_BMSK		= 0xFFFF,
	CLK_CTL_RCG_MND_SHFT		= 0,
};

enum clk_ctl_bcr {
	CLK_CTL_BCR_BLK_ARES_BMSK	= 0x1,
	CLK_CTL_BCR_BLK_ARES_SHFT	= 0,
};

enum clk_qup {
	QUP_WRAP0_S0,
	QUP_WRAP0_S1,
	QUP_WRAP0_S2,
	QUP_WRAP0_S3,
	QUP_WRAP0_S4,
	QUP_WRAP0_S5,
	QUP_WRAP0_S6,
	QUP_WRAP0_S7,
	QUP_WRAP1_S0,
	QUP_WRAP1_S1,
	QUP_WRAP1_S2,
	QUP_WRAP1_S3,
	QUP_WRAP1_S4,
	QUP_WRAP1_S5,
	QUP_WRAP1_S6,
	QUP_WRAP1_S7
};

struct clock_config {
	uint32_t hz;
	uint8_t src;
	uint8_t div;
	uint16_t m;
	uint16_t n;
	uint16_t d_2;
};

static struct sdm845_gcc *const gcc = (void *)GCC_BASE;

void clock_init(void);
void clock_reset_aop(void);
void clock_configure_qspi(uint32_t hz);
int clock_reset_bcr(void *bcr_addr, bool reset);
void clock_configure_qup(int qup, uint32_t hz);
void clock_enable_qup(int qup);

#endif	// __SOC_QUALCOMM_SDM845_CLOCK_H__
