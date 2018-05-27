/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2010-2015, NVIDIA CORPORATION.  All rights reserved.
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

#ifndef _TEGRA210_PMC_H_
#define _TEGRA210_PMC_H_

#include <stdint.h>

enum {
	POWER_PARTID_CRAIL = 0,
	POWER_PARTID_TD = 1,
	POWER_PARTID_VE = 2,
	POWER_PARTID_PCX = 3,
	POWER_PARTID_C0L2 = 5,
	POWER_PARTID_MPE = 6,
	POWER_PARTID_HEG = 7,
	POWER_PARTID_SAX = 8,
	POWER_PARTID_CE1 = 9,
	POWER_PARTID_CE2 = 10,
	POWER_PARTID_CE3 = 11,
	POWER_PARTID_CELP = 12,
	POWER_PARTID_CE0 = 14,
	POWER_PARTID_C0NC = 15,
	POWER_PARTID_C1NC = 16,
	POWER_PARTID_SOR = 17,
	POWER_PARTID_DIS = 18,
	POWER_PARTID_DISB = 19,
	POWER_PARTID_XUSBA = 20,
	POWER_PARTID_XUSBB = 21,
	POWER_PARTID_XUSBC = 22,
	POWER_PARTID_VIC = 23,
	POWER_PARTID_IRAM = 24,
	POWER_PARTID_NVDEC = 25,
	POWER_PARTID_NVJPG = 26,
	POWER_PARTID_APE = 27,
	POWER_PARTID_DFD = 28,
	POWER_PARTID_VE2 = 29,
};

struct tegra_pmc_regs {
	u32 cntrl;
	u32 sec_disable;
	u32 pmc_swrst;
	u32 wake_mask;
	u32 wake_lvl;
	u32 wake_status;
	u32 sw_wake_status;
	u32 dpd_pads_oride;
	u32 dpd_sample;
	u32 dpd_enable;
	u32 pwrgate_timer_off;
	u32 clamp_status;
	u32 pwrgate_toggle;
	u32 remove_clamping_cmd;
	u32 pwrgate_status;
	u32 pwrgood_timer;
	u32 blink_timer;
	u32 no_iopower;
	u32 pwr_det;
	u32 pwr_det_latch;
	u32 scratch0;
	u32 scratch1;
	u32 scratch2;
	u32 scratch3;
	u32 scratch4;
	u32 scratch5;
	u32 scratch6;
	u32 scratch7;
	u32 scratch8;
	u32 scratch9;
	u32 scratch10;
	u32 scratch11;
	u32 scratch12;
	u32 scratch13;
	u32 scratch14;
	u32 scratch15;
	u32 scratch16;
	u32 scratch17;
	u32 scratch18;
	u32 scratch19;
	u32 odmdata;
	u32 scratch21;
	u32 scratch22;
	u32 scratch23;
	u32 secure_scratch0;
	u32 secure_scratch1;
	u32 secure_scratch2;
	u32 secure_scratch3;
	u32 secure_scratch4;
	u32 secure_scratch5;
	u32 cpupwrgood_timer;
	u32 cpupwroff_timer;
	u32 pg_mask;
	u32 pg_mask_1;
	u32 auto_wake_lvl;
	u32 auto_wake_lvl_mask;
	u32 wake_delay;
	u32 pwr_det_val;
	u32 ddr_pwr;
	u32 usb_debounce_del;
	u32 usb_a0;
	u32 crypto_op;
	u32 pllp_wb0_override;
	u32 scratch24;
	u32 scratch25;
	u32 scratch26;
	u32 scratch27;
	u32 scratch28;
	u32 scratch29;
	u32 scratch30;
	u32 scratch31;
	u32 scratch32;
	u32 scratch33;
	u32 scratch34;
	u32 scratch35;
	u32 scratch36;
	u32 scratch37;
	u32 scratch38;
	u32 scratch39;
	u32 scratch40;
	u32 scratch41;
	u32 scratch42;
	u32 bondout_mirror[3];
	u32 sys_33v_en;
	u32 bondout_mirror_access;
	u32 gate;
	u32 wake2_mask;
	u32 wake2_lvl;
	u32 wake2_status;
	u32 sw_wake2_status;
	u32 auto_wake2_lvl_mask;
	u32 pg_mask_2;
	u32 pg_mask_ce1;
	u32 pg_mask_ce2;
	u32 pg_mask_ce3;
	u32 pwrgate_timer_ce[7];
	u32 pcx_edpd_cntrl;
	u32 osc_edpd_over;
	u32 clk_out_cntrl;
	u32 sata_pwrgt;
	u32 sensor_ctrl;
	u32 rst_status;
	u32 io_dpd_req;
	u32 io_dpd_status;
	u32 io_dpd2_req;
	u32 io_dpd2_status;
	u32 sel_dpd_tim;
	u32 vddp_sel;
	u32 ddr_cfg;
	u32 e_no_vttgen;
	u8 _rsv0[4];
	u32 pllm_wb0_override_freq;
	u32 test_pwrgate;
	u32 pwrgate_timer_mult;
	u32 dis_sel_dpd;
	u32 utmip_uhsic_triggers;
	u32 utmip_uhsic_saved_state;
	u32 utmip_pad_cfg;
	u32 utmip_term_pad_cfg;
	u32 utmip_uhsic_sleep_cfg;
	u32 utmip_uhsic_sleepwalk_cfg;
	u32 utmip_sleepwalk_p[3];
	u32 uhsic_sleepwalk_p0;
	u32 utmip_uhsic_status;
	u32 utmip_uhsic_fake;
	u32 bondout_mirror3[5 - 3];
	u32 secure_scratch6;
	u32 secure_scratch7;
	u32 scratch43;
	u32 scratch44;
	u32 scratch45;
	u32 scratch46;
	u32 scratch47;
	u32 scratch48;
	u32 scratch49;
	u32 scratch50;
	u32 scratch51;
	u32 scratch52;
	u32 scratch53;
	u32 scratch54;
	u32 scratch55;
	u32 scratch0_eco;
	u32 por_dpd_ctrl;
	u32 scratch2_eco;
	u32 utmip_uhsic_line_wakeup;
	u32 utmip_bias_master_cntrl;
	u32 utmip_master_config;
	u32 td_pwrgate_inter_part_timer;
	u32 utmip_uhsic2_triggers;
	u32 utmip_uhsic2_saved_state;
	u32 utmip_uhsic2_sleep_cfg;
	u32 utmip_uhsic2_sleepwalk_cfg;
	u32 uhsic2_sleepwalk_p1;
	u32 utmip_uhsic2_status;
	u32 utmip_uhsic2_fake;
	u32 utmip_uhsic2_line_wakeup;
	u32 utmip_master2_config;
	u32 utmip_uhsic_rpd_cfg;
	u32 pg_mask_ce0;
	u32 pg_mask3[5 - 3];
	u32 pllm_wb0_override2;
	u32 tsc_mult;
	u32 cpu_vsense_override;
	u32 glb_amap_cfg;
	u32 sticky_bits;
	u32 sec_disable2;
	u32 weak_bias;
	u32 reg_short;
	u32 pg_mask_andor;
	u8 _rsv1[0x2c];
	u32 secure_scratch8;	/* offset 0x300 */
	u32 secure_scratch9;
	u32 secure_scratch10;
	u32 secure_scratch11;
	u32 secure_scratch12;
	u32 secure_scratch13;
	u32 secure_scratch14;
	u32 secure_scratch15;
	u32 secure_scratch16;
	u32 secure_scratch17;
	u32 secure_scratch18;
	u32 secure_scratch19;
	u32 secure_scratch20;
	u32 secure_scratch21;
	u32 secure_scratch22;
	u32 secure_scratch23;
	u32 secure_scratch24;
	u32 secure_scratch25;
	u32 secure_scratch26;
	u32 secure_scratch27;
	u32 secure_scratch28;
	u32 secure_scratch29;
	u32 secure_scratch30;
	u32 secure_scratch31;
	u32 secure_scratch32;
	u32 secure_scratch33;
	u32 secure_scratch34;
	u32 secure_scratch35;
	u32 secure_scratch36;
	u32 secure_scratch37;
	u32 secure_scratch38;
	u32 secure_scratch39;
	u32 secure_scratch40;
	u32 secure_scratch41;
	u32 secure_scratch42;
	u32 secure_scratch43;
	u32 secure_scratch44;
	u32 secure_scratch45;
	u32 secure_scratch46;
	u32 secure_scratch47;
	u32 secure_scratch48;
	u32 secure_scratch49;
	u32 secure_scratch50;
	u32 secure_scratch51;
	u32 secure_scratch52;
	u32 secure_scratch53;
	u32 secure_scratch54;
	u32 secure_scratch55;
	u32 secure_scratch56;
	u32 secure_scratch57;
	u32 secure_scratch58;
	u32 secure_scratch59;
	u32 secure_scratch60;
	u32 secure_scratch61;
	u32 secure_scratch62;
	u32 secure_scratch63;
	u32 secure_scratch64;
	u32 secure_scratch65;
	u32 secure_scratch66;
	u32 secure_scratch67;
	u32 secure_scratch68;
	u32 secure_scratch69;
	u32 secure_scratch70;
	u32 secure_scratch71;
	u32 secure_scratch72;
	u32 secure_scratch73;
	u32 secure_scratch74;
	u32 secure_scratch75;
	u32 secure_scratch76;
	u32 secure_scratch77;
	u32 secure_scratch78;
	u32 secure_scratch79;
	u32 _rsv0x420[8];
	u32 cntrl2;			/* 0x440 */
	u32 _rsv0x444[2];
	u32 event_counter;	/* 0x44C */
	u32 fuse_control;
	u32 scratch1_eco;
	u32 _rsv0x458[1];
	u32 io_dpd3_req;	/* 0x45C */
	u32 io_dpd3_status;
	u32 io_dpd4_req;
	u32 io_dpd4_status;
	u32 _rsv0x46C[30];
	u32 ddr_cntrl;		/* 0x4E4 */
	u32 _rsv0x4E8[70];
	u32 scratch56;		/* 0x600 */
	u32 scratch57;
	u32 scratch58;
	u32 scratch59;
	u32 scratch60;
	u32 scratch61;
	u32 scratch62;
	u32 scratch63;
	u32 scratch64;
	u32 scratch65;
	u32 scratch66;
	u32 scratch67;
	u32 scratch68;
	u32 scratch69;
	u32 scratch70;
	u32 scratch71;
	u32 scratch72;
	u32 scratch73;
	u32 scratch74;
	u32 scratch75;
	u32 scratch76;
	u32 scratch77;
	u32 scratch78;
	u32 scratch79;
	u32 scratch80;
	u32 scratch81;
	u32 scratch82;
	u32 scratch83;
	u32 scratch84;
	u32 scratch85;
	u32 scratch86;
	u32 scratch87;
	u32 scratch88;
	u32 scratch89;
	u32 scratch90;
	u32 scratch91;
	u32 scratch92;
	u32 scratch93;
	u32 scratch94;
	u32 scratch95;
	u32 scratch96;
	u32 scratch97;
	u32 scratch98;
	u32 scratch99;
	u32 scratch100;
	u32 scratch101;
	u32 scratch102;
	u32 scratch103;
	u32 scratch104;
	u32 scratch105;
	u32 scratch106;
	u32 scratch107;
	u32 scratch108;
	u32 scratch109;
	u32 scratch110;
	u32 scratch111;
	u32 scratch112;
	u32 scratch113;
	u32 scratch114;
	u32 scratch115;
	u32 scratch116;
	u32 scratch117;
	u32 scratch118;
	u32 scratch119;
	u32 scratch120;		/* 0x700 */
	u32 scratch121;
	u32 scratch122;
	u32 scratch123;
	u32 scratch124;
	u32 scratch125;
	u32 scratch126;
	u32 scratch127;
	u32 scratch128;
	u32 scratch129;
	u32 scratch130;
	u32 scratch131;
	u32 scratch132;
	u32 scratch133;
	u32 scratch134;
	u32 scratch135;
	u32 scratch136;
	u32 scratch137;
	u32 scratch138;
	u32 scratch139;
	u32 scratch140;
	u32 scratch141;
	u32 scratch142;
	u32 scratch143;
	u32 scratch144;
	u32 scratch145;
	u32 scratch146;
	u32 scratch147;
	u32 scratch148;
	u32 scratch149;
	u32 scratch150;
	u32 scratch151;
	u32 scratch152;
	u32 scratch153;
	u32 scratch154;
	u32 scratch155;
	u32 scratch156;
	u32 scratch157;
	u32 scratch158;
	u32 scratch159;
	u32 scratch160;
	u32 scratch161;
	u32 scratch162;
	u32 scratch163;
	u32 scratch164;
	u32 scratch165;
	u32 scratch166;
	u32 scratch167;
	u32 scratch168;
	u32 scratch169;
	u32 scratch170;
	u32 scratch171;
	u32 scratch172;
	u32 scratch173;
	u32 scratch174;
	u32 scratch175;
	u32 scratch176;
	u32 scratch177;
	u32 scratch178;
	u32 scratch179;
	u32 scratch180;
	u32 scratch181;
	u32 scratch182;
	u32 scratch183;
	u32 scratch184;
	u32 scratch185;
	u32 scratch186;
	u32 scratch187;
	u32 scratch188;
	u32 scratch189;
	u32 scratch190;
	u32 scratch191;
	u32 scratch192;
	u32 scratch193;
	u32 scratch194;
	u32 scratch195;
	u32 scratch196;
	u32 scratch197;
	u32 scratch198;
	u32 scratch199;
	u32 scratch200;
	u32 scratch201;
	u32 scratch202;
	u32 scratch203;
	u32 scratch204;
	u32 scratch205;
	u32 scratch206;
	u32 scratch207;
	u32 scratch208;
	u32 scratch209;
	u32 scratch210;
	u32 scratch211;
	u32 scratch212;
	u32 scratch213;
	u32 scratch214;
	u32 scratch215;
	u32 scratch216;
	u32 scratch217;
	u32 scratch218;
	u32 scratch219;
	u32 scratch220;
	u32 scratch221;
	u32 scratch222;
	u32 scratch223;
	u32 scratch224;
	u32 scratch225;
	u32 scratch226;
	u32 scratch227;
	u32 scratch228;
	u32 scratch229;
	u32 scratch230;
	u32 scratch231;
	u32 scratch232;
	u32 scratch233;
	u32 scratch234;
	u32 scratch235;
	u32 scratch236;
	u32 scratch237;
	u32 scratch238;
	u32 scratch239;
	u32 scratch240;
	u32 scratch241;
	u32 scratch242;
	u32 scratch243;
	u32 scratch244;
	u32 scratch245;
	u32 scratch246;
	u32 scratch247;
	u32 scratch248;
	u32 scratch249;
	u32 scratch250;
	u32 scratch251;
	u32 scratch252;
	u32 scratch253;
	u32 scratch254;
	u32 scratch255;
	u32 scratch256;
	u32 scratch257;
	u32 scratch258;
	u32 scratch259;
	u32 scratch260;
	u32 scratch261;
	u32 scratch262;
	u32 scratch263;
	u32 scratch264;
	u32 scratch265;
	u32 scratch266;
	u32 scratch267;
	u32 scratch268;
	u32 scratch269;
	u32 scratch270;
	u32 scratch271;
	u32 scratch272;
	u32 scratch273;
	u32 scratch274;
	u32 scratch275;
	u32 scratch276;
	u32 scratch277;
	u32 scratch278;
	u32 scratch279;
	u32 scratch280;
	u32 scratch281;
	u32 scratch282;
	u32 scratch283;
	u32 scratch284;
	u32 scratch285;
	u32 scratch286;
	u32 scratch287;
	u32 scratch288;
	u32 scratch289;
	u32 scratch290;
	u32 scratch291;
	u32 scratch292;
	u32 scratch293;
	u32 scratch294;
	u32 scratch295;
	u32 scratch296;
	u32 scratch297;
	u32 scratch298;
	u32 scratch299;			/* 0x9CC */
	u32 _rsv0x9D0[50];
	u32 secure_scratch80;	/* 0xa98 */
	u32 secure_scratch81;
	u32 secure_scratch82;
	u32 secure_scratch83;
	u32 secure_scratch84;
	u32 secure_scratch85;
	u32 secure_scratch86;
	u32 secure_scratch87;
	u32 secure_scratch88;
	u32 secure_scratch89;
	u32 secure_scratch90;
	u32 secure_scratch91;
	u32 secure_scratch92;
	u32 secure_scratch93;
	u32 secure_scratch94;
	u32 secure_scratch95;
	u32 secure_scratch96;
	u32 secure_scratch97;
	u32 secure_scratch98;
	u32 secure_scratch99;
	u32 secure_scratch100;
	u32 secure_scratch101;
	u32 secure_scratch102;
	u32 secure_scratch103;
	u32 secure_scratch104;
	u32 secure_scratch105;
	u32 secure_scratch106;
	u32 secure_scratch107;
	u32 secure_scratch108;
	u32 secure_scratch109;
	u32 secure_scratch110;
	u32 secure_scratch111;
	u32 secure_scratch112;
	u32 secure_scratch113;
	u32 secure_scratch114;
	u32 secure_scratch115;
	u32 secure_scratch116;
	u32 secure_scratch117;
	u32 secure_scratch118;
	u32 secure_scratch119;
};

check_member(tegra_pmc_regs, secure_scratch119, 0xB34);

enum {
	PMC_RST_STATUS_SOURCE_MASK = 0x7,
	PMC_RST_STATUS_SOURCE_POR = 0x0,
	PMC_RST_STATUS_SOURCE_WATCHDOG = 0x1,
	PMC_RST_STATUS_SOURCE_SENSOR = 0x2,
	PMC_RST_STATUS_SOURCE_SW_MAIN = 0x3,
	PMC_RST_STATUS_SOURCE_LP0 = 0x4,
	PMC_RST_STATUS_NUM_SOURCES = 0x5,
};

enum {
	PMC_PWRGATE_TOGGLE_PARTID_MASK = 0x1f,
	PMC_PWRGATE_TOGGLE_PARTID_SHIFT = 0,
	PMC_PWRGATE_TOGGLE_START = 0x1 << 8
};

enum {
	PMC_CNTRL_KBC_CLK_DIS = 0x1 << 0,
	PMC_CNTRL_RTC_CLK_DIS = 0x1 << 1,
	PMC_CNTRL_RTC_RST = 0x1 << 2,
	PMC_CNTRL_KBC_RST = 0x1 << 3,
	PMC_CNTRL_MAIN_RST = 0x1 << 4,
	PMC_CNTRL_LATCHWAKE_EN = 0x1 << 5,
	PMC_CNTRL_GLITCHDET_DIS = 0x1 << 6,
	PMC_CNTRL_BLINK_EN = 0x1 << 7,
	PMC_CNTRL_PWRREQ_POLARITY = 0x1 << 8,
	PMC_CNTRL_PWRREQ_OE = 0x1 << 9,
	PMC_CNTRL_SYSCLK_POLARITY = 0x1 << 10,
	PMC_CNTRL_SYSCLK_OE = 0x1 << 11,
	PMC_CNTRL_PWRGATE_DIS = 0x1 << 12,
	PMC_CNTRL_AOINIT = 0x1 << 13,
	PMC_CNTRL_SIDE_EFFECT_LP0 = 0x1 << 14,
	PMC_CNTRL_CPUPWRREQ_POLARITY = 0x1 << 15,
	PMC_CNTRL_CPUPWRREQ_OE = 0x1 << 16,
	PMC_CNTRL_INTR_POLARITY = 0x1 << 17,
	PMC_CNTRL_FUSE_OVERRIDE = 0x1 << 18,
	PMC_CNTRL_CPUPWRGOOD_EN = 0x1 << 19,
	PMC_CNTRL_CPUPWRGOOD_SEL_SHIFT = 20,
	PMC_CNTRL_CPUPWRGOOD_SEL_MASK =
		0x3 << PMC_CNTRL_CPUPWRGOOD_SEL_SHIFT
};

enum {
	PMC_DDR_PWR_EMMC_MASK = 1 << 1,
	PMC_DDR_PWR_VAL_MASK = 1 << 0,
};

enum {
	PMC_DDR_CFG_PKG_MASK = 1 << 0,
	PMC_DDR_CFG_IF_MASK = 1 << 1,
	PMC_DDR_CFG_XM0_RESET_TRI_MASK = 1 << 12,
	PMC_DDR_CFG_XM0_RESET_DPDIO_MASK = 1 << 13,
};

enum {
	PMC_NO_IOPOWER_MEM_MASK = 1 << 7,
	PMC_NO_IOPOWER_MEM_COMP_MASK = 1 << 16,
};

enum {
	PMC_POR_DPD_CTRL_MEM0_ADDR0_CLK_SEL_DPD_MASK = 1 << 0,
	PMC_POR_DPD_CTRL_MEM0_ADDR1_CLK_SEL_DPD_MASK = 1 << 1,
	PMC_POR_DPD_CTRL_MEM0_HOLD_CKE_LOW_OVR_MASK = 1 << 31,
};

enum {
	PMC_CNTRL2_HOLD_CKE_LOW_EN = 0x1 << 12
};

enum {
	PMC_OSC_EDPD_OVER_XOFS_SHIFT = 1,
	PMC_OSC_EDPD_OVER_XOFS_MASK =
		0x3f << PMC_OSC_EDPD_OVER_XOFS_SHIFT
};

enum {
	PMC_CMD_HOLD_LOW_BR00_11_MASK = 0x0007FF80,
	DPD_OFF = 1 << 30,
	DPD_ON = 2 << 30,
};

enum {
	PMC_GPIO_RAIL_AO_SHIFT = 21,
	PMC_GPIO_RAIL_AO_MASK = (1 << PMC_GPIO_RAIL_AO_SHIFT),
	PMC_GPIO_RAIL_AO_DISABLE = (0 << PMC_GPIO_RAIL_AO_SHIFT),
	PMC_GPIO_RAIL_AO_ENABLE = (1 << PMC_GPIO_RAIL_AO_SHIFT),

	PMC_AUDIO_RAIL_AO_SHIFT = 18,
	PMC_AUDIO_RAIL_AO_MASK = (1 << PMC_AUDIO_RAIL_AO_SHIFT),
	PMC_AUDIO_RAIL_AO_DISABLE = (0 << PMC_AUDIO_RAIL_AO_SHIFT),
	PMC_AUDIO_RAIL_AO_ENABLE = (1 << PMC_AUDIO_RAIL_AO_SHIFT),

	PMC_SDMMC3_RAIL_AO_SHIFT = 13,
	PMC_SDMMC3_RAIL_AO_MASK = (1 << PMC_SDMMC3_RAIL_AO_SHIFT),
	PMC_SDMMC3_RAIL_AO_DISABLE = (0 << PMC_SDMMC3_RAIL_AO_SHIFT),
	PMC_SDMMC3_RAIL_AO_ENABLE = (1 << PMC_SDMMC3_RAIL_AO_SHIFT),
};

#endif	/* _TEGRA210_PMC_H_ */
