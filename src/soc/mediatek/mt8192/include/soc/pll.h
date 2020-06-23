/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_PLL_H
#define SOC_MEDIATEK_MT8192_PLL_H

#include <device/mmio.h>
#include <types.h>
#include <soc/pll_common.h>

struct mtk_topckgen_regs {
	u32 clk_mode;
	u32 clk_cfg_update;
	u32 clk_cfg_update1;
	u32 clk_cfg_update2;
	u32 clk_cfg_0;
	u32 clk_cfg_0_set;
	u32 clk_cfg_0_clr;
	u32 reserved1[1];
	u32 clk_cfg_1;	/* 0x0020 */
	u32 clk_cfg_1_set;
	u32 clk_cfg_1_clr;
	u32 reserved2[1];
	u32 clk_cfg_2;	/* 0x0030 */
	u32 clk_cfg_2_set;
	u32 clk_cfg_2_clr;
	u32 reserved3[1];
	u32 clk_cfg_3;	/* 0x0040 */
	u32 clk_cfg_3_set;
	u32 clk_cfg_3_clr;
	u32 reserved4[1];
	u32 clk_cfg_4;	/* 0x0050 */
	u32 clk_cfg_4_set;
	u32 clk_cfg_4_clr;
	u32 reserved5[1];
	u32 clk_cfg_5;	/* 0x0060 */
	u32 clk_cfg_5_set;
	u32 clk_cfg_5_clr;
	u32 reserved6[1];
	u32 clk_cfg_6;	/* 0x0070 */
	u32 clk_cfg_6_set;
	u32 clk_cfg_6_clr;
	u32 reserved7[1];
	u32 clk_cfg_7;	/* 0x0080 */
	u32 clk_cfg_7_set;
	u32 clk_cfg_7_clr;
	u32 reserved8[1];
	u32 clk_cfg_8;	/* 0x0090 */
	u32 clk_cfg_8_set;
	u32 clk_cfg_8_clr;
	u32 reserved9[1];
	u32 clk_cfg_9;	/* 0x00a0 */
	u32 clk_cfg_9_set;
	u32 clk_cfg_9_clr;
	u32 reserved10[1];
	u32 clk_cfg_10;	/* 0x00b0 */
	u32 clk_cfg_10_set;
	u32 clk_cfg_10_clr;
	u32 reserved11[1];
	u32 clk_cfg_11;	/* 0x00c0 */
	u32 clk_cfg_11_set;
	u32 clk_cfg_11_clr;
	u32 reserved12[1];
	u32 clk_cfg_12;	/* 0x00d0 */
	u32 clk_cfg_12_set;
	u32 clk_cfg_12_clr;
	u32 reserved13[1];
	u32 clk_cfg_13;	/* 0x00e0 */
	u32 clk_cfg_13_set;
	u32 clk_cfg_13_clr;
	u32 reserved14[1];
	u32 clk_cfg_14;	/* 0x00f0 */
	u32 clk_cfg_14_set;
	u32 clk_cfg_14_clr;
	u32 reserved15[1];
	u32 clk_cfg_15;	/* 0x0100 */
	u32 clk_cfg_15_set;
	u32 clk_cfg_15_clr;
	u32 reserved16[1];
	u32 clk_cfg_16;	/* 0x0110 */
	u32 clk_cfg_16_set;
	u32 clk_cfg_16_clr;
	u32 reserved17[9];
	u32 clk_misc_cfg_0;	/* 0x0140 */
	u32 reserved18[3];
	u32 clk_misc_cfg_1;	/* 0x0150 */
	u32 reserved19[10];
	u32 clk_dbg_cfg;	/* 0x017c */
	u32 reserved20[32];
	u32 clk_scp_cfg_0;	/* 0x0200 */
	u32 reserved21[3];
	u32 clk_scp_cfg_1;	/* 0x0210 */
	u32 reserved22[3];
	u32 clk26cali_0;	/* 0x0220 */
	u32 clk26cali_1;
	u32 reserved23[2];
	u32 cksta_reg;	/* 0x0230 */
	u32 cksta_reg1;
	u32 cksta_reg2;
	u32 reserved24[49];
	u32 clkmon_clk_sel_reg;	/* 0x0300 */
	u32 clkmon_k1_reg;
	u32 reserved25[6];
	u32 clk_auddiv_0;	/* 0x0320 */
	u32 clk_auddiv_1;
	u32 clk_auddiv_2;
	u32 aud_top_cfg;
	u32 aud_top_mon;
	u32 clk_auddiv_3;
	u32 clk_auddiv_4;
	u32 reserved26[113];
	u32 clk_extck_reg;	/* 0x0500 */
};

check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x0140);
check_member(mtk_topckgen_regs, clk_misc_cfg_1, 0x0150);
check_member(mtk_topckgen_regs, clk_dbg_cfg, 0x017c);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x0200);
check_member(mtk_topckgen_regs, clk_scp_cfg_1, 0x0210);
check_member(mtk_topckgen_regs, clk26cali_0, 0x0220);
check_member(mtk_topckgen_regs, cksta_reg, 0x0230);
check_member(mtk_topckgen_regs, clkmon_clk_sel_reg, 0x0300);
check_member(mtk_topckgen_regs, clk_auddiv_0, 0x0320);
check_member(mtk_topckgen_regs, clk_extck_reg, 0x0500);

struct mtk_apmixed_regs {
	u32 ap_pll_con0;
	u32 ap_pll_con1;
	u32 ap_pll_con2;
	u32 ap_pll_con3;
	u32 ap_pll_con4;
	u32 ap_pll_con5;
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
	u32 apll1_tuner_con0;
	u32 apll2_tuner_con0;
	u32 reserved0[2];
	u32 pllon_con0;
	u32 pllon_con1;
	u32 pllon_con2;
	u32 pllon_con3;
	u32 reserved1[104];
	u32 ap_pllgp1_con0;	/* 0x0200 */
	u32 ap_pllgp1_con1;
	u32 armpll_ll_con0;
	u32 armpll_ll_con1;
	u32 armpll_ll_con2;
	u32 armpll_ll_con3;
	u32 armpll_bl0_con0;
	u32 armpll_bl_con1;
	u32 armpll_bl_con2;
	u32 armpll_bl_con3;
	u32 armpll_bl1_con0;
	u32 armpll_bl1_con1;
	u32 armpll_bl1_con2;
	u32 armpll_bl1_con3;
	u32 armpll_bl2_con0;
	u32 armpll_bl2_con1;
	u32 armpll_bl2_con2;
	u32 armpll_bl2_con3;
	u32 armpll_bl3_con0;
	u32 armpll_bl3_con1;
	u32 armpll_bl3_con2;
	u32 armpll_bl3_con3;
	u32 ccipll_con0;
	u32 ccipll_con1;
	u32 ccipll_con2;
	u32 ccipll_con3;
	u32 mfgpll_con0;
	u32 mfgpll_con1;
	u32 mfgpll_con2;
	u32 mfgpll_con3;
	u32 ap_pllgp1_con2;
	u32 reserved2[13];
	u32 ulposc1_con0;
	u32 ulposc1_con1;
	u32 ulposc1_con2;
	u32 reserved3[17];
	u32 ap_pllgp2_con0;	/* 0x0300 */
	u32 ap_pllgp2_con1;
	u32 univpll_con0;
	u32 univpll_con1;
	u32 univpll_con2;
	u32 univpll_con3;
	u32 apll1_con0;
	u32 apll1_con1;
	u32 apll1_con2;
	u32 apll1_con3;
	u32 apll1_con4;
	u32 apll2_con0;
	u32 apll2_con1;
	u32 apll2_con2;
	u32 apll2_con3;
	u32 apll2_con4;
	u32 mainpll_con0;
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_con3;
	u32 msdcpll_con0;
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_con3;
	u32 mmpll_con0;
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_con3;
	u32 adsppll_con0;
	u32 adsppll_con1;
	u32 adsppll_con2;
	u32 adsppll_con3;
	u32 tvdpll_con0;
	u32 tvdpll_con1;
	u32 tvdpll_con2;
	u32 tvdpll_con3;
	u32 mpll_con0;
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_con3;
	u32 apupll_con0;
	u32 apupll_con1;
	u32 apupll_con2;
	u32 apupll_con3;
	u32 ap_pllgp3_con0;
	u32 npupll_con0;
	u32 npupll_con1;
	u32 npupll_con2;
	u32 npupll_con3;
	u32 usbpll_con0;
	u32 usbpll_con1;
	u32 usbpll_con2;
};

check_member(mtk_apmixed_regs, ap_pllgp1_con0, 0x0200);
check_member(mtk_apmixed_regs, ap_pllgp2_con0, 0x0300);
check_member(mtk_apmixed_regs, usbpll_con2, 0x03cc);

#define MPLL_CON1_FREQ		0x84200000

enum {
	USBPLL_EN = 0x1 << 2,

	PLL_DIV_EN = 0xff << 24,
};

enum {
	MCU_DIV_MASK = 0x1f << 17,
	MCU_DIV_1 = 0x8 << 17,

	MCU_MUX_MASK = 0x3 << 9,
	MCU_MUX_SRC_PLL = 0x1 << 9,
	MCU_MUX_SRC_DIV_PLL1 = 0x2 << 9,
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
	ARMPLL_LL_HZ	= 1075 * MHz,
	ARMPLL_BL_HZ	= 774 * MHz,
	CCIPLL_HZ	= 730 * MHz,
	MAINPLL_HZ	= 2184UL * MHz,
	UNIVPLL_HZ	= 2496UL * MHz,
	USBPLL_HZ	= 192UL * 13 * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	MMPLL_HZ	= 2750UL * MHz,
	ADSPPLL_HZ	= 750 * MHz,
	MFGPLL_HZ	= 358 * MHz,
	TVDPLL_HZ	= 594 * MHz,
	APLL1_HZ	= 180633600,
	APLL2_HZ	= 196608 * KHz,
};

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
	MAINPLL_D5_HZ   = MAINPLL_HZ / 5,
	MAINPLL_D5_D4_HZ = MAINPLL_D5_HZ / 4,
};

/* top_mux rate */
enum {
	SPI_HZ		= MAINPLL_D5_D4_HZ,
	UART_HZ		= CLK26M_HZ,
};

DEFINE_BITFIELD(PLLGP1_LVRREF, 18, 17)
DEFINE_BITFIELD(PLLGP2_LVRREF, 10, 9)

DEFINE_BITFIELD(CLK_DBG_CFG_ABIST_CK_SEL, 21, 16)
DEFINE_BITFIELD(CLK_DBG_CFG_CKGEN_CK_SEL, 13, 8)
DEFINE_BITFIELD(CLK_DBG_CFG_METER_CK_SEL, 1, 0)
DEFINE_BITFIELD(CLK_MISC_CFG_0_METER_DIV, 31, 24)
DEFINE_BITFIELD(CLK26CALI_0_TRIGGER, 4, 4)
DEFINE_BITFIELD(CLK26CALI_1_LOAD_CNT, 25, 16)

DEFINE_BIT(MPLL_IOS_SEL, 2)
DEFINE_BIT(MPLL_EN_SEL, 11)
DEFINE_BIT(MPLL_PWR_SEL, 20)
DEFINE_BIT(MPLL_BY_ISO_DLY, 2)
DEFINE_BIT(MPLL_BY_PWR_DLY, 2)

DEFINE_BITFIELD(WDT_SWSYSRST_KEY, 31, 24)
DEFINE_BITFIELD(WDT_SWSYSRST_CONN_MCU, 12, 12)

enum {
	INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK = (0x1f << 12) | (0x1 << 17) | (0x1 << 18),
	INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON = (0x10 << 12) | (0x1 << 17) | (0x0 << 18),
	INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK = (0x1 << 0) |
					      (0x1 << 1) |
					      (0x1 << 3) |
					      (0x1 << 4) |
					      (0x1f << 5) |
					      (0x1f << 10) |
					      (0x1 << 20) |
					      (0x1 << 23) |
					      (0x1 << 30),
	INFRACFG_AO_INFRA_BUS_DCM_REG0_ON = (0x1 << 0) |
					    (0x1 << 1) |
					    (0x0 << 3) |
					    (0x0 << 4) |
					    (0x10 << 5) |
					    (0x1 << 10) |
					    (0x1 << 20) |
					    (0x1 << 23) |
					    (0x1 << 30),
	INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_MASK = (0x1 << 8),
	INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_ON = (0x1 << 8),
	INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_MASK = (0x1 << 8),
	INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_ON = (0x0 << 8),
	INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK = (0xf << 0),
	INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON = (0x0 << 0),
	INFRACFG_AO_PERI_BUS_DCM_REG0_MASK = (0x1 << 0) |
					     (0x1 << 1) |
					     (0x1 << 3) |
					     (0x1 << 4) |
					     (0x1f << 5) |
					     (0x1f << 10) |
					     (0x1f << 15) |
					     (0x1 << 20) |
					     (0x1 << 21),
	INFRACFG_AO_PERI_BUS_DCM_REG0_ON = (0x1 << 0) |
					   (0x1 << 1) |
					   (0x0 << 3) |
					   (0x0 << 4) |
					   (0x1f << 5) |
					   (0x0 << 10) |
					   (0x1f << 15) |
					   (0x1 << 20) |
					   (0x1 << 21),
	INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK = (0x1 << 29) | (0x1 << 31),
	INFRACFG_AO_PERI_MODULE_DCM_REG0_ON = (0x1 << 29) | (0x1 << 31),
};

#endif /* SOC_MEDIATEK_MT8192_PLL_H */
