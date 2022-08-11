/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_MT8195_PLL_H
#define SOC_MEDIATEK_MT8195_PLL_H

#include <device/mmio.h>
#include <types.h>
#include <soc/pll_common.h>

struct mtk_topckgen_regs {
	u32 reserved1[1];
	u32 clk_cfg_update;	/* 0x0004 */
	u32 clk_cfg_update1;
	u32 clk_cfg_update2;
	u32 clk_cfg_update3;
	u32 clk_cfg_update4;
	u32 reserved2[2];
	u32 clk_cfg_0;	/* 0x0020 */
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
	u32 clk_extck_reg;	/* 0x0204 */
	u32 reserved4[1];
	u32 clk_dbg_cfg;	/* 0x020c */
	u32 reserved5[2];
	u32 clk26cali_0;	/* 0x0218 */
	u32 clk26cali_1;
	u32 reserved6[3];
	u32 clk_misc_cfg_0;	/* 0x022c */
	u32 reserved7[2];
	u32 clk_misc_cfg_1;	/* 0x0238 */
	u32 reserved8[2];
	u32 clk_misc_cfg_2;	/* 0x0244 */
	u32 reserved9[2];
	u32 clk_misc_cfg_3;	/* 0x0250 */
	u32 reserved10[2];
	u32 clk_misc_cfg_6;	/* 0x025c */
	u32 reserved11[1];
	u32 clk_scp_cfg_0;	/* 0x0264 */
	u32 reserved12[13];
	u32 clkmon_clk_sel;	/* 0x029c */
	u32 clkmon_k1;
	u32 reserved13[6];
	u32 cksta_reg_0;	/* 0x02bc */
	u32 cksta_reg_1;
	u32 cksta_reg_2;
	u32 cksta_reg_3;
	u32 cksta_reg_4;
	u32 reserved14[20];
	u32 clk_auddiv_0;	/* 0x0320 */
	u32 clk_auddiv_1;
	u32 clk_auddiv_2;
	u32 aud_top_cfg;
	u32 aud_top_mon;
	u32 clk_auddiv_3;
	u32 clk_auddiv_4;
};

check_member(mtk_topckgen_regs, clk_cfg_0, 0x0020);
check_member(mtk_topckgen_regs, clk_cfg_11_clr, 0x00ac);
check_member(mtk_topckgen_regs, clk_extck_reg, 0x0204);
check_member(mtk_topckgen_regs, clk26cali_0, 0x0218);
check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x022c);
check_member(mtk_topckgen_regs, clk_misc_cfg_1, 0x0238);
check_member(mtk_topckgen_regs, clk_misc_cfg_2, 0x0244);
check_member(mtk_topckgen_regs, clk_misc_cfg_3, 0x0250);
check_member(mtk_topckgen_regs, clk_misc_cfg_6, 0x025c);
check_member(mtk_topckgen_regs, clkmon_clk_sel, 0x029c);
check_member(mtk_topckgen_regs, cksta_reg_0, 0x02bc);
check_member(mtk_topckgen_regs, clk_auddiv_0, 0x0320);
check_member(mtk_topckgen_regs, clk_auddiv_4, 0x0338);

struct mtk_apmixed_regs {
	u32 ap_pll_con0;
	u32 ap_pll_con1;
	u32 ap_pll_con2;
	u32 ap_pll_con3;
	u32 ap_pll_con4;
	u32 ap_pll_con5;
	u32 clksq_stb_con0;
	u32 clksq_stb_con1;
	u32 armpll_ll_con0;
	u32 armpll_ll_con1;
	u32 armpll_ll_con2;
	u32 armpll_ll_con3;
	u32 ccipll_con0;
	u32 pll_chg_con0;
	u32 reserved1[6];
	u32 pllon_con0;	/* 0x0050 */
	u32 pllon_con1;
	u32 pllon_con2;
	u32 pllon_con3;
	u32 reserved2[4];
	u32 armpll_bl_con0;	/* 0x0070 */
	u32 armpll_bl_con1;
	u32 armpll_bl_con2;
	u32 armpll_bl_con3;
	u32 armpll_bl_con4;
	u32 reserved3[3];
	u32 ap_pllgp2_con0;	/* 0x0090 */
	u32 reserved4[3];
	u32 tvdpll1_con0;	/* 0x00a0 */
	u32 tvdpll1_con1;
	u32 tvdpll1_con2;
	u32 tvdpll1_con3;
	u32 tvdpll1_con4;
	u32 reserved5[3];
	u32 tvdpll2_con0;	/* 0x00c0 */
	u32 tvdpll2_con1;
	u32 tvdpll2_con2;
	u32 tvdpll2_con3;
	u32 tvdpll2_con4;
	u32 reserved6[3];
	u32 mmpll_con0;	/* 0x00e0 */
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_con3;
	u32 mmpll_con4;
	u32 reserved7[3];
	u32 imgpll_con0;	/* 0x0100 */
	u32 imgpll_con1;
	u32 imgpll_con2;
	u32 imgpll_con3;
	u32 imgpll_con4;
	u32 reserved8[3];
	u32 ap_pllgp3_con0;	/* 0x0120 */
	u32 reserved9[11];
	u32 dgipll_con0;	/* 0x0150 */
	u32 dgipll_con1;
	u32 dgipll_con2;
	u32 dgipll_con3;
	u32 dgipll_con4;
	u32 reserved10[11];
	u32 respll_con0;	/* 0x0190 */
	u32 respll_con1;
	u32 respll_con2;
	u32 respll_con3;
	u32 usb1pll_con0;
	u32 usb1pll_con1;
	u32 usb1pll_con2;
	u32 usb1pll_con3;
	u32 usb1pll_con4;
	u32 reserved11[7];
	u32 mainpll_con0;	/* 0x01d0 */
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_con3;
	u32 mainpll_con4;
	u32 reserved12[3];
	u32 univpll_con0;	/* 0x01f0 */
	u32 univpll_con1;
	u32 univpll_con2;
	u32 univpll_con3;
	u32 ap_pllgp1_con0;
	u32 reserved13[43];
	u32 ulposc1_con0;	/* 0x02b0 */
	u32 ulposc1_con1;
	u32 reserved14[2];
	u32 ulposc2_con0;	/* 0x02c0 */
	u32 ulposc2_con1;
	u32 reserved15[22];
	u32 respll_con4;	/* 0x0320 */
	u32 reserved16[3];
	u32 ap_pllgp4_con0;	/* 0x0330 */
	u32 reserved17[3];
	u32 mfgpll_con0;	/* 0x0340 */
	u32 mfgpll_con1;
	u32 mfgpll_con2;
	u32 mfgpll_con3;
	u32 mfgpll_con4;
	u32 reserved18[3];
	u32 ethpll_con0;	/* 0x0360 */
	u32 ethpll_con1;
	u32 ethpll_con2;
	u32 ethpll_con3;
	u32 ethpll_con4;
	u32 reserved19[7];
	u32 nnapll_con0;	/* 0x0390 */
	u32 nnapll_con1;
	u32 nnapll_con2;
	u32 nnapll_con3;
	u32 nnapll_con4;
	u32 reserved20[39];
	u32 ap_auxadc_con0;	/* 0x0440 */
	u32 ap_auxadc_con1;
	u32 ap_auxadc_con2;
	u32 ap_auxadc_con3;
	u32 ap_auxadc_con4;
	u32 ap_auxadc_con5;
	u32 reserved21[6];
	u32 apll1_tuner_con0;	/* 0x0470 */
	u32 apll2_tuner_con0;
	u32 apll3_tuner_con0;
	u32 apll4_tuner_con0;
	u32 apll5_tuner_con0;
	u32 reserved22[15];
	u32 pll_pwr_con0;	/* 0x04c0 */
	u32 pll_pwr_con1;
	u32 reserved23[2];
	u32 pll_iso_con0;	/* 0x04d0 */
	u32 pll_iso_con1;
	u32 reserved24[2];
	u32 pll_stb_con0;	/* 0x04e0 */
	u32 reserved25[3];
	u32 div_stb_con0;	/* 0x04f0 */
	u32 reserved26[3];
	u32 ap_abist_mon_con0;	/* 0x0500 */
	u32 ap_abist_mon_con1;
	u32 ap_abist_mon_con2;
	u32 ap_abist_mon_con3;
	u32 occscan_con0;
	u32 occscan_con1;
	u32 occscan_con2;
	u32 occscan_con3;
	u32 mcu_occscan_con0;
	u32 mcu_occscan_con1;
	u32 reserved27[6];
	u32 cksys_occ_sel0;	/* 0x0540 */
	u32 cksys_occ_sel1;
	u32 cksys_occ_sel2;
	u32 cksys_occ_sel3;
	u32 cksys_occ_sel4;
	u32 cksys_occ_sel5;
	u32 cksys_occ_sel6;
	u32 cksys_occ_sel7;
	u32 cksys_occ_tstsel0;
	u32 cksys_occ_tstsel1;
	u32 cksys_occ_tstsel2;
	u32 cksys_occ_tstsel3;
	u32 cksys_occ_tstsel4;
	u32 reserved28[3];
	u32 clkdiv_con0;	/* 0x0580 */
	u32 clkdiv_con1;
	u32 rsv_rw0_con4;
	u32 reserved29[1];
	u32 rsv_rw0_con0;	/* 0x0590 */
	u32 rsv_rw0_con1;
	u32 rsv_rw0_con2;
	u32 rsv_rw0_con3;
	u32 rsv_ro_con0;
	u32 rsv_ro_con1;
	u32 rsv_ro_con2;
	u32 rsv_ro_con3;
	u32 reserved30[20];
	u32 armpll_ll_con4;	/* 0x0600 */
	u32 reserved31[12];
	u32 ccipll_con1;	/* 0x0634 */
	u32 ccipll_con2;
	u32 ccipll_con3;
	u32 ccipll_con4;
	u32 reserved32[47];
	u32 univpll_con4;	/* 0x0700 */
	u32 reserved33[3];
	u32 msdcpll_con0;	/* 0x0710 */
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_con3;
	u32 msdcpll_con4;
	u32 reserved34[7];
	u32 apll4_con0;	/* 0x0740 */
	u32 apll4_con1;
	u32 apll4_con2;
	u32 apll4_con3;
	u32 apll4_con4;
	u32 reserved35[3];
	u32 apll3_con0;	/* 0x0760 */
	u32 apll3_con1;
	u32 apll3_con2;
	u32 apll3_con3;
	u32 apll3_con4;
	u32 reserved36[3];
	u32 apll2_con0;	/* 0x0780 */
	u32 apll2_con1;
	u32 apll2_con2;
	u32 apll2_con3;
	u32 apll2_con4;
	u32 reserved37[3];
	u32 apll5_con0;	/* 0x07a0 */
	u32 apll5_con1;
	u32 apll5_con2;
	u32 apll5_con3;
	u32 apll5_con4;
	u32 reserved38[3];
	u32 apll1_con0;	/* 0x07c0 */
	u32 apll1_con1;
	u32 apll1_con2;
	u32 apll1_con3;
	u32 apll1_con4;
	u32 reserved39[3];
	u32 adsppll_con0;	/* 0x07e0 */
	u32 adsppll_con1;
	u32 adsppll_con2;
	u32 adsppll_con3;
	u32 adsppll_con4;
	u32 reserved40[3];
	u32 mpll_con0;	/* 0x0800 */
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_con3;
	u32 mpll_con4;
	u32 reserved41[23];
	u32 hdmipll2_con0;	/* 0x0870 */
	u32 hdmipll2_con1;
	u32 hdmipll2_con2;
	u32 hdmipll2_con3;
	u32 hdmipll2_con4;
	u32 reserved42[3];
	u32 vdecpll_con0;	/* 0x0890 */
	u32 vdecpll_con1;
	u32 vdecpll_con2;
	u32 vdecpll_con3;
	u32 vdecpll_con4;
	u32 reserved43[7];
	u32 hdmipll1_con0;	/* 0x08c0 */
	u32 hdmipll1_con1;
	u32 hdmipll1_con2;
	u32 hdmipll1_con3;
	u32 hdmipll1_con4;
	u32 reserved44[3];
	u32 hdmirx_apll_con0;	/* 0x08e0 */
	u32 hdmirx_apll_con1;
	u32 hdmirx_apll_con2;
	u32 hdmirx_apll_con3;
	u32 hdmirx_apll_con4;
	u32 reserved45[291];
	u32 occscan_con4;	/* 0x0d80 */
	u32 occscan_con5;
	u32 occscan_con6;
	u32 occscan_con7;
	u32 occscan_con8;
	u32 occscan_con9;
	u32 occscan_con10;
	u32 occscan_con11;
	u32 occscan_con12;
	u32 occscan_con13;
	u32 occscan_con14;
	u32 occscan_con15;
	u32 ref_clk_con0;
	u32 reserved46[3];
	u32 apll1_con5;	/* 0x0dc0 */
	u32 apll2_con5;
	u32 apll3_con5;
	u32 apll4_con5;
	u32 apll5_con5;
	u32 hdmirx_apll_con5;
};

check_member(mtk_apmixed_regs, pllon_con0, 0x0050);
check_member(mtk_apmixed_regs, armpll_bl_con0, 0x0070);
check_member(mtk_apmixed_regs, ap_pllgp2_con0, 0x0090);
check_member(mtk_apmixed_regs, tvdpll1_con0, 0x00a0);
check_member(mtk_apmixed_regs, tvdpll2_con0, 0x00c0);
check_member(mtk_apmixed_regs, mmpll_con0, 0x00e0);
check_member(mtk_apmixed_regs, imgpll_con0, 0x0100);
check_member(mtk_apmixed_regs, ap_pllgp3_con0, 0x0120);
check_member(mtk_apmixed_regs, dgipll_con0, 0x0150);
check_member(mtk_apmixed_regs, respll_con0, 0x0190);
check_member(mtk_apmixed_regs, mainpll_con0, 0x01d0);
check_member(mtk_apmixed_regs, univpll_con0, 0x01f0);
check_member(mtk_apmixed_regs, ulposc1_con0, 0x02b0);
check_member(mtk_apmixed_regs, ulposc2_con0, 0x02c0);
check_member(mtk_apmixed_regs, respll_con4, 0x0320);
check_member(mtk_apmixed_regs, ap_pllgp4_con0, 0x0330);
check_member(mtk_apmixed_regs, mfgpll_con0, 0x0340);
check_member(mtk_apmixed_regs, ethpll_con0, 0x0360);
check_member(mtk_apmixed_regs, nnapll_con0, 0x0390);
check_member(mtk_apmixed_regs, ap_auxadc_con0, 0x0440);
check_member(mtk_apmixed_regs, apll1_tuner_con0, 0x0470);
check_member(mtk_apmixed_regs, pll_pwr_con0, 0x04c0);
check_member(mtk_apmixed_regs, pll_iso_con0, 0x04d0);
check_member(mtk_apmixed_regs, pll_stb_con0, 0x04e0);
check_member(mtk_apmixed_regs, div_stb_con0, 0x04f0);
check_member(mtk_apmixed_regs, ap_abist_mon_con0, 0x0500);
check_member(mtk_apmixed_regs, cksys_occ_sel0, 0x0540);
check_member(mtk_apmixed_regs, clkdiv_con0, 0x0580);
check_member(mtk_apmixed_regs, armpll_ll_con4, 0x0600);
check_member(mtk_apmixed_regs, ccipll_con1, 0x0634);
check_member(mtk_apmixed_regs, univpll_con4, 0x0700);
check_member(mtk_apmixed_regs, msdcpll_con0, 0x0710);
check_member(mtk_apmixed_regs, apll4_con0, 0x0740);
check_member(mtk_apmixed_regs, apll3_con0, 0x0760);
check_member(mtk_apmixed_regs, apll2_con0, 0x0780);
check_member(mtk_apmixed_regs, apll5_con0, 0x07a0);
check_member(mtk_apmixed_regs, apll1_con0, 0x07c0);
check_member(mtk_apmixed_regs, adsppll_con0, 0x07e0);
check_member(mtk_apmixed_regs, mpll_con0, 0x0800);
check_member(mtk_apmixed_regs, hdmipll2_con0, 0x0870);
check_member(mtk_apmixed_regs, vdecpll_con0, 0x0890);
check_member(mtk_apmixed_regs, hdmipll1_con0, 0x08c0);
check_member(mtk_apmixed_regs, hdmirx_apll_con0, 0x08e0);
check_member(mtk_apmixed_regs, occscan_con4, 0x0d80);
check_member(mtk_apmixed_regs, apll1_con5, 0x0dc0);
check_member(mtk_apmixed_regs, hdmirx_apll_con5, 0x0dd4);

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
	MT8195_PLL_EN = 0x1 << 9,
	MT8195_APLL5_EN = (0x1 << 9) | (0x1 << 20),
	GLITCH_FREE_EN = 0x1 << 12,
	PLL_DIV_EN = 0xff << 24,
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
	ARMPLL_LL_HZ	= 1036 * MHz,
	ARMPLL_BL_HZ	= 1027 * MHz,
	CCIPLL_HZ	= 835 * MHz,
	NNAPLL_HZ	= 860 * MHz,
	RESPLL_HZ	= 600 * MHz,
	ETHPLL_HZ	= 500 * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	TVDPLL1_HZ	= 594 * MHz,
	TVDPLL2_HZ	= 594 * MHz,
	MMPLL_HZ	= 2750UL * MHz,
	MAINPLL_HZ	= 2184UL * MHz,
	VDECPLL_HZ	= 220 * MHz,
	IMGPLL_HZ	= 650 * MHz,
	UNIVPLL_HZ	= 2496UL * MHz,
	HDMIPLL1_HZ	= 884 * MHz,
	HDMIPLL2_HZ	= 600 * MHz,
	HDMIRX_APLL_HZ	= 294915 * KHz,
	USB1PLL_HZ	= 192 * MHz,
	ADSPPLL_HZ	= 720 * MHz,
	APLL1_HZ	= 196608 * KHz,
	APLL2_HZ	= 180633600,
	APLL3_HZ	= 196608 * KHz,
	APLL4_HZ	= 196608 * KHz,
	APLL5_HZ	= 196608 * KHz,
	MFGPLL_HZ	= 700 * MHz,
	DGIPLL_HZ	= 165 * MHz,
};

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
	UNIVPLL_D6_D2_HZ = UNIVPLL_HZ / 6 / 2,
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

#endif /* SOC_MEDIATEK_MT8195_PLL_H */
