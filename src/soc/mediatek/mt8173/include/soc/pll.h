/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8173_PLL_H
#define SOC_MEDIATEK_MT8173_PLL_H

#include <soc/emi.h>
#include <soc/pll_common.h>

struct mtk_topckgen_regs {
	u32 clk_mode;
	u32 dcm_cfg;
	u32 reserved1[6];
	u32 tst_sel_0;		/* 0x020 */
	u32 tst_sel_1;
	u32 tst_sel_2;
	u32 reserved2[5];
	u32 clk_cfg_0;		/* 0x040 */
	u32 clk_cfg_0_set;
	u32 clk_cfg_0_clr;
	u32 reserved3[1];
	u32 clk_cfg_1;		/* 0x050 */
	u32 clk_cfg_1_set;
	u32 clk_cfg_1_clr;
	u32 reserved4[1];
	u32 clk_cfg_2;		/* 0x060 */
	u32 clk_cfg_2_set;
	u32 clk_cfg_2_clr;
	u32 reserved5[1];
	u32 clk_cfg_3;		/* 0x070 */
	u32 clk_cfg_3_set;
	u32 clk_cfg_3_clr;
	u32 reserved6[1];
	u32 clk_cfg_4;		/* 0x080 */
	u32 clk_cfg_4_set;
	u32 clk_cfg_4_clr;
	u32 reserved7[1];
	u32 clk_cfg_5;		/* 0x090 */
	u32 clk_cfg_5_set;
	u32 clk_cfg_5_clr;
	u32 reserved8[1];
	u32 clk_cfg_6;		/* 0x0a0 */
	u32 clk_cfg_6_set;
	u32 clk_cfg_6_clr;
	u32 reserved9[1];
	u32 clk_cfg_7;		/* 0x0b0 */
	u32 clk_cfg_7_set;
	u32 clk_cfg_7_clr;
	u32 reserved10[1];
	u32 clk_cfg_12;		/* 0x0c0 */
	u32 clk_cfg_12_set;
	u32 clk_cfg_12_clr;
	u32 reserved11[1];
	u32 clk_cfg_13;		/* 0x0d0 */
	u32 clk_cfg_13_set;
	u32 clk_cfg_13_clr;
	u32 reserved12[9];
	u32 clk_cfg_8;		/* 0x100 */
	u32 clk_cfg_9;
	u32 clk_cfg_10;
	u32 clk_cfg_11;
	u32 reserved13[4];
	u32 clk_auddiv_0;	/* 0x120 */
	u32 clk_auddiv_1;
	u32 clk_auddiv_2;
	u32 clk_auddiv_3;
	u32 clk_mjcdiv_0;
	u32 reserved14[51];
	u32 clk_scp_cfg_0;	/* 0x200 */
	u32 clk_scp_cfg_1;
	u32 reserved15[2];
	u32 clk_misc_cfg_0;	/* 0x210 */
	u32 clk_misc_cfg_1;
	u32 clk_misc_cfg_2;
	u32 reserved16[1];
	u32 clk26cali_0;	/* 0x220 */
	u32 clk26cali_1;
	u32 clk26cali_2;
	u32 cksta_reg;
	u32 test_mode_cfg;
	u32 reserved17[53];
	u32 mbist_cfg_0;	/* 0x308 */
	u32 mbist_cfg_1;
	u32 reset_deglitch_key;
	u32 mbist_cfg_3;	/* 0x314 */
};

check_member(mtk_topckgen_regs, clk_cfg_0, 0x40);
check_member(mtk_topckgen_regs, clk_cfg_8, 0x100);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x200);
check_member(mtk_topckgen_regs, mbist_cfg_3, 0x314);

struct mtk_apmixed_regs {
	u32 ap_pll_con0;
	u32 reserved1[1];
	u32 ap_pll_con2;	/* 0x008 */
	u32 ap_pll_con3;
	u32 ap_pll_con4;
	u32 ap_pll_con5;
	u32 ap_pll_con6;
	u32 ap_pll_con7;
	u32 clksq_stb_con0;
	u32 pll_pwr_con0;
	u32 pll_pwr_con1;
	u32 pll_iso_con0;
	u32 pll_iso_con1;
	u32 pll_stb_con0;
	u32 div_stb_con0;
	u32 pll_chg_con0;
	u32 pll_test_con0;
	u32 pll_test_con1;	/* 0x044 */
	u32 reserved2[110];
	u32 armca15pll_con0;	/* 0x200 */
	u32 armca15pll_con1;
	u32 armca15pll_con2;
	u32 armca15pll_pwr_con0;
	u32 armca7pll_con0;
	u32 armca7pll_con1;
	u32 armca7pll_con2;
	u32 armca7pll_pwr_con0;
	u32 mainpll_con0;
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_pwr_con0;
	u32 univpll_con0;
	u32 univpll_con1;
	u32 univpll_con2;
	u32 univpll_pwr_con0;
	u32 mmpll_con0;
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_pwr_con0;
	u32 msdcpll_con0;
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_pwr_con0;
	u32 vencpll_con0;
	u32 vencpll_con1;
	u32 vencpll_con2;
	u32 vencpll_pwr_con0;
	u32 tvdpll_con0;
	u32 tvdpll_con1;
	u32 tvdpll_con2;
	u32 tvdpll_pwr_con0;
	u32 mpll_con0;
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_pwr_con0;
	u32 vcodecpll_con0;
	u32 vcodecpll_con1;
	u32 vcodecpll_con2;
	u32 vcodecpll_pwr_con0;
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
	u32 reserved3[2];
	u32 lvdspll_con0;	/* 0x2d0 */
	u32 lvdspll_con1;
	u32 lvdspll_con2;
	u32 lvdspll_pwr_con0;
	u32 lvdspll_ssc_con0;
	u32 lvdspll_ssc_con1;
	u32 lvdspll_ssc_con2;
	u32 reserved4[1];
	u32 msdcpll2_con0;	/* 0x2f0 */
	u32 msdcpll2_con1;
	u32 msdcpll2_con2;
	u32 msdcpll2_pwr_con0;	/* 0x2fc */
};

check_member(mtk_apmixed_regs, ap_pll_con2, 0x8);
check_member(mtk_apmixed_regs, armca15pll_con0, 0x200);
check_member(mtk_apmixed_regs, msdcpll2_pwr_con0, 0x2fc);

enum {
	PLL_PWR_ON_DELAY = 5,
	PLL_ISO_DELAY = 0,
	PLL_EN_DELAY = 40,
};

enum {
	PCW_INTEGER_BITS = 7,
};

/* PLL rate */
enum {
	ARMCA15PLL_HZ	= 851500 * KHz,
	ARMCA7PLL_HZ	= 1105 * MHz,
	MAINPLL_HZ	= 1092 * MHz,
	UNIVPLL_HZ	= 1248 * MHz,
	MMPLL_HZ	= 455 * MHz,
	MSDCPLL_HZ	= 800 * MHz,
	VENCPLL_HZ	= 660 * MHz,
	TVDPLL_HZ	= 1782 * MHz,
	MPLL_HZ		= 1456 * MHz,
	VCODECPLL_HZ	= 1104 * MHz,
	LVDSPLL_HZ	= 150 * MHz,
	MSDCPLL2_HZ	= 800 * MHz,
	APLL1_HZ	= 180633600,
	APLL2_HZ	= 196608 * KHz,
};

/* top_div rate */
enum {
	AD_HDMITX_CLK_HZ	= TVDPLL_HZ / 12,
	AD_LVDSPLL_CK_HZ	= LVDSPLL_HZ,
	APLL1_CK_HZ		= APLL1_HZ,
	APLL2_CK_HZ		= APLL2_HZ,
	CLK26M_HZ		= 26 * MHz,
	CLKRTC_EXT_HZ		= 32 * KHz,
	MMPLL_CK_HZ		= MMPLL_HZ,
	MSDCPLL_D4_HZ		= MSDCPLL_HZ / 4,
	SYSPLL1_D2_HZ		= MAINPLL_HZ / 4,
	SYSPLL1_D4_HZ		= MAINPLL_HZ / 8,
	SYSPLL2_D2_HZ		= MAINPLL_HZ / 6,
	SYSPLL3_D2_HZ		= MAINPLL_HZ / 10,
	SYSPLL3_D4_HZ		= MAINPLL_HZ / 20,
	SYSPLL_D2_HZ		= MAINPLL_HZ / 2,
	TVDPLL_D2_HZ		= TVDPLL_HZ / 2,
	UNIVPLL1_D2_HZ		= UNIVPLL_HZ / 4,
	UNIVPLL1_D8_HZ		= UNIVPLL_HZ / 16,
	UNIVPLL2_D2_HZ		= UNIVPLL_HZ / 6,
	UNIVPLL2_D4_HZ		= UNIVPLL_HZ / 12,
	UNIVPLL3_D2_HZ		= UNIVPLL_HZ / 10,
	UNIVPLL_D52_HZ		= UNIVPLL_HZ / 52,
	VCODECPLL_CK_HZ		= VCODECPLL_HZ / 3,
	VENCPLL_D2_HZ		= VENCPLL_HZ / 2,
};

/* top_mux rate */
enum {
	AXI_HZ		= UNIVPLL2_D2_HZ,
	MEM_HZ		= CLK26M_HZ,
	DDRPHYCFG_HZ	= CLK26M_HZ,
	MM_HZ		= VENCPLL_D2_HZ,
	PWM_HZ		= CLK26M_HZ,
	VDEC_HZ		= VCODECPLL_CK_HZ,
	VENC_HZ		= VCODECPLL_CK_HZ,
	MFG_HZ		= MMPLL_CK_HZ,
	CAMTG_HZ	= CLK26M_HZ,
	UART_HZ		= CLK26M_HZ,
	SPI_HZ		= SYSPLL3_D2_HZ,
	USB20_HZ	= UNIVPLL1_D8_HZ,
	MSDC30_2_HZ	= MSDCPLL_D4_HZ,
	MSDC30_3_HZ	= MSDCPLL_D4_HZ,
	AUDIO_HZ	= CLK26M_HZ,
	AUD_INTBUS_HZ	= SYSPLL1_D4_HZ,
	PMICSPI_HZ	= CLK26M_HZ,
	SCP_HZ		= SYSPLL1_D2_HZ,
	ATB_HZ		= CLK26M_HZ,
	VENC_LT_HZ	= UNIVPLL1_D2_HZ,
	DPI0_HZ		= TVDPLL_D2_HZ,
	IRDA_HZ		= UNIVPLL2_D4_HZ,
	CCI400_HZ	= SYSPLL_D2_HZ,
	AUD_1_HZ	= APLL1_CK_HZ,
	AUD_2_HZ	= APLL2_CK_HZ,
	MEM_MFG_IN_HZ	= MMPLL_CK_HZ,
	AXI_MFG_IN_HZ	= AXI_HZ,
	SCAM_HZ		= SYSPLL3_D2_HZ,
	SPINFI_IFR_HZ	= CLK26M_HZ,
	HDMI_HZ		= AD_HDMITX_CLK_HZ,
	DPILVDS_HZ	= AD_LVDSPLL_CK_HZ,
	MSDC50_2_H_HZ	= SYSPLL2_D2_HZ,
	HDCP_HZ		= SYSPLL3_D4_HZ,
	HDCP_24M_HZ	= UNIVPLL_D52_HZ,
	RTC_HZ		= CLKRTC_EXT_HZ,
	USB30_HZ	= UNIVPLL3_D2_HZ,
	MSDC50_0_H_HZ	= SYSPLL2_D2_HZ,
	MSDC50_0_HZ	= MSDCPLL_D4_HZ,
	MSDC30_1_HZ	= MSDCPLL_D4_HZ,
};

void mt_pll_post_init(void);
void mt_pll_set_aud_div(u32 rate);
void mt_pll_enable_ssusb_clk(void);
void mt_pll_raise_ca53_freq(u32 freq);
void mt_mem_pll_set_clk_cfg(void);
void mt_mem_pll_config_pre(const struct mt8173_sdram_params *sdram_params);
void mt_mem_pll_config_post(void);
void mt_mem_pll_mux(void);

#endif /* SOC_MEDIATEK_MT8173_PLL_H */
