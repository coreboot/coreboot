/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.2
 */

#ifndef SOC_MEDIATEK_MT8186_PLL_H
#define SOC_MEDIATEK_MT8186_PLL_H

#include <device/mmio.h>
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
	u32 clk_cfg_11;
	u32 clk_cfg_11_set;
	u32 clk_cfg_11_clr;
	u32 reserved12[2];
	u32 clk_cfg_12;
	u32 clk_cfg_12_set;
	u32 clk_cfg_12_clr;
	u32 reserved13[1];
	u32 clk_cfg_13;
	u32 clk_cfg_13_set;
	u32 clk_cfg_13_clr;
	u32 reserved14[1];
	u32 clk_cfg_14;
	u32 clk_cfg_14_set;
	u32 clk_cfg_14_clr;
	u32 reserved15[1];
	u32 clk_cfg_20;
	u32 clk_cfg_20_set;
	u32 clk_cfg_20_clr;
	u32 reserved16[1];
	u32 clk_misc_cfg_0;
	u32 reserved17[3];
	u32 clk_misc_cfg_1;
	u32 reserved18[10];
	u32 clk_dbg_cfg;
	u32 clk_cfg_15;
	u32 clk_cfg_15_set;
	u32 clk_cfg_15_clr;
	u32 reserved19[29];
	u32 clk_scp_cfg_0;
	u32 reserved20[3];
	u32 clk_scp_cfg_1;
	u32 reserved21[3];
	u32 clk26cali_0;
	u32 clk26cali_1;
	u32 reserved22[2];
	u32 cksta_reg;
	u32 cksta_reg1;
	u32 reserved23[50];
	u32 clkmon_clk_sel_reg;
	u32 clkmon_k1_reg;
	u32 reserved24[6];
	u32 clk_auddiv_0;
	u32 clk_auddiv_1;
	u32 clk_auddiv_2;
	u32 aud_top_cfg;
	u32 aud_top_mon;
	u32 clk_auddiv_3;
	u32 reserved25[1];
	u32 usb_top_cfg;
	u32 reserved26[112];
	u32 clk_extck_reg;
};

check_member(mtk_topckgen_regs, clk_mode, 0x0);
check_member(mtk_topckgen_regs, clk_cfg_update, 0x4);
check_member(mtk_topckgen_regs, clk_cfg_update1, 0x8);
check_member(mtk_topckgen_regs, clk_cfg_0, 0x40);
check_member(mtk_topckgen_regs, clk_cfg_0_set, 0x44);
check_member(mtk_topckgen_regs, clk_cfg_0_clr, 0x48);
check_member(mtk_topckgen_regs, clk_cfg_1, 0x50);
check_member(mtk_topckgen_regs, clk_cfg_1_set, 0x54);
check_member(mtk_topckgen_regs, clk_cfg_1_clr, 0x58);
check_member(mtk_topckgen_regs, clk_cfg_2, 0x60);
check_member(mtk_topckgen_regs, clk_cfg_2_set, 0x64);
check_member(mtk_topckgen_regs, clk_cfg_2_clr, 0x68);
check_member(mtk_topckgen_regs, clk_cfg_3, 0x70);
check_member(mtk_topckgen_regs, clk_cfg_3_set, 0x74);
check_member(mtk_topckgen_regs, clk_cfg_3_clr, 0x78);
check_member(mtk_topckgen_regs, clk_cfg_4, 0x80);
check_member(mtk_topckgen_regs, clk_cfg_4_set, 0x84);
check_member(mtk_topckgen_regs, clk_cfg_4_clr, 0x88);
check_member(mtk_topckgen_regs, clk_cfg_5, 0x90);
check_member(mtk_topckgen_regs, clk_cfg_5_set, 0x94);
check_member(mtk_topckgen_regs, clk_cfg_5_clr, 0x98);
check_member(mtk_topckgen_regs, clk_cfg_6, 0xa0);
check_member(mtk_topckgen_regs, clk_cfg_6_set, 0xa4);
check_member(mtk_topckgen_regs, clk_cfg_6_clr, 0xa8);
check_member(mtk_topckgen_regs, clk_cfg_7, 0xb0);
check_member(mtk_topckgen_regs, clk_cfg_7_set, 0xb4);
check_member(mtk_topckgen_regs, clk_cfg_7_clr, 0xb8);
check_member(mtk_topckgen_regs, clk_cfg_8, 0xc0);
check_member(mtk_topckgen_regs, clk_cfg_8_set, 0xc4);
check_member(mtk_topckgen_regs, clk_cfg_8_clr, 0xc8);
check_member(mtk_topckgen_regs, clk_cfg_9, 0xd0);
check_member(mtk_topckgen_regs, clk_cfg_9_set, 0xd4);
check_member(mtk_topckgen_regs, clk_cfg_9_clr, 0xd8);
check_member(mtk_topckgen_regs, clk_cfg_10, 0xe0);
check_member(mtk_topckgen_regs, clk_cfg_10_set, 0xe4);
check_member(mtk_topckgen_regs, clk_cfg_10_clr, 0xe8);
check_member(mtk_topckgen_regs, clk_cfg_11, 0xec);
check_member(mtk_topckgen_regs, clk_cfg_11_set, 0xf0);
check_member(mtk_topckgen_regs, clk_cfg_11_clr, 0xf4);
check_member(mtk_topckgen_regs, clk_cfg_12, 0x100);
check_member(mtk_topckgen_regs, clk_cfg_12_set, 0x104);
check_member(mtk_topckgen_regs, clk_cfg_12_clr, 0x108);
check_member(mtk_topckgen_regs, clk_cfg_13, 0x110);
check_member(mtk_topckgen_regs, clk_cfg_13_set, 0x114);
check_member(mtk_topckgen_regs, clk_cfg_13_clr, 0x118);
check_member(mtk_topckgen_regs, clk_cfg_14, 0x120);
check_member(mtk_topckgen_regs, clk_cfg_14_set, 0x124);
check_member(mtk_topckgen_regs, clk_cfg_14_clr, 0x128);
check_member(mtk_topckgen_regs, clk_cfg_20, 0x130);
check_member(mtk_topckgen_regs, clk_cfg_20_set, 0x134);
check_member(mtk_topckgen_regs, clk_cfg_20_clr, 0x138);
check_member(mtk_topckgen_regs, clk_misc_cfg_0, 0x140);
check_member(mtk_topckgen_regs, clk_misc_cfg_1, 0x150);
check_member(mtk_topckgen_regs, clk_dbg_cfg, 0x17c);
check_member(mtk_topckgen_regs, clk_cfg_15, 0x180);
check_member(mtk_topckgen_regs, clk_cfg_15_set, 0x184);
check_member(mtk_topckgen_regs, clk_cfg_15_clr, 0x188);
check_member(mtk_topckgen_regs, clk_scp_cfg_0, 0x200);
check_member(mtk_topckgen_regs, clk_scp_cfg_1, 0x210);
check_member(mtk_topckgen_regs, clk26cali_0, 0x220);
check_member(mtk_topckgen_regs, clk26cali_1, 0x224);
check_member(mtk_topckgen_regs, cksta_reg, 0x230);
check_member(mtk_topckgen_regs, cksta_reg1, 0x234);
check_member(mtk_topckgen_regs, clkmon_clk_sel_reg, 0x300);
check_member(mtk_topckgen_regs, clkmon_k1_reg, 0x304);
check_member(mtk_topckgen_regs, clk_auddiv_0, 0x320);
check_member(mtk_topckgen_regs, clk_auddiv_1, 0x324);
check_member(mtk_topckgen_regs, clk_auddiv_2, 0x328);
check_member(mtk_topckgen_regs, aud_top_cfg, 0x32c);
check_member(mtk_topckgen_regs, aud_top_mon, 0x330);
check_member(mtk_topckgen_regs, clk_auddiv_3, 0x334);
check_member(mtk_topckgen_regs, usb_top_cfg, 0x33c);
check_member(mtk_topckgen_regs, clk_extck_reg, 0x500);

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
	u32 reserved1[2];
	u32 pllon_con0;
	u32 pllon_con1;
	u32 reserved2[106];
	u32 ap_pllgp1_con0;
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
	u32 apupll_con0;
	u32 apupll_con1;
	u32 apupll_con2;
	u32 apupll_con3;
	u32 mainpll_con0;
	u32 mainpll_con1;
	u32 mainpll_con2;
	u32 mainpll_con3;
	u32 mmpll_con0;
	u32 mmpll_con1;
	u32 mmpll_con2;
	u32 mmpll_con3;
	u32 tvdpll_con0;
	u32 tvdpll_con1;
	u32 tvdpll_con2;
	u32 tvdpll_con3;
	u32 mpll_con0;
	u32 mpll_con1;
	u32 mpll_con2;
	u32 mpll_con3;
	u32 reserved3[31];
	u32 ap_pllgp2_con0;
	u32 adsppll_con0;
	u32 adsppll_con1;
	u32 adsppll_con2;
	u32 adsppll_con3;
	u32 mfgpll_con0;
	u32 mfgpll_con1;
	u32 mfgpll_con2;
	u32 mfgpll_con3;
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
	u32 nnapll_con0;
	u32 nnapll_con1;
	u32 nnapll_con2;
	u32 nnapll_con3;
	u32 nna2pll_con0;
	u32 nna2pll_con1;
	u32 nna2pll_con2;
	u32 nna2pll_con3;
	u32 mdbrppll_con0;
	u32 mdbrppll_con1;
	u32 mdbrppll_con2;
	u32 mdbrppll_con3;
	u32 msdcpll_con0;
	u32 msdcpll_con1;
	u32 msdcpll_con2;
	u32 msdcpll_con3;
	u32 mdbpipll_con0;
	u32 mdbpipll_con1;
	u32 mdbpipll_con2;
	u32 mdbpipll_con3;
	u32 reserved4[21];
	u32 ap_auxadc_con0;
	u32 ap_auxadc_con1;
	u32 reserved5[30];
	u32 ap_tsense_con0;
	u32 ap_tsense_con1;
	u32 ap_tsense_con2;
	u32 reserved6[29];
	u32 ulposc_con0;
	u32 ulposc_con1;
	u32 reserved7[30];
	u32 ulposc2_con0;
	u32 ulposc2_con1;
	u32 reserved8[158];
	u32 ap_abist_mon_con0;
	u32 ap_abist_mon_con1;
	u32 ap_abist_mon_con2;
	u32 ap_abist_mon_con3;
	u32 occscan_con0;
	u32 clkdiv_con0;
	u32 occscan_con1;
	u32 occscan_con2;
	u32 occscan_con3;
	u32 mcu_occscan_con0;
	u32 occscan_con4;
	u32 occscan_con5;
	u32 reserved9[52];
	u32 rsv_rw0_con0;
	u32 rsv_rw1_con0;
	u32 rsv_ro_con0;
};
check_member(mtk_apmixed_regs, ap_pll_con0, 0x0);
check_member(mtk_apmixed_regs, ap_pll_con1, 0x4);
check_member(mtk_apmixed_regs, ap_pll_con2, 0x8);
check_member(mtk_apmixed_regs, ap_pll_con3, 0xc);
check_member(mtk_apmixed_regs, ap_pll_con4, 0x10);
check_member(mtk_apmixed_regs, ap_pll_con5, 0x14);
check_member(mtk_apmixed_regs, clksq_stb_con0, 0x18);
check_member(mtk_apmixed_regs, pll_pwr_con0, 0x1c);
check_member(mtk_apmixed_regs, pll_pwr_con1, 0x20);
check_member(mtk_apmixed_regs, pll_iso_con0, 0x24);
check_member(mtk_apmixed_regs, pll_iso_con1, 0x28);
check_member(mtk_apmixed_regs, pll_stb_con0, 0x2c);
check_member(mtk_apmixed_regs, div_stb_con0, 0x30);
check_member(mtk_apmixed_regs, pll_chg_con0, 0x34);
check_member(mtk_apmixed_regs, pll_test_con0, 0x38);
check_member(mtk_apmixed_regs, pll_test_con1, 0x3c);
check_member(mtk_apmixed_regs, apll1_tuner_con0, 0x40);
check_member(mtk_apmixed_regs, apll2_tuner_con0, 0x44);
check_member(mtk_apmixed_regs, pllon_con0, 0x50);
check_member(mtk_apmixed_regs, pllon_con1, 0x54);
check_member(mtk_apmixed_regs, ap_pllgp1_con0, 0x200);
check_member(mtk_apmixed_regs, armpll_ll_con0, 0x204);
check_member(mtk_apmixed_regs, armpll_ll_con1, 0x208);
check_member(mtk_apmixed_regs, armpll_ll_con2, 0x20c);
check_member(mtk_apmixed_regs, armpll_ll_con3, 0x210);
check_member(mtk_apmixed_regs, armpll_bl_con0, 0x214);
check_member(mtk_apmixed_regs, armpll_bl_con1, 0x218);
check_member(mtk_apmixed_regs, armpll_bl_con2, 0x21c);
check_member(mtk_apmixed_regs, armpll_bl_con3, 0x220);
check_member(mtk_apmixed_regs, ccipll_con0, 0x224);
check_member(mtk_apmixed_regs, ccipll_con1, 0x228);
check_member(mtk_apmixed_regs, ccipll_con2, 0x22c);
check_member(mtk_apmixed_regs, ccipll_con3, 0x230);
check_member(mtk_apmixed_regs, apupll_con0, 0x234);
check_member(mtk_apmixed_regs, apupll_con1, 0x238);
check_member(mtk_apmixed_regs, apupll_con2, 0x23c);
check_member(mtk_apmixed_regs, apupll_con3, 0x240);
check_member(mtk_apmixed_regs, mainpll_con0, 0x244);
check_member(mtk_apmixed_regs, mainpll_con1, 0x248);
check_member(mtk_apmixed_regs, mainpll_con2, 0x24c);
check_member(mtk_apmixed_regs, mainpll_con3, 0x250);
check_member(mtk_apmixed_regs, mmpll_con0, 0x254);
check_member(mtk_apmixed_regs, mmpll_con1, 0x258);
check_member(mtk_apmixed_regs, mmpll_con2, 0x25c);
check_member(mtk_apmixed_regs, mmpll_con3, 0x260);
check_member(mtk_apmixed_regs, tvdpll_con0, 0x264);
check_member(mtk_apmixed_regs, tvdpll_con1, 0x268);
check_member(mtk_apmixed_regs, tvdpll_con2, 0x26c);
check_member(mtk_apmixed_regs, tvdpll_con3, 0x270);
check_member(mtk_apmixed_regs, mpll_con0, 0x274);
check_member(mtk_apmixed_regs, mpll_con1, 0x278);
check_member(mtk_apmixed_regs, mpll_con2, 0x27c);
check_member(mtk_apmixed_regs, mpll_con3, 0x280);
check_member(mtk_apmixed_regs, ap_pllgp2_con0, 0x300);
check_member(mtk_apmixed_regs, adsppll_con0, 0x304);
check_member(mtk_apmixed_regs, adsppll_con1, 0x308);
check_member(mtk_apmixed_regs, adsppll_con2, 0x30c);
check_member(mtk_apmixed_regs, adsppll_con3, 0x310);
check_member(mtk_apmixed_regs, mfgpll_con0, 0x314);
check_member(mtk_apmixed_regs, mfgpll_con1, 0x318);
check_member(mtk_apmixed_regs, mfgpll_con2, 0x31c);
check_member(mtk_apmixed_regs, mfgpll_con3, 0x320);
check_member(mtk_apmixed_regs, univpll_con0, 0x324);
check_member(mtk_apmixed_regs, univpll_con1, 0x328);
check_member(mtk_apmixed_regs, univpll_con2, 0x32c);
check_member(mtk_apmixed_regs, univpll_con3, 0x330);
check_member(mtk_apmixed_regs, apll1_con0, 0x334);
check_member(mtk_apmixed_regs, apll1_con1, 0x338);
check_member(mtk_apmixed_regs, apll1_con2, 0x33c);
check_member(mtk_apmixed_regs, apll1_con3, 0x340);
check_member(mtk_apmixed_regs, apll1_con4, 0x344);
check_member(mtk_apmixed_regs, apll2_con0, 0x348);
check_member(mtk_apmixed_regs, apll2_con1, 0x34c);
check_member(mtk_apmixed_regs, apll2_con2, 0x350);
check_member(mtk_apmixed_regs, apll2_con3, 0x354);
check_member(mtk_apmixed_regs, apll2_con4, 0x358);
check_member(mtk_apmixed_regs, nnapll_con0, 0x35c);
check_member(mtk_apmixed_regs, nnapll_con1, 0x360);
check_member(mtk_apmixed_regs, nnapll_con2, 0x364);
check_member(mtk_apmixed_regs, nnapll_con3, 0x368);
check_member(mtk_apmixed_regs, nna2pll_con0, 0x36c);
check_member(mtk_apmixed_regs, nna2pll_con1, 0x370);
check_member(mtk_apmixed_regs, nna2pll_con2, 0x374);
check_member(mtk_apmixed_regs, nna2pll_con3, 0x378);
check_member(mtk_apmixed_regs, mdbrppll_con0, 0x37c);
check_member(mtk_apmixed_regs, mdbrppll_con1, 0x380);
check_member(mtk_apmixed_regs, mdbrppll_con2, 0x384);
check_member(mtk_apmixed_regs, mdbrppll_con3, 0x388);
check_member(mtk_apmixed_regs, msdcpll_con0, 0x38c);
check_member(mtk_apmixed_regs, msdcpll_con1, 0x390);
check_member(mtk_apmixed_regs, msdcpll_con2, 0x394);
check_member(mtk_apmixed_regs, msdcpll_con3, 0x398);
check_member(mtk_apmixed_regs, mdbpipll_con0, 0x39c);
check_member(mtk_apmixed_regs, mdbpipll_con1, 0x3a0);
check_member(mtk_apmixed_regs, mdbpipll_con2, 0x3a4);
check_member(mtk_apmixed_regs, mdbpipll_con3, 0x3a8);
check_member(mtk_apmixed_regs, ap_auxadc_con0, 0x400);
check_member(mtk_apmixed_regs, ap_auxadc_con1, 0x404);
check_member(mtk_apmixed_regs, ap_tsense_con0, 0x480);
check_member(mtk_apmixed_regs, ap_tsense_con1, 0x484);
check_member(mtk_apmixed_regs, ap_tsense_con2, 0x488);
check_member(mtk_apmixed_regs, ulposc_con0, 0x500);
check_member(mtk_apmixed_regs, ulposc_con1, 0x504);
check_member(mtk_apmixed_regs, ulposc2_con0, 0x580);
check_member(mtk_apmixed_regs, ulposc2_con1, 0x584);
check_member(mtk_apmixed_regs, ap_abist_mon_con0, 0x800);
check_member(mtk_apmixed_regs, ap_abist_mon_con1, 0x804);
check_member(mtk_apmixed_regs, ap_abist_mon_con2, 0x808);
check_member(mtk_apmixed_regs, ap_abist_mon_con3, 0x80c);
check_member(mtk_apmixed_regs, occscan_con0, 0x810);
check_member(mtk_apmixed_regs, clkdiv_con0, 0x814);
check_member(mtk_apmixed_regs, occscan_con1, 0x818);
check_member(mtk_apmixed_regs, occscan_con2, 0x81c);
check_member(mtk_apmixed_regs, occscan_con3, 0x820);
check_member(mtk_apmixed_regs, mcu_occscan_con0, 0x824);
check_member(mtk_apmixed_regs, occscan_con4, 0x828);
check_member(mtk_apmixed_regs, occscan_con5, 0x82c);
check_member(mtk_apmixed_regs, rsv_rw0_con0, 0x900);
check_member(mtk_apmixed_regs, rsv_rw1_con0, 0x904);
check_member(mtk_apmixed_regs, rsv_ro_con0, 0x908);

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
	MT8186_PLL_EN = 0x1 << 0,
	GLITCH_FREE_EN = 0x1 << 4,
	PLL_DIV_EN = 0xff << 24,
};

enum {
	ARMPLL_DIVIDER_PLL1_EN = 0x1 << 4,
	ARMPLL_DIVIDER_PLL2_EN = 0x1 << 5,
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
	ARMPLL_LL_HZ	= 1280 * MHz,
	ARMPLL_BL_HZ	= 1085 * MHz,
	CCIPLL_HZ	= 800 * MHz,
	MAINPLL_HZ	= 1092 * MHz,
	UNIV2PLL_HZ	= 2496UL * MHz,
	MSDCPLL_HZ	= 384 * MHz,
	MMPLL_HZ	= 560 * MHz,
	NNAPLL_HZ	= 800 * MHz,
	NNA2PLL_HZ	= 800 * MHz,
	ADSPPLL_HZ	= 800 * MHz,
	MFGPLL_HZ	= 250 * MHz,
	TVDPLL_HZ	= 297 * MHz,
	APLL1_HZ	= 180633600,
	APLL2_HZ	= 196608 * KHz,
};

/* top_div rate */
enum {
	CLK26M_HZ	= 26 * MHz,
	MAINPLL_D5_HZ = MAINPLL_HZ / 5,
};

/* top_mux rate */
enum {
	SPI_HZ		= MAINPLL_D5_HZ,
	UART_HZ		= CLK26M_HZ,
};

DEFINE_BITFIELD(CLK_DBG_CFG_ABIST_CK_SEL, 21, 16)
DEFINE_BITFIELD(CLK_DBG_CFG_CKGEN_CK_SEL, 13, 8)
DEFINE_BITFIELD(CLK_DBG_CFG_METER_CK_SEL, 1, 0)
DEFINE_BITFIELD(CLK_MISC_CFG_0_METER_DIV, 31, 24)
DEFINE_BITFIELD(CLK26CALI_0_ENABLE, 12, 12)
DEFINE_BITFIELD(CLK26CALI_0_TRIGGER, 4, 4)

DEFINE_BIT(INFRACFG_AO_AUDIO_BUS_REG0, 29)
DEFINE_BIT(INFRACFG_AO_ICUSB_BUS_REG0, 28)

DEFINE_BITFIELD(INFRACFG_AO_INFRA_BUS_REG0_0, 14, 0)
DEFINE_BITFIELD(INFRACFG_AO_INFRA_BUS_REG0_1, 23, 20)
DEFINE_BIT(INFRACFG_AO_INFRA_BUS_REG0_2, 30)

DEFINE_BIT(INFRACFG_AO_P2P_RX_CLK_REG0_MASK_0, 0)
DEFINE_BIT(INFRACFG_AO_P2P_RX_CLK_REG0_MASK_1, 5)

DEFINE_BITFIELD(INFRACFG_AO_PERI_BUS_REG0_0, 1, 0)
DEFINE_BITFIELD(INFRACFG_AO_PERI_BUS_REG0_1, 27, 3)
DEFINE_BIT(INFRACFG_AO_PERI_BUS_REG0_2, 31)

DEFINE_BITFIELD(USB_TOP_CFG_MACRO_CTRL, 1, 0)

#endif /* SOC_MEDIATEK_MT8186_PLL_H */
