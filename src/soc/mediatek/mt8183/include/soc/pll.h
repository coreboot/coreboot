/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8183_PLL_H
#define SOC_MEDIATEK_MT8183_PLL_H

#include <types.h>
#include <soc/pll_common.h>

struct mtk_topckgen_regs {
	u32 clk_mode;
	u32 clk_cfg_update;
	u32 clk_cfg_update1;
	u32 reserved1[13];
	u32 clk_cfg_0;
	u32 clk_cfg_0_set;
	u32 clk_cfg_0_clr;
	u32 reserved2[1];
	u32 clk_cfg_1;
	u32 clk_cfg_1_set;
	u32 clk_cfg_1_clr;
	u32 reserved3[1];
	u32 clk_cfg_2;
	u32 clk_cfg_2_set;
	u32 clk_cfg_2_clr;
	u32 reserved4[1];
	u32 clk_cfg_3;
	u32 clk_cfg_3_set;
	u32 clk_cfg_3_clr;
	u32 reserved5[1];
	u32 clk_cfg_4;
	u32 clk_cfg_4_set;
	u32 clk_cfg_4_clr;
	u32 reserved6[1];
	u32 clk_cfg_5;
	u32 clk_cfg_5_set;
	u32 clk_cfg_5_clr;
	u32 reserved7[1];
	u32 clk_cfg_6;
	u32 clk_cfg_6_set;
	u32 clk_cfg_6_clr;
	u32 reserved8[1];
	u32 clk_cfg_7;
	u32 clk_cfg_7_set;
	u32 clk_cfg_7_clr;
	u32 reserved9[1];
	u32 clk_cfg_8;
	u32 clk_cfg_8_set;
	u32 clk_cfg_8_clr;
	u32 reserved10[1];
	u32 clk_cfg_9;
	u32 clk_cfg_9_set;
	u32 clk_cfg_9_clr;
	u32 reserved11[1];
	u32 clk_cfg_10;
	u32 clk_cfg_10_set;
	u32 clk_cfg_10_clr;
	u32 reserved12[6];
	u32 clk_misc_cfg_0;
	u32 clk_misc_cfg_1;
	u32 clk_dbg_cfg;
	u32 reserved13[60];
	u32 clk_scp_cfg_0;
	u32 clk_scp_cfg_1;
	u32 reserved14[6];
	u32 clk26cali_0;
	u32 clk26cali_1;
	u32 reserved15[2];
	u32 cksta_reg;
	u32 cksta_reg1;
	u32 reserved16[50];
	u32 clkmon_clk_sel_reg;
	u32 clkmon_k1_reg;
	u32 reserved17[6];
	u32 clk_auddiv_0;
	u32 clk_auddiv_1;
	u32 clk_auddiv_2;
	u32 aud_top_cfg;
	u32 aud_top_mon;
	u32 clk_auddiv_3;
	u32 reserved18[50];
	u32 clk_pdn_reg;
	u32 reserved19[63];
	u32 clk_extck_reg;
	u32 reserved20[79];
	u32 clk_cfg_20;
	u32 clk_cfg_20_set;
	u32 clk_cfg_20_clr;
};

check_member(mtk_topckgen_regs, clk_cfg_0, 0x0040);
check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x0104);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x0200);
check_member(mtk_topckgen_regs, clk26cali_0, 0x0220);
check_member(mtk_topckgen_regs, cksta_reg, 0x0230);
check_member(mtk_topckgen_regs, clkmon_clk_sel_reg, 0x0300);
check_member(mtk_topckgen_regs, clk_auddiv_0, 0x0320);
check_member(mtk_topckgen_regs, clk_pdn_reg, 0x0400);
check_member(mtk_topckgen_regs, clk_extck_reg, 0x0500);
check_member(mtk_topckgen_regs, clk_cfg_20, 0x0640);
check_member(mtk_topckgen_regs, clk_cfg_20_clr, 0x0648);

struct mtk_apmixed_regs {
	u32 ap_pll_con0;
	u32 ap_pll_con1;
	u32 ap_pll_con2;
	u32 ap_pll_con3;
	u32 ap_pll_con4;
	u32 ap_pll_con5;
	u32 ap_pll_con6;
	u32 ap_pll_con7;
	u32 ap_pll_con8;
	u32 clksq_stb_con0;
	u32 pll_pwr_con0;
	u32 pll_pwr_con1;
	u32 pll_iso_con0;
	u32 pll_iso_con1;
	u32 pll_stb_con0;
	u32 div_stb_con0;
	u32 pll_chg_con0;
	u32 pll_test_con0;
	u32 pll_test_con1;
	u32 reserved1[109];
	u32 armpll_ll_con0;
	u32 armpll_ll_con1;
	u32 armpll_ll_con2;
	u32 armpll_ll_pwr_con0;
	u32 armpll_l_con0;
	u32 armpll_l_con1;
	u32 armpll_l_con2;
	u32 armpll_l_pwr_con0;
	u32 mainpll_con0;
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_pwr_con0;
	u32 univpll_con0;
	u32 univpll_con1;
	u32 univpll_con2;
	u32 univpll_pwr_con0;
	u32 mfgpll_con0;
	u32 mfgpll_con1;
	u32 mfgpll_con2;
	u32 mfgpll_pwr_con0;
	u32 msdcpll_con0;
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_pwr_con0;
	u32 tvdpll_con0;
	u32 tvdpll_con1;
	u32 tvdpll_con2;
	u32 tvdpll_pwr_con0;
	u32 mmpll_con0;
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_pwr_con0;
	u32 mpll_con0;
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_pwr_con0;
	u32 ccipll_con0;
	u32 ccipll_con1;
	u32 ccipll_con2;
	u32 ccipll_pwr_con0;
	u32 apll1_con0;
	u32 apll1_con1;
	u32 apll1_con2;
	u32 apll1_con3;
	u32 apll1_pwr_con0;
	u32 apll2_con0;
	u32 apll2_con1;
	u32 apll2_con2;
	u32 apll2_con3;
	u32 apll2_pwr_con0;
	u32 reserved2[78];
	u32 ap_auxadc_con0;
	u32 ap_auxadc_con1;
	u32 ap_auxadc_con2;
	u32 ap_auxadc_con3;
	u32 ap_auxadc_con4;
	u32 ap_auxadc_con5;
	u32 reserved3[122];
	u32 ts_con0;
	u32 ts_con1;
	u32 ts_con2;
	u32 reserved4[61];
	u32 ulposc_con0;
	u32 ulposc_con1;
	u32 ulposc2_con0;
	u32 ulposc2_con1;
	u32 reserved5[60];
	u32 ap_abist_mon_con0;
	u32 ap_abist_mon_con1;
	u32 ap_abist_mon_con2;
	u32 ap_abist_mon_con3;
	u32 occscan_con0;
	u32 clkdiv_con0;
	u32 occscan_con1;
	u32 occscan_con2;
	u32 mcu_occscan_con0;
	u32 reserved6[55];
	u32 rsv_rw0_con0;
	u32 rsv_rw1_con0;
	u32 rsv_ro_con0;
};

check_member(mtk_apmixed_regs, armpll_ll_con0, 0x0200);
check_member(mtk_apmixed_regs, ap_auxadc_con0, 0x0400);
check_member(mtk_apmixed_regs, ts_con0, 0x0600);
check_member(mtk_apmixed_regs, ulposc_con0, 0x0700);
check_member(mtk_apmixed_regs, ap_abist_mon_con0, 0x0800);
check_member(mtk_apmixed_regs, rsv_rw0_con0, 0x0900);
check_member(mtk_apmixed_regs, rsv_ro_con0, 0x0908);

enum {
	DIV_MASK = 0x1f << 17,
	DIV_1 = 0x8 << 17,
	DIV_2 = 0xa << 17,

	MUX_MASK = 0x3 << 9,
	MUX_SRC_ARMPLL = 0x1 << 9,
};

enum {
	PLL_PWR_ON_DELAY = 30,
	PLL_ISO_DELAY = 1,
	PLL_EN_DELAY = 20,
};

enum {
	PCW_INTEGER_BITS = 8,
};

/* PLL rate */
enum {
	ARMPLL_LL_HZ	= 1100 * MHz,
	ARMPLL_L_HZ	= 1200 * MHz,
	CCIPLL_HZ	= 598 * 2 * MHz,
	MAINPLL_HZ	= 1092 * MHz,
	UNIVPLL_HZ	= 1248UL * 2 * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	MMPLL_HZ	= 3150UL * MHz,
	MFGPLL_HZ	= 512 * MHz,
	TVDPLL_HZ	= 594 * MHz,
	APLL1_HZ	= 180633600,
	APLL2_HZ	= 196608 * KHz,
};

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
	MAINPLL_D5_HZ   = MAINPLL_HZ / 5,
	MAINPLL_D5_D2_HZ = MAINPLL_D5_HZ / 2,
};

/* top_mux rate */
enum {
	SPI_HZ		= MAINPLL_D5_D2_HZ,
};

#endif /* SOC_MEDIATEK_MT8183_PLL_H */
