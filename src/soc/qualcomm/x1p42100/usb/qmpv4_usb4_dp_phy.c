/* SPDX-License-Identifier: GPL-2.0-only */
#include <console/console.h>
#include <timer.h>
#include <delay.h>
#include <soc/usb/qmp_usb_phy.h>
#include <soc/addressmap.h>

/* Only for QMP V4 PHY - TX registers */
struct usb3_phy_qserdes_tx_reg_layout {
	u8 _reserved1[48];			/* 0x00-0x2F */
	u32 tx_res_code_lane_offset_tx;		/* 0x30 */
	u32 tx_res_code_lane_offset_rx;		/* 0x34 */
	u8 _reserved2[64];			/* 0x38-0x77 */
	u32 tx_lane_mode_1;			/* 0x78 */
	u32 tx_lane_mode_2;			/* 0x7C */
	u32 tx_lane_mode_3;			/* 0x80 */
	u8 _reserved3[184];			/* 0x84-0x13B (rest of registers) */
} __packed;

check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_tx, 0x30);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_rx, 0x34);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1, 0x78);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_2, 0x7C);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_3, 0x80);

/* Only for QMP V4 PHY - RX registers */
struct usb3_phy_qserdes_rx_reg_layout {
	u8 _reserved1[8];			/* 0x00-0x07 */
	u32 rx_ucdr_fo_gain_rate2;		/* 0x08 */
	u8 _reserved2[12];			/* 0x0C-0x17 */
	u32 rx_ucdr_so_gain_rate2;		/* 0x18 */
	u8 _reserved3[4];			/* 0x1C-0x1F */
	u32 rx_ucdr_pi_controls;		/* 0x20 */
	u8 _reserved4[112];			/* 0x24-0x93 */
	u32 rx_ivcm_cal_code_override;		/* 0x94 */
	u8 _reserved5[4];			/* 0x98-0x9B */
	u32 rx_ivcm_cal_ctrl2;			/* 0x9C */
	u32 rx_ivcm_postcal_offset;		/* 0xA0 */
	u8 _reserved6[16];			/* 0xA4-0xB3 */
	u32 rx_dfe_3;				/* 0xB4 */
	u8 _reserved7[40];			/* 0xB8-0xDF */
	u32 rx_vga_cal_cntrl1;			/* 0xE0 */
	u8 _reserved8[4];			/* 0xE4-0xE7 */
	u32 rx_vga_cal_man_val;			/* 0xE8 */
	u8 _reserved9[32];			/* 0xEC-0x10B */
	u32 rx_gm_cal;				/* 0x10C */
	u8 _reserved10[56];			/* 0x110-0x147 */
	u32 rx_sigdet_enables;			/* 0x148 */
	u32 rx_sigdet_cntrl;			/* 0x14C */
	u8 _reserved11[4];			/* 0x150-0x153 */
	u32 rx_sigdet_deglitch_cntrl;		/* 0x154 */
	u8 _reserved12[60];			/* 0x158-0x193 */
	u32 rx_dfe_ctle_post_cal_offset;	/* 0x194 */
	u8 _reserved13[68];			/* 0x198-0x1DB */
	u32 rx_q_pi_intrinsic_bias_rate32;	/* 0x1DC */
	u8 _reserved14[92];			/* 0x1E0-0x23B */
	u32 rx_ucdr_pi_ctrl1;			/* 0x23C */
	u32 rx_ucdr_pi_ctrl2;			/* 0x240 */
	u8 _reserved15[56];			/* 0x244-0x27B */
	u32 rx_ucdr_sb2_gain2_rate2;		/* 0x27C */
	u8 _reserved16[24];			/* 0x280-0x297 */
	u32 rx_dfe_dac_enable1;			/* 0x298 */
	u8 _reserved17[28];			/* 0x29C-0x2B7 */
	u32 rx_mode_rate_0_1_b0;		/* 0x2B8 */
	u32 rx_mode_rate_0_1_b1;		/* 0x2BC */
	u32 rx_mode_rate_0_1_b2;		/* 0x2C0 */
	u32 rx_mode_rate_0_1_b3;		/* 0x2C4 */
	u32 rx_mode_rate_0_1_b4;		/* 0x2C8 */
	u32 rx_mode_rate_0_1_b5;		/* 0x2CC */
	u32 rx_mode_rate_0_1_b6;		/* 0x2D0 */
	u32 rx_mode_rate2_b0;			/* 0x2D4 */
	u32 rx_mode_rate2_b1;			/* 0x2D8 */
	u32 rx_mode_rate2_b2;			/* 0x2DC */
	u32 rx_mode_rate2_b3;			/* 0x2E0 */
	u32 rx_mode_rate2_b4;			/* 0x2E4 */
	u32 rx_mode_rate2_b5;			/* 0x2E8 */
	u32 rx_mode_rate2_b6;			/* 0x2EC */
	u8 _reserved18[28];			/* 0x2F0-0x30B */
	u32 rx_summer_cal_spd_mode;		/* 0x30C */
	u32 rx_bkup_ctrl1;			/* 0x310 */
	u8 _reserved19[812];			/* 0x314-0x63F (remaining space) */
} __packed;

check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fo_gain_rate2, 0x08);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain_rate2, 0x18);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_controls, 0x20);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ivcm_cal_code_override, 0x94);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ivcm_cal_ctrl2, 0x9C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ivcm_postcal_offset, 0xA0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dfe_3, 0xB4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl1, 0xE0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_man_val, 0xE8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_gm_cal, 0x10C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_enables, 0x148);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cntrl, 0x14C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_deglitch_cntrl, 0x154);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dfe_ctle_post_cal_offset, 0x194);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_q_pi_intrinsic_bias_rate32, 0x1DC);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_ctrl1, 0x23C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_ctrl2, 0x240);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_gain2_rate2, 0x27C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dfe_dac_enable1, 0x298);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b0, 0x2B8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b1, 0x2BC);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b2, 0x2C0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b3, 0x2C4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b4, 0x2C8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b5, 0x2CC);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b6, 0x2D0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b0, 0x2D4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b1, 0x2D8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b2, 0x2DC);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b3, 0x2E0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b4, 0x2E4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b5, 0x2E8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b6, 0x2EC);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_summer_cal_spd_mode, 0x30C);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_bkup_ctrl1, 0x310);

/* Only for QMP V4 PHY - PCS registers */
struct usb3_phy_pcs_reg_layout {
	u32 pcs_sw_reset;			/* 0x00 */
	u8 _reserved1[16];			/* 0x04-0x13 */
	u32 pcs_pcs_status1;			/* 0x14 */
	u8 _reserved2[40];			/* 0x18-0x3F */
	u32 pcs_power_down_control;		/* 0x40 */
	u32 pcs_start_control;			/* 0x44 */
	u8 _reserved3[124];			/* 0x48-0xC3 */
	u32 pcs_lock_detect_config1;		/* 0xC4 */
	u32 pcs_lock_detect_config2;		/* 0xC8 */
	u32 pcs_lock_detect_config3;		/* 0xCC */
	u8 _reserved4[8];			/* 0xD0-0xD7 */
	u32 pcs_lock_detect_config6;		/* 0xD8 */
	u32 pcs_refgen_req_config1;		/* 0xDC */
	u8 _reserved5[168];			/* 0xE0-0x187 */
	u32 pcs_rx_sigdet_lvl;			/* 0x188 */
	u8 _reserved6[4];			/* 0x18C-0x18F */
	u32 pcs_rcvr_dtct_dly_p1u2_l;		/* 0x190 */
	u32 pcs_rcvr_dtct_dly_p1u2_h;		/* 0x194 */
	u8 _reserved7[24];			/* 0x198-0x1AF */
	u32 pcs_rx_config;			/* 0x1B0 */
	u8 _reserved8[12];			/* 0x1B4-0x1BF */
	u32 pcs_align_detect_config1;		/* 0x1C0 */
	u32 pcs_align_detect_config2;		/* 0x1C4 */
	u8 _reserved9[8];			/* 0x1C8-0x1CF */
	u32 pcs_pcs_tx_rx_config;		/* 0x1D0 */
	u8 _reserved10[8];			/* 0x1D4-0x1DB */
	u32 pcs_eq_config1;			/* 0x1DC */
	u8 _reserved11[12];			/* 0x1E0-0x1EB */
	u32 pcs_eq_config5;			/* 0x1EC */
	u8 _reserved12[20];			/* 0x1F0-0x203 (remaining space) */
} __packed;

check_member(usb3_phy_pcs_reg_layout, pcs_sw_reset, 0x00);
check_member(usb3_phy_pcs_reg_layout, pcs_pcs_status1, 0x14);
check_member(usb3_phy_pcs_reg_layout, pcs_power_down_control, 0x40);
check_member(usb3_phy_pcs_reg_layout, pcs_start_control, 0x44);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config1, 0xC4);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config2, 0xC8);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config3, 0xCC);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config6, 0xD8);
check_member(usb3_phy_pcs_reg_layout, pcs_refgen_req_config1, 0xDC);
check_member(usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl, 0x188);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l, 0x190);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h, 0x194);
check_member(usb3_phy_pcs_reg_layout, pcs_rx_config, 0x1B0);
check_member(usb3_phy_pcs_reg_layout, pcs_align_detect_config1, 0x1C0);
check_member(usb3_phy_pcs_reg_layout, pcs_align_detect_config2, 0x1C4);
check_member(usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config, 0x1D0);
check_member(usb3_phy_pcs_reg_layout, pcs_eq_config1, 0x1DC);
check_member(usb3_phy_pcs_reg_layout, pcs_eq_config5, 0x1EC);

/* Only for QMP V4 PHY - PCS USB3 registers */
struct usb3_phy_pcs_usb3_reg_layout {
	u8 _reserved1[24];			/* 0x00-0x17 */
	u32 pcs_usb3_lfps_det_high_count_val;	/* 0x18 */
	u8 _reserved2[32];			/* 0x1C-0x3B */
	u32 pcs_usb3_rxeqtraining_dfe_time_s2;	/* 0x3C */
	u32 pcs_usb3_rcvr_dtct_dly_u3_l;	/* 0x40 */
	u32 pcs_usb3_rcvr_dtct_dly_u3_h;	/* 0x44 */
	u8 _reserved3[184];			/* 0x48-0xFF (remaining space) */
} __packed;

check_member(usb3_phy_pcs_usb3_reg_layout, pcs_usb3_lfps_det_high_count_val, 0x18);
check_member(usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rxeqtraining_dfe_time_s2, 0x3C);
check_member(usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rcvr_dtct_dly_u3_l, 0x40);
check_member(usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rcvr_dtct_dly_u3_h, 0x44);

/* USB43DP_COM registers */
struct usb43dp_com_reg_layout {
	u32 phy_mode_ctrl;			/* 0x00 */
	u32 sw_reset;				/* 0x04 */
	u32 power_down_ctrl;			/* 0x08 */
	u32 swi_ctrl;				/* 0x0C */
	u32 typec_ctrl;				/* 0x10 */
	u32 typec_pwrdn_ctrl;			/* 0x14 */
	u32 dp_bist_cfg_0;			/* 0x18 */
	u32 reset_ovrd_ctrl1;			/* 0x1C */
	u32 reset_ovrd_ctrl2;			/* 0x20 */
	u32 dbg_clk_mux_ctrl;			/* 0x24 */
	u32 typec_status;			/* 0x28 */
	u32 placeholder_status;			/* 0x2C */
	u32 revision_id0;			/* 0x30 */
	u32 revision_id1;			/* 0x34 */
	u32 revision_id2;			/* 0x38 */
	u32 revision_id3;			/* 0x3C */
} __packed;

check_member(usb43dp_com_reg_layout, phy_mode_ctrl, 0x00);
check_member(usb43dp_com_reg_layout, sw_reset, 0x04);
check_member(usb43dp_com_reg_layout, power_down_ctrl, 0x08);
check_member(usb43dp_com_reg_layout, swi_ctrl, 0x0C);
check_member(usb43dp_com_reg_layout, typec_ctrl, 0x10);
check_member(usb43dp_com_reg_layout, typec_pwrdn_ctrl, 0x14);
check_member(usb43dp_com_reg_layout, dp_bist_cfg_0, 0x18);
check_member(usb43dp_com_reg_layout, reset_ovrd_ctrl1, 0x1C);
check_member(usb43dp_com_reg_layout, reset_ovrd_ctrl2, 0x20);
check_member(usb43dp_com_reg_layout, dbg_clk_mux_ctrl, 0x24);
check_member(usb43dp_com_reg_layout, typec_status, 0x28);
check_member(usb43dp_com_reg_layout, placeholder_status, 0x2C);
check_member(usb43dp_com_reg_layout, revision_id0, 0x30);
check_member(usb43dp_com_reg_layout, revision_id1, 0x34);
check_member(usb43dp_com_reg_layout, revision_id2, 0x38);
check_member(usb43dp_com_reg_layout, revision_id3, 0x3C);

static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_com_tbl[] = {
	{offsetof(struct usb43dp_com_reg_layout, power_down_ctrl), 0x01},
	{offsetof(struct usb43dp_com_reg_layout, reset_ovrd_ctrl1), 0x15},
};

/* USB3_QSERDES_PLL table */
struct usb3_qserdes_pll_reg_layout {
	u32 ssc_step_size1_mode1;		/* 0x00 */
	u32 ssc_step_size2_mode1;		/* 0x04 */
	u8 _reserved1[8];			/* 0x08-0x0F */
	u32 cp_ctrl_mode1;			/* 0x10 */
	u32 pll_rctrl_mode1;			/* 0x14 */
	u32 pll_cctrl_mode1;			/* 0x18 */
	u32 coreclk_div_mode1;			/* 0x1C */
	u32 lock_cmp1_mode1;			/* 0x20 */
	u32 lock_cmp2_mode1;			/* 0x24 */
	u32 dec_start_mode1;			/* 0x28 */
	u32 dec_start_msb_mode1;		/* 0x2C */
	u32 div_frac_start1_mode1;		/* 0x30 */
	u32 div_frac_start2_mode1;		/* 0x34 */
	u32 div_frac_start3_mode1;		/* 0x38 */
	u32 hsclk_sel_1;			/* 0x3C */
	u32 integloop_gain0_mode1;		/* 0x40 */
	u8 _reserved3[4];			/* 0x44-0x47 */
	u32 vco_tune1_mode1;			/* 0x48 */
	u32 vco_tune2_mode1;			/* 0x4C */
	u8 _reserved4[16];			/* 0x50-0x5F */
	u32 ssc_step_size1_mode0;		/* 0x60 */
	u32 ssc_step_size2_mode0;		/* 0x64 */
	u8 _reserved5[8];			/* 0x68-0x6F */
	u32 cp_ctrl_mode0;			/* 0x70 */
	u32 pll_rctrl_mode0;			/* 0x74 */
	u32 pll_cctrl_mode0;			/* 0x78 */
	u32 coreclk_div_mode0;			/* 0x7C */
	u32 lock_cmp1_mode0;			/* 0x80 */
	u32 lock_cmp2_mode0;			/* 0x84 */
	u32 dec_start_mode0;			/* 0x88 */
	u32 dec_start_msb_mode0;		/* 0x8C */
	u32 div_frac_start1_mode0;		/* 0x90 */
	u32 div_frac_start2_mode0;		/* 0x94 */
	u32 div_frac_start3_mode0;		/* 0x98 */
	u32 hsclk_hs_switch_sel_1;		/* 0x9C */
	u32 integloop_gain0_mode0;		/* 0xA0 */
	u8 _reserved6[4];			/* 0xA4-0xA7 */
	u32 vco_tune1_mode0;			/* 0xA8 */
	u32 vco_tune2_mode0;			/* 0xAC */
	u8 _reserved7[12];			/* 0xB0-0xBB */
	u32 bg_timer;				/* 0xBC */
	u32 ssc_en_center;			/* 0xC0 */
	u8 _reserved8[8];			/* 0xC4-0xCB */
	u32 ssc_per1;				/* 0xCC */
	u32 ssc_per2;				/* 0xD0 */
	u8 _reserved9[20];			/* 0xD4-0xE7 */
	u32 sysclk_buf_enable;			/* 0xE8 */
	u8 _reserved10[4];			/* 0xEC-0xEF */
	u8 _reserved11[4];			/* 0xF0-0xF3 */
	u32 pll_ivco;				/* 0xF4 */
	u32 pll_ivco_mode1;			/* 0xF8 */
	u8 _reserved12[16];			/* 0xFC-0x10B */
	u8 _reserved13[4];			/* 0x10C-0x10F */
	u32 sysclk_en_sel;			/* 0x110 */
	u8 _reserved14[8];			/* 0x114-0x11B */
	u8 _reserved15[4];			/* 0x11C-0x11F */
	u32 lock_cmp_en;			/* 0x120 */
	u32 lock_cmp_cfg;			/* 0x124 */
	u8 _reserved16[24];			/* 0x128-0x13F */
	u32 vco_tune_map;			/* 0x140 */
	u8 _reserved17[4];			/* 0x144-0x147 */
	u32 vco_tune_initval2;			/* 0x148 */
	u8 _reserved18[12];			/* 0x14C-0x157 */
	u32 vco_tune_maxval2;			/* 0x158 */
	u8 _reserved19[20];			/* 0x15C-0x16F */
	u32 core_clk_en;			/* 0x170 */
	u32 cmn_config_1;			/* 0x174 */
	u8 _reserved20[4];			/* 0x178-0x17B */
	u32 svs_mode_clk_sel;			/* 0x17C */
	u8 _reserved21[128];			/* 0x180-0x1FF (remaining space) */
} __packed;

check_member(usb3_qserdes_pll_reg_layout, ssc_step_size1_mode1, 0x00);
check_member(usb3_qserdes_pll_reg_layout, ssc_step_size2_mode1, 0x04);
check_member(usb3_qserdes_pll_reg_layout, cp_ctrl_mode1, 0x10);
check_member(usb3_qserdes_pll_reg_layout, pll_rctrl_mode1, 0x14);
check_member(usb3_qserdes_pll_reg_layout, pll_cctrl_mode1, 0x18);
check_member(usb3_qserdes_pll_reg_layout, coreclk_div_mode1, 0x1C);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp1_mode1, 0x20);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp2_mode1, 0x24);
check_member(usb3_qserdes_pll_reg_layout, dec_start_mode1, 0x28);
check_member(usb3_qserdes_pll_reg_layout, dec_start_msb_mode1, 0x2C);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start1_mode1, 0x30);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start2_mode1, 0x34);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start3_mode1, 0x38);
check_member(usb3_qserdes_pll_reg_layout, hsclk_sel_1, 0x3C);
check_member(usb3_qserdes_pll_reg_layout, integloop_gain0_mode1, 0x40);
check_member(usb3_qserdes_pll_reg_layout, vco_tune1_mode1, 0x48);
check_member(usb3_qserdes_pll_reg_layout, vco_tune2_mode1, 0x4C);
check_member(usb3_qserdes_pll_reg_layout, ssc_step_size1_mode0, 0x60);
check_member(usb3_qserdes_pll_reg_layout, ssc_step_size2_mode0, 0x64);
check_member(usb3_qserdes_pll_reg_layout, cp_ctrl_mode0, 0x70);
check_member(usb3_qserdes_pll_reg_layout, pll_rctrl_mode0, 0x74);
check_member(usb3_qserdes_pll_reg_layout, pll_cctrl_mode0, 0x78);
check_member(usb3_qserdes_pll_reg_layout, coreclk_div_mode0, 0x7C);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp1_mode0, 0x80);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp2_mode0, 0x84);
check_member(usb3_qserdes_pll_reg_layout, dec_start_mode0, 0x88);
check_member(usb3_qserdes_pll_reg_layout, dec_start_msb_mode0, 0x8C);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start1_mode0, 0x90);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start2_mode0, 0x94);
check_member(usb3_qserdes_pll_reg_layout, div_frac_start3_mode0, 0x98);
check_member(usb3_qserdes_pll_reg_layout, hsclk_hs_switch_sel_1, 0x9C);
check_member(usb3_qserdes_pll_reg_layout, integloop_gain0_mode0, 0xA0);
check_member(usb3_qserdes_pll_reg_layout, vco_tune1_mode0, 0xA8);
check_member(usb3_qserdes_pll_reg_layout, vco_tune2_mode0, 0xAC);
check_member(usb3_qserdes_pll_reg_layout, bg_timer, 0xBC);
check_member(usb3_qserdes_pll_reg_layout, ssc_en_center, 0xC0);
check_member(usb3_qserdes_pll_reg_layout, ssc_per1, 0xCC);
check_member(usb3_qserdes_pll_reg_layout, ssc_per2, 0xD0);
check_member(usb3_qserdes_pll_reg_layout, sysclk_buf_enable, 0xE8);
check_member(usb3_qserdes_pll_reg_layout, pll_ivco, 0xF4);
check_member(usb3_qserdes_pll_reg_layout, pll_ivco_mode1, 0xF8);
check_member(usb3_qserdes_pll_reg_layout, sysclk_en_sel, 0x110);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp_en, 0x120);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp_cfg, 0x124);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_map, 0x140);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_initval2, 0x148);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_maxval2, 0x158);
check_member(usb3_qserdes_pll_reg_layout, core_clk_en, 0x170);
check_member(usb3_qserdes_pll_reg_layout, cmn_config_1, 0x174);
check_member(usb3_qserdes_pll_reg_layout, svs_mode_clk_sel, 0x17C);

/* QSERDES PLL table */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_serdes_tbl[] = {
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_en_center), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_per1), 0x62},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_per2), 0x02},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size1_mode0), 0xC2},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size2_mode0), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size1_mode1), 0xC2},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size2_mode1), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, sysclk_buf_enable), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cp_ctrl_mode0), 0x02},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cp_ctrl_mode1), 0x02},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_rctrl_mode0), 0x16},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_rctrl_mode1), 0x16},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_cctrl_mode0), 0x36},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_cctrl_mode1), 0x36},
	{offsetof(struct usb3_qserdes_pll_reg_layout, sysclk_en_sel), 0x1A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp_en), 0x04},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp_cfg), 0x04},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp1_mode0), 0x08},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp2_mode0), 0x1A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp1_mode1), 0x16},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp2_mode1), 0x41},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_mode0), 0x82},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_msb_mode0), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_mode1), 0x82},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_msb_mode1), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start1_mode0), 0x55},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start2_mode0), 0x55},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start3_mode0), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start1_mode1), 0x55},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start2_mode1), 0x55},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start3_mode1), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune_map), 0x14},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune1_mode0), 0xBA},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune2_mode0), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune1_mode1), 0xBA},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune2_mode1), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, hsclk_sel_1), 0x13},
	{offsetof(struct usb3_qserdes_pll_reg_layout, hsclk_hs_switch_sel_1), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, coreclk_div_mode0), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, coreclk_div_mode1), 0x04},
	{offsetof(struct usb3_qserdes_pll_reg_layout, core_clk_en), 0xA0},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cmn_config_1), 0x76},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_ivco), 0x0F},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_ivco_mode1), 0x0F},
	{offsetof(struct usb3_qserdes_pll_reg_layout, integloop_gain0_mode0), 0x20},
	{offsetof(struct usb3_qserdes_pll_reg_layout, integloop_gain0_mode1), 0x20},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune_initval2), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune_maxval2), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, svs_mode_clk_sel), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bg_timer), 0x0A},
};

/* TX table - single set of registers (will be applied to both TXA and TXB via different base addresses) */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_tx_tbl[] = {
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1), 0x05},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_2), 0x50},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_3), 0x40},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_tx), 0x1F},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_rx), 0x0A},
};

/* RX table - single set of registers (will be applied to both RXA and RXB via different base addresses) */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_rx_tbl[] = {
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cntrl), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_deglitch_cntrl), 0x0E},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_enables), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b0), 0xC3},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b1), 0xC3},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b2), 0xD8},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b3), 0x9E},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b4), 0x36},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b5), 0xB6},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate_0_1_b6), 0x64},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b0), 0xD3},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b1), 0xEE},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b2), 0x58},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b3), 0x9A},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b4), 0x24},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b5), 0x76},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_mode_rate2_b6), 0xEF},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ivcm_cal_code_override), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ivcm_cal_ctrl2), 0x80},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_summer_cal_spd_mode), 0x2F},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dfe_ctle_post_cal_offset), 0x08},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_controls), 0x15},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_ctrl1), 0xD0},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_ctrl2), 0x48},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_gain2_rate2), 0x0A},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ivcm_postcal_offset), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl1), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_man_val), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dfe_dac_enable1), 0x88},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dfe_3), 0x45},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_gm_cal), 0x0D},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fo_gain_rate2), 0x09},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain_rate2), 0x05},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_q_pi_intrinsic_bias_rate32), 0x2F},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_bkup_ctrl1), 0x14},
};

/* PCS table */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_pcs_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l), 0xE7},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h), 0x03},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config1), 0xC4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config2), 0x89},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config3), 0x20},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config6), 0x13},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_refgen_req_config1), 0x21},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl), 0x55},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rx_config), 0x0A},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config1), 0xD4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config2), 0x30},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config), 0x0C},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config1), 0x4B},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config5), 0x10},
};

/* PCS USB3 table */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_pcs_usb3_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_lfps_det_high_count_val), 0xF8},
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rxeqtraining_dfe_time_s2), 0x07},
};

const struct ss_usb_phy_reg qmp_phy_ss_instance[] = {
	[0] = { /* SS0 PHY */
		.com_base = (void *)QMP_PHY_SS0_COM_REG_BASE,
		.qserdes_pll_base = (void *)QMP_PHY_SS0_QSERDES_PLL_REG_BASE,
		.tx_base = (void *)QMP_PHY_SS0_TXA_REG_BASE,
		.rx_base = (void *)QMP_PHY_SS0_RXA_REG_BASE,
		.txb_base = (void *)QMP_PHY_SS0_TXB_REG_BASE,
		.rxb_base = (void *)QMP_PHY_SS0_RXB_REG_BASE,
		.pcs_base = (void *)QMP_PHY_SS0_PCS_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_SS0_PCS_USB3_REG_BASE,
		.name = "SS0",
		.serdes_tbl = qmp_v4_usb3_ss0_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_serdes_tbl),
		.tx_tbl = qmp_v4_usb3_ss0_tx_tbl,
		.tx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_tx_tbl),
		.rx_tbl = qmp_v4_usb3_ss0_rx_tbl,
		.rx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_rx_tbl),
		.pcs_tbl = qmp_v4_usb3_ss0_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_ss0_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_usb3_tbl),
	},
	[1] = { /* SS1 PHY */
		.com_base = (void *)QMP_PHY_SS1_COM_REG_BASE,
		.qserdes_pll_base = (void *)QMP_PHY_SS1_QSERDES_PLL_REG_BASE,
		.tx_base = (void *)QMP_PHY_SS1_TXA_REG_BASE,
		.rx_base = (void *)QMP_PHY_SS1_RXA_REG_BASE,
		.txb_base = (void *)QMP_PHY_SS1_TXB_REG_BASE,
		.rxb_base = (void *)QMP_PHY_SS1_RXB_REG_BASE,
		.pcs_base = (void *)QMP_PHY_SS1_PCS_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_SS1_PCS_USB3_REG_BASE,
		.name = "SS1",
		.serdes_tbl = qmp_v4_usb3_ss0_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_serdes_tbl),
		.tx_tbl = qmp_v4_usb3_ss0_tx_tbl,
		.tx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_tx_tbl),
		.rx_tbl = qmp_v4_usb3_ss0_rx_tbl,
		.rx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_rx_tbl),
		.pcs_tbl = qmp_v4_usb3_ss0_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_ss0_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_usb3_tbl),
	},
};

/* Helper function to write register table */
static void qcom_qmp_phy_configure(void *base_addr, const qmp_phy_init_tbl_t tbl[], unsigned int num)
{
	int i;
	const qmp_phy_init_tbl_t *t = tbl;

	if (!t)
		return;

	for (i = 0; i < num; i++, t++)
		write32((void *)((uintptr_t)base_addr + t->offset), t->val);
}

/* SS QMP PHY initialization function - supports both SS0 and SS1 */
enum cb_err qmp_usb4_dp_phy_ss_init(int phy_instance)
{
	if (phy_instance < 0 || phy_instance >= ARRAY_SIZE(qmp_phy_ss_instance)) {
		printk(BIOS_ERR, "Invalid PHY instance %d\n", phy_instance);
		return CB_ERR;
	}

	const struct ss_usb_phy_reg *ss_phy_reg = &qmp_phy_ss_instance[phy_instance];
	struct usb3_phy_pcs_reg_layout *current_pcs_reg = (struct usb3_phy_pcs_reg_layout *)ss_phy_reg->pcs_base;

	printk(BIOS_DEBUG, "QMP-1x16 USB4 DP PHY %s init\n", ss_phy_reg->name);

	/* Configure TYPEC_CTRL register for lane selection */
	struct usb43dp_com_reg_layout *current_com_reg = (struct usb43dp_com_reg_layout *)ss_phy_reg->com_base;

	/* Configure SW_PORTSELECT and SW_PORTSELECT_MUX using coreboot style */
	clrsetbits32(&current_com_reg->typec_ctrl,
				SW_PORTSELECT_MASK | SW_PORTSELECT_MUX_MASK,
				(0 << SW_PORTSELECT_SHIFT) | SW_PORTSELECT_MUX_MASK);

	/* Step 1: COM registers */
	qcom_qmp_phy_configure(ss_phy_reg->com_base,
						qmp_v4_usb3_ss0_com_tbl,
						ARRAY_SIZE(qmp_v4_usb3_ss0_com_tbl));

	/* Step 2: power up USB3 PHY */
	write32(&current_pcs_reg->pcs_power_down_control, 0x01);

	/* Step 3: PLL/Serdes registers */
	qcom_qmp_phy_configure(ss_phy_reg->qserdes_pll_base,
						ss_phy_reg->serdes_tbl,
						ss_phy_reg->serdes_tbl_num);

	/* Step 4: TXA registers */
	qcom_qmp_phy_configure(ss_phy_reg->tx_base,
						ss_phy_reg->tx_tbl,
						ss_phy_reg->tx_tbl_num);

	/* Step 5: RXA registers */
	qcom_qmp_phy_configure(ss_phy_reg->rx_base,
						ss_phy_reg->rx_tbl,
						ss_phy_reg->rx_tbl_num);

	/* Step 6: TXB registers */
	qcom_qmp_phy_configure(ss_phy_reg->txb_base,
						ss_phy_reg->tx_tbl,
						ss_phy_reg->tx_tbl_num);

	/* Step 7: RXB registers */
	qcom_qmp_phy_configure(ss_phy_reg->rxb_base,
						ss_phy_reg->rx_tbl,
						ss_phy_reg->rx_tbl_num);

	/* Step 8: PCS registers */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_base,
						ss_phy_reg->pcs_tbl,
						ss_phy_reg->pcs_tbl_num);

	/* Step 9: PCS USB3 registers */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_usb3_base,
						ss_phy_reg->pcs_usb3_tbl,
						ss_phy_reg->pcs_usb3_tbl_num);

	udelay(100);

	/* perform software reset of USB43DP_COM */
	write32(&current_com_reg->sw_reset, PCS_SW_RESET_DEASSERT);

	write32(&current_pcs_reg->pcs_sw_reset, PCS_SW_RESET_DEASSERT);
	write32(&current_pcs_reg->pcs_start_control, QPHY_PCS_START | QPHY_SERDES_START);

	udelay(100);

	long lock_us = wait_us(10000,
			!(read32(&current_pcs_reg->pcs_pcs_status1) &
			USB3_PCS_PHYSTATUS));

	uint32_t phy_status = read32(&current_pcs_reg->pcs_pcs_status1);

	if (!lock_us) {
		printk(BIOS_ERR, "QMP PHY %s PLL LOCK failed after 10ms, phy_status: 0x%x\n",
				ss_phy_reg->name, phy_status);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "QMP PHY %s initialized and locked in %ldus, phy_status: 0x%x\n",
			ss_phy_reg->name, lock_us, phy_status);

	return CB_SUCCESS;
}
