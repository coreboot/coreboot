/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 4.1
 */

#ifndef SOC_MEDIATEK_MT8188_PLL_H
#define SOC_MEDIATEK_MT8188_PLL_H

#include <device/mmio.h>
#include <soc/pll_common.h>
#include <types.h>

struct mtk_topckgen_regs {
	u32 reserved1[1];
	u32 clk_cfg_update;
	u32 clk_cfg_update1;
	u32 clk_cfg_update2;
	u32 clk_cfg_update3;
	u32 reserved2[3];
	u32 clk_cfg_0;
	u32 clk_cfg_0_set;
	u32 clk_cfg_0_clr;
	u32 clk_cfg_1;
	u32 clk_cfg_1_set;
	u32 clk_cfg_1_clr;
	u32 clk_cfg_2;
	u32 clk_cfg_2_set;
	u32 clk_cfg_2_clr;
	u32 clk_cfg_3;
	u32 clk_cfg_3_set;
	u32 clk_cfg_3_clr;
	u32 clk_cfg_4;
	u32 clk_cfg_4_set;
	u32 clk_cfg_4_clr;
	u32 clk_cfg_5;
	u32 clk_cfg_5_set;
	u32 clk_cfg_5_clr;
	u32 clk_cfg_6;
	u32 clk_cfg_6_set;
	u32 clk_cfg_6_clr;
	u32 clk_cfg_7;
	u32 clk_cfg_7_set;
	u32 clk_cfg_7_clr;
	u32 clk_cfg_8;
	u32 clk_cfg_8_set;
	u32 clk_cfg_8_clr;
	u32 clk_cfg_9;
	u32 clk_cfg_9_set;
	u32 clk_cfg_9_clr;
	u32 clk_cfg_10;
	u32 clk_cfg_10_set;
	u32 clk_cfg_10_clr;
	u32 clk_cfg_11;
	u32 clk_cfg_11_set;
	u32 clk_cfg_11_clr;
	u32 clk_cfg_12;
	u32 clk_cfg_12_set;
	u32 clk_cfg_12_clr;
	u32 clk_cfg_13;
	u32 clk_cfg_13_set;
	u32 clk_cfg_13_clr;
	u32 clk_cfg_14;
	u32 clk_cfg_14_set;
	u32 clk_cfg_14_clr;
	u32 clk_cfg_15;
	u32 clk_cfg_15_set;
	u32 clk_cfg_15_clr;
	u32 clk_cfg_16;
	u32 clk_cfg_16_set;
	u32 clk_cfg_16_clr;
	u32 clk_cfg_17;
	u32 clk_cfg_17_set;
	u32 clk_cfg_17_clr;
	u32 clk_cfg_18;
	u32 clk_cfg_18_set;
	u32 clk_cfg_18_clr;
	u32 clk_cfg_19;
	u32 clk_cfg_19_set;
	u32 clk_cfg_19_clr;
	u32 clk_cfg_20;
	u32 clk_cfg_20_set;
	u32 clk_cfg_20_clr;
	u32 clk_cfg_21;
	u32 clk_cfg_21_set;
	u32 clk_cfg_21_clr;
	u32 clk_cfg_22;
	u32 clk_cfg_22_set;
	u32 clk_cfg_22_clr;
	u32 clk_cfg_23;
	u32 clk_cfg_23_set;
	u32 clk_cfg_23_clr;
	u32 clk_cfg_24;
	u32 clk_cfg_24_set;
	u32 clk_cfg_24_clr;
	u32 clk_cfg_25;
	u32 clk_cfg_25_set;
	u32 clk_cfg_25_clr;
	u32 clk_cfg_26;
	u32 clk_cfg_26_set;
	u32 clk_cfg_26_clr;
	u32 clk_cfg_27;
	u32 clk_cfg_27_set;
	u32 clk_cfg_27_clr;
	u32 clk_cfg_28;
	u32 clk_cfg_28_set;
	u32 clk_cfg_28_clr;
	u32 clk_cfg_29;
	u32 clk_cfg_29_set;
	u32 clk_cfg_29_clr;
	u32 clk_cfg_30;
	u32 clk_cfg_30_set;
	u32 clk_cfg_30_clr;
	u32 clk_cfg_31;
	u32 clk_cfg_31_set;
	u32 clk_cfg_31_clr;
	u32 clk_cfg_32;
	u32 clk_cfg_32_set;
	u32 clk_cfg_32_clr;
	u32 clk_cfg_33;
	u32 clk_cfg_33_set;
	u32 clk_cfg_33_clr;
	u32 clk_cfg_34;
	u32 clk_cfg_34_set;
	u32 clk_cfg_34_clr;
	u32 clk_cfg_35;
	u32 clk_cfg_35_set;
	u32 clk_cfg_35_clr;
	u32 clk_cfg_36;
	u32 clk_cfg_36_set;
	u32 clk_cfg_36_clr;
	u32 clk_cfg_37;
	u32 clk_cfg_37_set;
	u32 clk_cfg_37_clr;
	u32 reserved3[7];
	u32 clk_extck_reg;
	u32 reserved4[1];
	u32 clk_dbg_cfg;
	u32 reserved5[2];
	u32 clk26cali_0;
	u32 clk26cali_1;
	u32 reserved6[3];
	u32 clk_misc_cfg_0;
	u32 reserved7[2];
	u32 clk_misc_cfg_1;
	u32 reserved8[2];
	u32 clk_misc_cfg_2;
	u32 reserved9[2];
	u32 clk_misc_cfg_3;
	u32 reserved10[2];
	u32 clk_misc_cfg_6;
	u32 reserved11[1];
	u32 clk_scp_cfg_0;
};
check_member(mtk_topckgen_regs, clk_cfg_update1, 0x0008);
check_member(mtk_topckgen_regs, clk_cfg_0, 0x0020);
check_member(mtk_topckgen_regs, clk_cfg_10_set, 0x009c);
check_member(mtk_topckgen_regs, clk_cfg_10_clr, 0x00a0);
check_member(mtk_topckgen_regs, clk_cfg_11_clr, 0x00ac);
check_member(mtk_topckgen_regs, clk_extck_reg, 0x0204);
check_member(mtk_topckgen_regs, clk26cali_0, 0x0218);
check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x022c);
check_member(mtk_topckgen_regs, clk_misc_cfg_1, 0x0238);
check_member(mtk_topckgen_regs, clk_misc_cfg_2, 0x0244);
check_member(mtk_topckgen_regs, clk_misc_cfg_3, 0x0250);
check_member(mtk_topckgen_regs, clk_misc_cfg_6, 0x025c);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x264);

struct mtk_apmixed_regs {
	u32 ap_pll_con0;
	u32 ap_pll_con1;
	u32 ap_pll_con2;
	u32 ap_pll_con3;
	u32 reserved12[9];
	u32 apll1_tuner_con0;
	u32 apll2_tuner_con0;
	u32 apll3_tuner_con0;
	u32 apll4_tuner_con0;
	u32 apll5_tuner_con0;
	u32 ref_clk_con0;
	u32 ulposc_ctrl_sel;
	u32 reserved13[109];
	u32 armpll_ll_con0;
	u32 armpll_ll_con1;
	u32 armpll_ll_con2;
	u32 armpll_ll_con3;
	u32 armpll_bl_con0;
	u32 armpll_bl_con1;
	u32 armpll_bl_con2;
	u32 armpll_bl_con3;
	u32 ccipll_con0;
	u32 ccipll_con1;
	u32 ccipll_con2;
	u32 ccipll_con3;
	u32 reserved14[52];
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
	u32 apll3_con0;
	u32 apll3_con1;
	u32 apll3_con2;
	u32 apll3_con3;
	u32 apll3_con4;
	u32 mfgpll_con0;
	u32 mfgpll_con1;
	u32 mfgpll_con2;
	u32 mfgpll_con3;
	u32 reserved15[45];
	u32 apll4_con0;
	u32 apll4_con1;
	u32 apll4_con2;
	u32 apll4_con3;
	u32 apll4_con4;
	u32 apll5_con0;
	u32 apll5_con1;
	u32 apll5_con2;
	u32 apll5_con3;
	u32 apll5_con4;
	u32 adsppll_con0;
	u32 adsppll_con1;
	u32 adsppll_con2;
	u32 adsppll_con3;
	u32 mpll_con0;
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_con3;
	u32 ethpll_con0;
	u32 ethpll_con1;
	u32 ethpll_con2;
	u32 ethpll_con3;
	u32 mainpll_con0;
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_con3;
	u32 reserved16[38];
	u32 univpll_con0;
	u32 univpll_con1;
	u32 univpll_con2;
	u32 univpll_con3;
	u32 msdcpll_con0;
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_con3;
	u32 tvdpll1_con0;
	u32 tvdpll1_con1;
	u32 tvdpll1_con2;
	u32 tvdpll1_con3;
	u32 tvdpll2_con0;
	u32 tvdpll2_con1;
	u32 tvdpll2_con2;
	u32 tvdpll2_con3;
	u32 mmpll_con0;
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_con3;
	u32 imgpll_con0;
	u32 imgpll_con1;
	u32 imgpll_con2;
	u32 imgpll_con3;
	u32 reserved17[39];
	u32 ulposc1_con0;
	u32 ulposc1_con1;
	u32 ulposc1_con2;
};
check_member(mtk_apmixed_regs, ap_pll_con0, 0x0000);
check_member(mtk_apmixed_regs, apll4_tuner_con0, 0x0040);
check_member(mtk_apmixed_regs, ulposc_ctrl_sel, 0x004c);
check_member(mtk_apmixed_regs, armpll_ll_con0, 0x0204);
check_member(mtk_apmixed_regs, armpll_bl_con3, 0x0220);
check_member(mtk_apmixed_regs, ccipll_con3, 0x0230);
check_member(mtk_apmixed_regs, apll1_con3, 0x0310);
check_member(mtk_apmixed_regs, apll2_con2, 0x0320);
check_member(mtk_apmixed_regs, apll3_con1, 0x0330);
check_member(mtk_apmixed_regs, mainpll_con0, 0x045c);
check_member(mtk_apmixed_regs, univpll_con0, 0x0504);
check_member(mtk_apmixed_regs, mfgpll_con0, 0x0340);
check_member(mtk_apmixed_regs, apll4_con3, 0x0410);
check_member(mtk_apmixed_regs, apll5_con2, 0x0420);
check_member(mtk_apmixed_regs, mpll_con1, 0x0440);
check_member(mtk_apmixed_regs, mainpll_con1, 0x0460);
check_member(mtk_apmixed_regs, univpll_con3, 0x0510);
check_member(mtk_apmixed_regs, msdcpll_con3, 0x0520);
check_member(mtk_apmixed_regs, tvdpll2_con3, 0x0540);
check_member(mtk_apmixed_regs, mmpll_con3, 0x0550);
check_member(mtk_apmixed_regs, imgpll_con3, 0x0560);
check_member(mtk_apmixed_regs, ulposc1_con0, 0x0600);
check_member(mtk_apmixed_regs, ulposc1_con1, 0x0604);
check_member(mtk_apmixed_regs, ulposc1_con2, 0x0608);

struct mt8188_pericfg_ao_regs {
	u32 reserved1[4];
	u32 peri_module_sw_cg_0_set;	/* 0x0010 */
	u32 peri_module_sw_cg_0_clr;	/* 0x0014 */
};
check_member(mt8188_pericfg_ao_regs, peri_module_sw_cg_0_set, 0x0010);
check_member(mt8188_pericfg_ao_regs, peri_module_sw_cg_0_clr, 0x0014);
static struct mt8188_pericfg_ao_regs *const mt8188_pericfg_ao = (void *)PERICFG_AO_BASE;

enum {
	PLL_CKSQ_ON_DELAY = 100,
	PLL_PWR_ON_DELAY = 30,
	PLL_ISO_DELAY = 1,
	PLL_EN_DELAY = 20,
};

enum {
	PCW_INTEGER_BITS = 8,
};

enum {
	MT8188_PLL_EN = BIT(9),
	MT8188_APLL5_EN = BIT(9) | BIT(20),
	GLITCH_FREE_EN = BIT(12),
	PLL_DIV_EN = BIT(24) | BIT(25) | BIT(26) | BIT(27) |
		     BIT(28) | BIT(29) | BIT(30) | BIT(31),
};

enum {
	MCU_DIV_MASK = 0x1f << 17,
	MCU_DIV_1 = 0x8 << 17,

	MCU_MUX_MASK = 0x3 << 9,
	MCU_MUX_SRC_PLL = 0x1 << 9,
	MCU_MUX_SRC_26M = 0x0 << 9,
};

/* PLL rate */
enum {
	ARMPLL_LL_HZ	= 500 * MHz,
	ARMPLL_BL_HZ	= 650 * MHz,
	CCIPLL_HZ	= 520 * MHz,
	ETHPLL_HZ	= 500 * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	TVDPLL1_HZ	= 594 * MHz,
	TVDPLL2_HZ	= 594 * MHz,
	MMPLL_HZ	= 2750UL * MHz,
	MAINPLL_HZ	= 2184UL * MHz,
	IMGPLL_HZ	= 660 * MHz,
	UNIVPLL_HZ	= 2496UL * MHz,
	ADSPPLL_HZ	= 800 * MHz,
	APLL1_HZ	= 196608 * KHz,
	APLL2_HZ	= 180633600,
	APLL3_HZ	= 196608 * KHz,
	APLL4_HZ	= 196608 * KHz,
	APLL5_HZ	= 196608 * KHz,
	MFGPLL_HZ	= 390 * MHz,
};

/* top_div rate */
enum {
	CLK26M_HZ		= 26 * MHz,
	UNIVPLL_D6_D2_HZ	= UNIVPLL_HZ / 6 / 2,
};

/* top_mux rate */
enum {
	SPI_HZ		= UNIVPLL_D6_D2_HZ,
	UART_HZ		= CLK26M_HZ,
};

DEFINE_BITFIELD(CLK_DBG_CFG_ABIST_CK_SEL, 14, 8)
DEFINE_BITFIELD(CLK_DBG_CFG_CKGEN_CK_SEL, 23, 16)
DEFINE_BITFIELD(CLK_DBG_CFG_METER_CK_SEL, 1, 0)
DEFINE_BITFIELD(CLK_DBG_CFG_CKGEN_EN, 24, 24)
DEFINE_BITFIELD(CLK_MISC_CFG_0_METER_DIV, 31, 24)
DEFINE_BITFIELD(CLK26CALI_0_TRIGGER, 4, 4)
DEFINE_BITFIELD(CLK26CALI_1_LOAD_CNT, 25, 16)

enum {
	INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK = (0x1f << 12) | (0x1 << 17) | (0x1 << 18),
	INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON = (0x10 << 12) | (0x1 << 17) | (0x0 << 18),
	INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK = (0x1 << 0) |
					      (0x1 << 1) |
					      (0x1 << 3) |
					      (0x1 << 4) |
					      (0x1f << 5) |
					      (0x1 << 20) |
					      (0x1 << 23) |
					      (0x1 << 30),
	INFRACFG_AO_INFRA_BUS_DCM_REG0_ON = (0x1 << 0) |
					    (0x1 << 1) |
					    (0x0 << 3) |
					    (0x0 << 4) |
					    (0x10 << 5) |
					    (0x1 << 20) |
					    (0x1 << 23) |
					    (0x1 << 30),
	INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK = (0xf << 0),
	INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON = (0x0 << 0),
	INFRACFG_AO_PERI_BUS_DCM_REG0_MASK = (0x1 << 0) |
					     (0x1 << 1) |
					     (0x1 << 3) |
					     (0x1 << 4) |
					     (0x1f << 5) |
					     (0x1f << 15) |
					     (0x1 << 20) |
					     (0x1 << 21),
	INFRACFG_AO_PERI_BUS_DCM_REG0_ON = (0x1 << 0) |
					   (0x1 << 1) |
					   (0x0 << 3) |
					   (0x0 << 4) |
					   (0x1f << 5) |
					   (0x1f << 15) |
					   (0x1 << 20) |
					   (0x1 << 21),
	INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK = (0x1 << 29) | (0x1 << 31),
	INFRACFG_AO_PERI_MODULE_DCM_REG0_ON = (0x1 << 29) | (0x1 << 31),
};

#endif /* SOC_MEDIATEK_MT8188_PLL_H */
