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

/* USB3_PCS_MISC registers */
struct usb3_phy_pcs_misc_reg_layout {
	u8 _reserved1[8];			/* 0x00-0x07 */
	u32 pcs_misc_config1;			/* 0x08 */
	u8 _reserved2[244];			/* 0x0C-0xFF (remaining space) */
} __packed;

check_member(usb3_phy_pcs_misc_reg_layout, pcs_misc_config1, 0x08);

/* Only for QMP V4 PHY - PCS registers */
struct usb3_phy_pcs_reg_layout {
	u32 pcs_sw_reset;			/* 0x00 */
	u8 _reserved1[16];			/* 0x04-0x13 */
	u32 pcs_pcs_status1;			/* 0x14 */
	u8 _reserved2[40];			/* 0x18-0x3F */
	u32 pcs_power_down_control;		/* 0x40 */
	u32 pcs_start_control;			/* 0x44 */
	u8 _reserved3[76];			/* 0x48-0x93 */
	u32 pcs_power_state_config2;		/* 0x94 */
	u8 _reserved3b[44];			/* 0x98-0xC3 */
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
	u8 _reserved7[20];			/* 0x198-0x1AB */
	u32 pcs_tsync_rsync_time;		/* 0x1AC */
	u32 pcs_rx_config;			/* 0x1B0 */
	u32 pcs_tsync_dly_time;			/* 0x1B4 */
	u8 _reserved8[8];			/* 0x1B8-0x1BF */
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
check_member(usb3_phy_pcs_reg_layout, pcs_power_state_config2, 0x94);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config1, 0xC4);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config2, 0xC8);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config3, 0xCC);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config6, 0xD8);
check_member(usb3_phy_pcs_reg_layout, pcs_refgen_req_config1, 0xDC);
check_member(usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl, 0x188);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l, 0x190);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h, 0x194);
check_member(usb3_phy_pcs_reg_layout, pcs_tsync_rsync_time, 0x1AC);
check_member(usb3_phy_pcs_reg_layout, pcs_rx_config, 0x1B0);
check_member(usb3_phy_pcs_reg_layout, pcs_tsync_dly_time, 0x1B4);
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

/* USB43DP_AON registers */
struct usb43dp_aon_reg_layout {
	u32 usb3_aon_clamp_enable;
	u32 usb4_aon_clamp_enable;
	u32 usb3_aon_toggle_enable;
	u32 usb4_aon_toggle_enable;
	u32 dp_aon_toggle_enable;
	u32 dummy_status;
} __packed;

check_member(usb43dp_aon_reg_layout, usb3_aon_clamp_enable, 0x00);
check_member(usb43dp_aon_reg_layout, usb4_aon_clamp_enable, 0x04);
check_member(usb43dp_aon_reg_layout, usb3_aon_toggle_enable, 0x08);
check_member(usb43dp_aon_reg_layout, usb4_aon_toggle_enable, 0x0C);
check_member(usb43dp_aon_reg_layout, dp_aon_toggle_enable, 0x10);
check_member(usb43dp_aon_reg_layout, dummy_status, 0x14);

/* AON block is at fixed offset 0x100 from USB43DP_COM base */
#define USB43DP_AON_OFFSET_FROM_COM	0x100
/* Disable low-frequency toggle of the USB3 AON output clocks */
#define USB3_AON_TOGGLE_DISABLE	0x00

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
	u32 bin_vcocal_cmp_code1_mode1;		/* 0x50 */
	u32 bin_vcocal_cmp_code2_mode1;		/* 0x54 */
	u32 bin_vcocal_cmp_code1_mode0;		/* 0x58 */
	u32 bin_vcocal_cmp_code2_mode0;		/* 0x5C */
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
	u32 cmn_ietrim;				/* 0xFC */
	u8 _reserved12[12];			/* 0x100-0x10B */
	u8 _reserved13[4];			/* 0x10C-0x10F */
	u32 sysclk_en_sel;			/* 0x110 */
	u8 _reserved14[8];			/* 0x114-0x11B */
	u8 _reserved15[4];			/* 0x11C-0x11F */
	u32 lock_cmp_en;			/* 0x120 */
	u32 lock_cmp_cfg;			/* 0x124 */
	u8 _reserved16[20];			/* 0x128-0x13B */
	u32 vco_tune_ctrl;			/* 0x13C */
	u32 vco_tune_map;			/* 0x140 */
	u8 _reserved17[4];			/* 0x144-0x147 */
	u32 vco_tune_initval2;			/* 0x148 */
	u8 _reserved18[12];			/* 0x14C-0x157 */
	u32 vco_tune_maxval2;			/* 0x158 */
	u8 _reserved19[20];			/* 0x15C-0x16F */
	u32 core_clk_en;			/* 0x170 */
	u32 cmn_config_1;			/* 0x174 */
	u8 _reserved20[8];			/* 0x178-0x17F - verified: SVS_MODE_CLK_SEL is at 0x180 */
	u32 svs_mode_clk_sel;			/* 0x180 */
	u8 _reserved21[28];			/* 0x184-0x19F */
	u32 bin_vcocal_hsclk_sel_1;		/* 0x1A0 */
	u8 _reserved22[276];			/* 0x1A4-0x2B7 */
	u32 pll_spare_for_eco;			/* 0x2B8 */
	u8 _reserved23[28];			/* 0x2BC-0x2D7 */
	u32 dcc_cal_1;				/* 0x2D8 */
	u32 dcc_cal_2;				/* 0x2DC */
	u32 dcc_cal_3;				/* 0x2E0 */
	u8 _reserved24[12];			/* 0x2E4-0x2EF */
	u32 psm_cal_en;				/* 0x2F0 */
	u32 clk_fwd_config_1;			/* 0x2F4 */
	u8 _reserved25[4];			/* 0x2F8-0x2FB */
	u32 ip_ctrl_and_dp_sel;			/* 0x2FC */
	u8 _reserved26[256];			/* 0x300-0x3FF (remaining space) */
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
check_member(usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code1_mode1, 0x50);
check_member(usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code2_mode1, 0x54);
check_member(usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code1_mode0, 0x58);
check_member(usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code2_mode0, 0x5C);
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
check_member(usb3_qserdes_pll_reg_layout, cmn_ietrim, 0xFC);
check_member(usb3_qserdes_pll_reg_layout, sysclk_en_sel, 0x110);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp_en, 0x120);
check_member(usb3_qserdes_pll_reg_layout, lock_cmp_cfg, 0x124);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_ctrl, 0x13C);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_map, 0x140);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_initval2, 0x148);
check_member(usb3_qserdes_pll_reg_layout, vco_tune_maxval2, 0x158);
check_member(usb3_qserdes_pll_reg_layout, core_clk_en, 0x170);
check_member(usb3_qserdes_pll_reg_layout, cmn_config_1, 0x174);
check_member(usb3_qserdes_pll_reg_layout, svs_mode_clk_sel, 0x180);
check_member(usb3_qserdes_pll_reg_layout, bin_vcocal_hsclk_sel_1, 0x1A0);
check_member(usb3_qserdes_pll_reg_layout, pll_spare_for_eco, 0x2B8);
check_member(usb3_qserdes_pll_reg_layout, dcc_cal_1, 0x2D8);
check_member(usb3_qserdes_pll_reg_layout, dcc_cal_2, 0x2DC);
check_member(usb3_qserdes_pll_reg_layout, dcc_cal_3, 0x2E0);
check_member(usb3_qserdes_pll_reg_layout, psm_cal_en, 0x2F0);
check_member(usb3_qserdes_pll_reg_layout, clk_fwd_config_1, 0x2F4);
check_member(usb3_qserdes_pll_reg_layout, ip_ctrl_and_dp_sel, 0x2FC);

/* QSERDES PLL table - values per PHY HSR */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_serdes_tbl[] = {
	{offsetof(struct usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code1_mode1), 0x95},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code2_mode1), 0x1E},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code1_mode0), 0x4B},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bin_vcocal_cmp_code2_mode0), 0x0F},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_en_center), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_per1), 0x62},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_per2), 0x02},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size1_mode0), 0xE1},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size2_mode0), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size1_mode1), 0xE1},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ssc_step_size2_mode1), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, sysclk_buf_enable), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cp_ctrl_mode0), 0x06},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cp_ctrl_mode1), 0x06},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_rctrl_mode0), 0x16},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_rctrl_mode1), 0x16},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_cctrl_mode0), 0x36},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_cctrl_mode1), 0x36},
	{offsetof(struct usb3_qserdes_pll_reg_layout, sysclk_en_sel), 0x1A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp_en), 0x04},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp_cfg), 0x04},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune_ctrl), 0x40},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp1_mode0), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp2_mode0), 0x1A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp1_mode1), 0x1A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, lock_cmp2_mode1), 0x41},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_mode0), 0x41},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_msb_mode0), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_mode1), 0x41},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dec_start_msb_mode1), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start1_mode0), 0xAB},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start2_mode0), 0xAA},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start3_mode0), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start1_mode1), 0xAB},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start2_mode1), 0xAA},
	{offsetof(struct usb3_qserdes_pll_reg_layout, div_frac_start3_mode1), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune_map), 0x14},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune1_mode0), 0x4D},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune2_mode0), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune1_mode1), 0x4D},
	{offsetof(struct usb3_qserdes_pll_reg_layout, vco_tune2_mode1), 0x03},
	{offsetof(struct usb3_qserdes_pll_reg_layout, hsclk_sel_1), 0x13},
	{offsetof(struct usb3_qserdes_pll_reg_layout, hsclk_hs_switch_sel_1), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, coreclk_div_mode0), 0x05},
	{offsetof(struct usb3_qserdes_pll_reg_layout, coreclk_div_mode1), 0x02},
	{offsetof(struct usb3_qserdes_pll_reg_layout, core_clk_en), 0xA0},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cmn_config_1), 0x76},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_ivco), 0x0F},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_ivco_mode1), 0x0F},
	{offsetof(struct usb3_qserdes_pll_reg_layout, integloop_gain0_mode0), 0xFF},
	{offsetof(struct usb3_qserdes_pll_reg_layout, integloop_gain0_mode1), 0xFF},
	{offsetof(struct usb3_qserdes_pll_reg_layout, svs_mode_clk_sel), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bg_timer), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, cmn_ietrim), 0x0A},
	{offsetof(struct usb3_qserdes_pll_reg_layout, bin_vcocal_hsclk_sel_1), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, pll_spare_for_eco), 0x00},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dcc_cal_1), 0x40},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dcc_cal_2), 0x01},
	{offsetof(struct usb3_qserdes_pll_reg_layout, dcc_cal_3), 0x60},
	{offsetof(struct usb3_qserdes_pll_reg_layout, psm_cal_en), 0x25},
	{offsetof(struct usb3_qserdes_pll_reg_layout, clk_fwd_config_1), 0x33},
	{offsetof(struct usb3_qserdes_pll_reg_layout, ip_ctrl_and_dp_sel), 0xAF},
};

/* USB43DP QSERDES Lane register layout (LA and LB share the same layout) */
struct usb43dp_qserdes_lane_reg_layout {
	u32 bist_mode_laneno;				/* 0x000 */
	u8 _reserved1[0x58];				/* 0x004-0x05B */
	u32 clkbuf_enable;				/* 0x05C */
	u8 _reserved2[0x24];				/* 0x060-0x083 */
	u32 tx_lvl_update_ctrl;				/* 0x084 */
	u8 _reserved3[0x18];				/* 0x088-0x09F */
	u32 pcie5_top_ldo_code_ctrl1;			/* 0x0A0 */
	u32 pcie5_top_ldo_code_ctrl2;			/* 0x0A4 */
	u32 pcie5_top_ldo_code_ctrl3;			/* 0x0A8 */
	u32 pcie5_top_ldo_code_ctrl4;			/* 0x0AC */
	u32 transmitter_en_ctrl;			/* 0x0B0 */
	u8 _reserved4[0x18];				/* 0x0B4-0x0CB */
	u32 lane_mode_1;				/* 0x0CC */
	u32 lane_mode_2;				/* 0x0D0 */
	u32 lane_mode_3;				/* 0x0D4 */
	u32 lane_mode_4;				/* 0x0D8 */
	u8 _reserved5[0x10];				/* 0x0DC-0x0EB */
	u32 tx0_restrim_cal_ctrl;			/* 0x0EC */
	u8 _reserved6[0x4];				/* 0x0F0-0x0F3 */
	u32 tx0_restrim_post_cal_offset;		/* 0x0F4 */
	u8 _reserved7[0x8];				/* 0x0F8-0x0FF */
	u32 tx1_restrim_cal_ctrl;			/* 0x100 */
	u8 _reserved8[0x8];				/* 0x104-0x10B */
	u32 tx0_restrim_vref_sel;			/* 0x10C */
	u32 tx1_restrim_vref_sel;			/* 0x110 */
	u8 _reserved9[0x14];				/* 0x114-0x127 */
	u32 ana_interface_select2;			/* 0x128 */
	u8 _reserved10[0x4];				/* 0x12C-0x12F */
	u32 pcs_interface_select1;			/* 0x130 */
	u8 _reserved11[0x20];				/* 0x134-0x153 */
	u32 rx_mode_rate_0_1_b0;			/* 0x154 */
	u32 rx_mode_rate_0_1_b1;			/* 0x158 */
	u32 rx_mode_rate_0_1_b2;			/* 0x15C */
	u32 rx_mode_rate_0_1_b3;			/* 0x160 */
	u32 rx_mode_rate_0_1_b4;			/* 0x164 */
	u32 rx_mode_rate_0_1_b5;			/* 0x168 */
	u32 rx_mode_rate_0_1_b6;			/* 0x16C */
	u32 rx_mode_rate_0_1_b7;			/* 0x170 */
	u32 rx_mode_rate2_b0;				/* 0x174 */
	u32 rx_mode_rate2_b1;				/* 0x178 */
	u32 rx_mode_rate2_b2;				/* 0x17C */
	u32 rx_mode_rate2_b3;				/* 0x180 */
	u32 rx_mode_rate2_b4;				/* 0x184 */
	u32 rx_mode_rate2_b5;				/* 0x188 */
	u32 rx_mode_rate2_b6;				/* 0x18C */
	u32 rx_mode_rate2_b7;				/* 0x190 */
	u8 _reserved12[0x44];				/* 0x194-0x1D7 */
	u32 tx_dcc_ana_ctrl2;				/* 0x1D8 */
	u8 _reserved13[0x6C];				/* 0x1DC-0x247 */
	u32 cdr_vco_ctune_meas_cnt1_rate1;		/* 0x248 */
	u32 cdr_vco_ctune_meas_cnt2_rate1;		/* 0x24C */
	u32 cdr_vco_ctune_meas_cnt1_rate2;		/* 0x250 */
	u32 cdr_vco_ctune_meas_cnt2_rate2;		/* 0x254 */
	u8 _reserved14[0x14];				/* 0x258-0x26B */
	u32 cdr_vctrl_rate_2_3;				/* 0x26C */
	u8 _reserved15[0x4];				/* 0x270-0x273 */
	u32 kvco_init_rate_0_1;				/* 0x274 */
	u32 kvco_init_rate_2_3;				/* 0x278 */
	u8 _reserved16[0x8];				/* 0x27C-0x283 */
	u32 kvco_code_ovrd_rate1;			/* 0x284 */
	u32 kvco_code_ovrd_rate2;			/* 0x288 */
	u8 _reserved17[0x28];				/* 0x28C-0x2B3 */
	u32 kvco_ideal_freq_diff1_rate1;		/* 0x2B4 */
	u32 kvco_ideal_freq_diff2_rate1;		/* 0x2B8 */
	u32 kvco_ideal_freq_diff1_rate2;		/* 0x2BC */
	u32 kvco_ideal_freq_diff2_rate2;		/* 0x2C0 */
	u8 _reserved18[0x18];				/* 0x2C4-0x2DB */
	u32 kp_code_ovrd_rate_2_3;			/* 0x2DC */
	u8 _reserved19[0xC];				/* 0x2E0-0x2EB */
	u32 kp_cal_upper_freq_diff_bnd1_rate1;		/* 0x2EC */
	u32 kp_cal_upper_freq_diff_bnd2_rate1;		/* 0x2F0 */
	u32 kp_cal_upper_freq_diff_bnd1_rate2;		/* 0x2F4 */
	u32 kp_cal_upper_freq_diff_bnd2_rate2;		/* 0x2F8 */
	u8 _reserved20[0x14];				/* 0x2FC-0x30F */
	u32 kp_cal_lower_freq_diff_bnd_rate1;		/* 0x310 */
	u32 kp_cal_lower_freq_diff_bnd_rate2;		/* 0x314 */
	u8 _reserved21[0x1C];				/* 0x318-0x333 */
	u32 rx_summer_cal_spd_mode_rate_0123;		/* 0x334 */
	u8 _reserved22[0x8];				/* 0x338-0x33F */
	u32 rx_ivcm_cal_code_override_rate1;		/* 0x340 */
	u32 rx_ivcm_cal_code_override_rate2;		/* 0x344 */
	u8 _reserved23[0xC];				/* 0x348-0x353 */
	u32 rx_ivcm_cal_ctrl2;				/* 0x354 */
	u32 rx_ivcm_cal_ctrl3;				/* 0x358 */
	u8 _reserved24[0x8];				/* 0x35C-0x363 */
	u32 rx_ivcm_postcal_offset_rate1;		/* 0x364 */
	u32 rx_ivcm_postcal_offset_rate2;		/* 0x368 */
	u8 _reserved25[0x3C];				/* 0x36C-0x3A7 */
	u32 sigdet_enables;				/* 0x3A8 */
	u32 sigdet_cntrl;				/* 0x3AC */
	u32 sigdet_lvl;					/* 0x3B0 */
	u32 sigdet_deglitch_cntrl;			/* 0x3B4 */
	u32 sigdet_cal_ctrl1;				/* 0x3B8 */
	u32 sigdet_cal_ctrl2_and_cdr_lock_edge;		/* 0x3BC */
	u32 sigdet_cal_trim;				/* 0x3C0 */
	u8 _reserved26[0x8];				/* 0x3C4-0x3CB */
	u32 freq_lock_det_dly_rate1;			/* 0x3CC */
	u32 freq_lock_det_dly_rate2;			/* 0x3D0 */
	u8 _reserved27[0x24];				/* 0x3D4-0x3F7 */
	u32 cdr_cp_cur_fll_rate1;			/* 0x3F8 */
	u32 cdr_cp_cur_fll_rate2;			/* 0x3FC */
	u8 _reserved28[0xC];				/* 0x400-0x40B */
	u32 cdr_cp_cur_pll_rate1;			/* 0x40C */
	u32 cdr_cp_cur_pll_rate2;			/* 0x410 */
	u8 _reserved29[0x8];				/* 0x414-0x41B */
	u32 cdr_fll_div_ratio_rate_0123;		/* 0x41C */
	u8 _reserved30[0x4];				/* 0x420-0x423 */
	u32 cdr_loop_ccode_rate_01;			/* 0x424 */
	u32 cdr_loop_ccode_rate_23;			/* 0x428 */
	u8 _reserved31[0x4];				/* 0x42C-0x42F */
	u32 cdr_loop_rcode_fast_rate_0_1;		/* 0x430 */
	u32 cdr_loop_rcode_fast_rate_2_3;		/* 0x434 */
	u8 _reserved32[0x4];				/* 0x438-0x43B */
	u32 cdr_loop_rcode_fll_rate_0_1;		/* 0x43C */
	u32 cdr_loop_rcode_fll_rate_2_3;		/* 0x440 */
	u8 _reserved33[0x4];				/* 0x444-0x447 */
	u32 cdr_loop_rcode_pll_rate_0_1;		/* 0x448 */
	u32 cdr_loop_rcode_pll_rate_2_3;		/* 0x44C */
	u8 _reserved34[0x4];				/* 0x450-0x453 */
	u32 cdr_vco_cap_code_rate_0123;			/* 0x454 */
	u8 _reserved35[0x4];				/* 0x458-0x45B */
	u32 cdr_vco_type_config;			/* 0x45C */
	u32 cdr_vco_en_lowfreq;				/* 0x460 */
	u8 _reserved36[0x4];				/* 0x464-0x467 */
	u32 cdr_loop_func_ctrl;				/* 0x468 */
	u8 _reserved37[0x18];				/* 0x46C-0x483 */
	u32 gm_cal_en;					/* 0x484 */
	u32 gm_cal_res_rate0_1;				/* 0x488 */
	u32 gm_cal_res_rate2_3;				/* 0x48C */
	u8 _reserved38[0xC];				/* 0x490-0x49B */
	u32 aux_clk_ctrl;				/* 0x49C */
	u8 _reserved39[0x8];				/* 0x4A0-0x4A7 */
	u32 eom_ctrl1;					/* 0x4A8 */
	u8 _reserved40[0x28];				/* 0x4AC-0x4D3 */
	u32 rx_equ_adaptor_cntrl2;			/* 0x4D4 */
	u32 rx_equ_adaptor_cntrl3;			/* 0x4D8 */
	u32 rx_equ_adaptor_cntrl4;			/* 0x4DC */
	u8 _reserved41[0x1C];				/* 0x4E0-0x4FB */
	u32 ctle_post_cal_offset_rate_0_1_2;		/* 0x4FC */
	u8 _reserved42[0xC];				/* 0x500-0x50B */
	u32 vga_cal_cntrl1;				/* 0x50C */
	u8 _reserved43[0x4];				/* 0x510-0x513 */
	u32 vga_cal_man_val_rate0_1;			/* 0x514 */
	u32 vga_cal_man_val_rate2_3;			/* 0x518 */
	u8 _reserved44[0x78];				/* 0x51C-0x593 */
	u32 dfe_tap1_dac_enable;			/* 0x594 */
	u32 dfe_tap2_dac_enable;			/* 0x598 */
	u32 dfe_tap345_dac_enable;			/* 0x59C */
	u32 dfe_tap67_dac_enable;			/* 0x5A0 */
	u32 cdr_iqtune_ctrl;				/* 0x5A4 */
	u8 _reserved45[0x4];				/* 0x5A8-0x5AB */
	u32 cdr_iqtune_man_index;			/* 0x5AC */
	u8 _reserved46[0x38];				/* 0x5B0-0x5E7 */
	u32 cdr_iqtune_div2_ctrl_rate0123;		/* 0x5E8 */
	u8 _reserved47[0x38];				/* 0x5EC-0x623 */
	u32 cdr_vco_cap_code_ovrd_muxes;		/* 0x624 */
	u8 _reserved48[0x270];				/* 0x628-0x897 */
	u32 dig_bkup_ctrl16;				/* 0x898 */
	u32 dig_bkup_ctrl_v2_5;			/* 0x89C */
	u8 _reserved49[0x360];				/* 0x8A0-0xBFF (remaining space) */
} __packed;

check_member(usb43dp_qserdes_lane_reg_layout, bist_mode_laneno, 0x000);
check_member(usb43dp_qserdes_lane_reg_layout, clkbuf_enable, 0x05C);
check_member(usb43dp_qserdes_lane_reg_layout, tx_lvl_update_ctrl, 0x084);
check_member(usb43dp_qserdes_lane_reg_layout, pcie5_top_ldo_code_ctrl1, 0x0A0);
check_member(usb43dp_qserdes_lane_reg_layout, transmitter_en_ctrl, 0x0B0);
check_member(usb43dp_qserdes_lane_reg_layout, lane_mode_1, 0x0CC);
check_member(usb43dp_qserdes_lane_reg_layout, tx0_restrim_cal_ctrl, 0x0EC);
check_member(usb43dp_qserdes_lane_reg_layout, tx0_restrim_post_cal_offset, 0x0F4);
check_member(usb43dp_qserdes_lane_reg_layout, tx1_restrim_cal_ctrl, 0x100);
check_member(usb43dp_qserdes_lane_reg_layout, tx0_restrim_vref_sel, 0x10C);
check_member(usb43dp_qserdes_lane_reg_layout, ana_interface_select2, 0x128);
check_member(usb43dp_qserdes_lane_reg_layout, pcs_interface_select1, 0x130);
check_member(usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b0, 0x154);
check_member(usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b0, 0x174);
check_member(usb43dp_qserdes_lane_reg_layout, tx_dcc_ana_ctrl2, 0x1D8);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_vco_ctune_meas_cnt1_rate1, 0x248);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_vctrl_rate_2_3, 0x26C);
check_member(usb43dp_qserdes_lane_reg_layout, kvco_init_rate_0_1, 0x274);
check_member(usb43dp_qserdes_lane_reg_layout, kvco_code_ovrd_rate1, 0x284);
check_member(usb43dp_qserdes_lane_reg_layout, kvco_ideal_freq_diff1_rate1, 0x2B4);
check_member(usb43dp_qserdes_lane_reg_layout, kp_code_ovrd_rate_2_3, 0x2DC);
check_member(usb43dp_qserdes_lane_reg_layout, kp_cal_upper_freq_diff_bnd1_rate1, 0x2EC);
check_member(usb43dp_qserdes_lane_reg_layout, kp_cal_lower_freq_diff_bnd_rate1, 0x310);
check_member(usb43dp_qserdes_lane_reg_layout, rx_summer_cal_spd_mode_rate_0123, 0x334);
check_member(usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_code_override_rate1, 0x340);
check_member(usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_ctrl2, 0x354);
check_member(usb43dp_qserdes_lane_reg_layout, rx_ivcm_postcal_offset_rate1, 0x364);
check_member(usb43dp_qserdes_lane_reg_layout, sigdet_enables, 0x3A8);
check_member(usb43dp_qserdes_lane_reg_layout, sigdet_cal_trim, 0x3C0);
check_member(usb43dp_qserdes_lane_reg_layout, freq_lock_det_dly_rate1, 0x3CC);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_fll_rate1, 0x3F8);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_pll_rate1, 0x40C);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_fll_div_ratio_rate_0123, 0x41C);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_loop_ccode_rate_01, 0x424);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fast_rate_0_1, 0x430);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fll_rate_0_1, 0x43C);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_pll_rate_0_1, 0x448);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_vco_cap_code_rate_0123, 0x454);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_vco_type_config, 0x45C);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_loop_func_ctrl, 0x468);
check_member(usb43dp_qserdes_lane_reg_layout, gm_cal_en, 0x484);
check_member(usb43dp_qserdes_lane_reg_layout, aux_clk_ctrl, 0x49C);
check_member(usb43dp_qserdes_lane_reg_layout, eom_ctrl1, 0x4A8);
check_member(usb43dp_qserdes_lane_reg_layout, rx_equ_adaptor_cntrl2, 0x4D4);
check_member(usb43dp_qserdes_lane_reg_layout, ctle_post_cal_offset_rate_0_1_2, 0x4FC);
check_member(usb43dp_qserdes_lane_reg_layout, vga_cal_cntrl1, 0x50C);
check_member(usb43dp_qserdes_lane_reg_layout, vga_cal_man_val_rate0_1, 0x514);
check_member(usb43dp_qserdes_lane_reg_layout, dfe_tap1_dac_enable, 0x594);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_iqtune_ctrl, 0x5A4);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_iqtune_man_index, 0x5AC);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_iqtune_div2_ctrl_rate0123, 0x5E8);
check_member(usb43dp_qserdes_lane_reg_layout, cdr_vco_cap_code_ovrd_muxes, 0x624);
check_member(usb43dp_qserdes_lane_reg_layout, dig_bkup_ctrl16, 0x898);
check_member(usb43dp_qserdes_lane_reg_layout, dig_bkup_ctrl_v2_5, 0x89C);

/* Lane table - USB43DP QSERDES lane registers per PHY HSR.
 * Applied to both Lane A (tx_base) and Lane B (txb_base). */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_lane_tbl[] = {
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, bist_mode_laneno), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, clkbuf_enable), 0x85},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx_lvl_update_ctrl), 0x0D},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, pcie5_top_ldo_code_ctrl1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, pcie5_top_ldo_code_ctrl2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, pcie5_top_ldo_code_ctrl3), 0x80},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, pcie5_top_ldo_code_ctrl4), 0x8E},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, transmitter_en_ctrl), 0x13},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, lane_mode_1), 0x0C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, lane_mode_2), 0x40},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, lane_mode_3), 0x11},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, lane_mode_4), 0x01},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx0_restrim_cal_ctrl), 0x20},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx0_restrim_post_cal_offset), 0x10},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx1_restrim_cal_ctrl), 0x02},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx0_restrim_vref_sel), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx1_restrim_vref_sel), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, ana_interface_select2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, pcs_interface_select1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b0), 0xA4},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b1), 0xA2},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b2), 0x6E},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b3), 0x51},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b4), 0x0A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b5), 0x26},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b6), 0x12},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate_0_1_b7), 0x2A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b0), 0x1C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b1), 0x93},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b2), 0x7C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b3), 0x64},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b4), 0x04},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b5), 0x57},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b6), 0x12},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_mode_rate2_b7), 0x1A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, tx_dcc_ana_ctrl2), 0x0C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_ctune_meas_cnt1_rate1), 0x26},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_ctune_meas_cnt2_rate1), 0x26},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_ctune_meas_cnt1_rate2), 0x26},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_ctune_meas_cnt2_rate2), 0x26},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vctrl_rate_2_3), 0x33},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_init_rate_0_1), 0x11},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_init_rate_2_3), 0x11},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_code_ovrd_rate1), 0x03},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_code_ovrd_rate2), 0x03},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_ideal_freq_diff1_rate1), 0x15},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_ideal_freq_diff2_rate1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_ideal_freq_diff1_rate2), 0x19},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kvco_ideal_freq_diff2_rate2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_code_ovrd_rate_2_3), 0x22},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_upper_freq_diff_bnd1_rate1), 0xFF},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_upper_freq_diff_bnd2_rate1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_upper_freq_diff_bnd1_rate2), 0xFF},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_upper_freq_diff_bnd2_rate2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_lower_freq_diff_bnd_rate1), 0x16},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, kp_cal_lower_freq_diff_bnd_rate2), 0x0A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_summer_cal_spd_mode_rate_0123), 0x2F},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_code_override_rate1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_code_override_rate2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_ctrl2), 0x85},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_cal_ctrl3), 0x45},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_postcal_offset_rate1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_ivcm_postcal_offset_rate2), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_enables), 0x0C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_cntrl), 0xA3},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_lvl), 0x04},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_deglitch_cntrl), 0x0E},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_cal_ctrl1), 0x14},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_cal_ctrl2_and_cdr_lock_edge), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, sigdet_cal_trim), 0x66},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, freq_lock_det_dly_rate1), 0xFF},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, freq_lock_det_dly_rate2), 0x32},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_fll_rate1), 0x07},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_fll_rate2), 0x0A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_pll_rate1), 0x05},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_cp_cur_pll_rate2), 0x05},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_fll_div_ratio_rate_0123), 0xD5},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_ccode_rate_01), 0x76},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_ccode_rate_23), 0x55},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fast_rate_0_1), 0x20},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fast_rate_2_3), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fll_rate_0_1), 0x33},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_fll_rate_2_3), 0x43},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_pll_rate_0_1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_rcode_pll_rate_2_3), 0x51},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_cap_code_rate_0123), 0xF5},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_type_config), 0x1F},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_en_lowfreq), 0x07},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_loop_func_ctrl), 0xD0},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, gm_cal_en), 0x1F},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, gm_cal_res_rate0_1), 0x88},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, gm_cal_res_rate2_3), 0x88},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, aux_clk_ctrl), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, eom_ctrl1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_equ_adaptor_cntrl2), 0x0A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_equ_adaptor_cntrl3), 0x0A},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, rx_equ_adaptor_cntrl4), 0xAA},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, ctle_post_cal_offset_rate_0_1_2), 0x77},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, vga_cal_cntrl1), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, vga_cal_man_val_rate0_1), 0xDD},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, vga_cal_man_val_rate2_3), 0xD8},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dfe_tap1_dac_enable), 0x1C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dfe_tap2_dac_enable), 0x1C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dfe_tap345_dac_enable), 0x18},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dfe_tap67_dac_enable), 0x10},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_iqtune_ctrl), 0x00},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_iqtune_man_index), 0x10},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_iqtune_div2_ctrl_rate0123), 0x1C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, cdr_vco_cap_code_ovrd_muxes), 0x1C},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dig_bkup_ctrl16), 0x37},
	{(u32)offsetof(struct usb43dp_qserdes_lane_reg_layout, dig_bkup_ctrl_v2_5), 0x00},
};

/* PCS MISC table */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_pcs_misc_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_misc_reg_layout, pcs_misc_config1), 0x01},
};

/* PCS table */
static const qmp_phy_init_tbl_t qmp_v4_usb3_ss0_pcs_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_power_state_config2), 0x3D},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l), 0xE7},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h), 0x03},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config1), 0xC4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config2), 0x89},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config3), 0x20},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config6), 0x13},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_refgen_req_config1), 0x21},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl), 0x55},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_tsync_rsync_time), 0xA4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rx_config), 0x0A},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_tsync_dly_time), 0x04},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config1), 0xD4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config2), 0x30},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config), 0x0C},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config1), 0x4B},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config5), 0x00},
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
		.lanea_base = (void *)QMP_PHY_SS0_QSERDES_LA_REG_BASE,
		.rx_base = NULL,
		.laneb_base = (void *)QMP_PHY_SS0_QSERDES_LB_REG_BASE,
		.rxb_base = NULL,
		.pcs_base = (void *)QMP_PHY_SS0_PCS_REG_BASE,
		.pcs_misc_base = (void *)QMP_PHY_SS0_PCS_MISC_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_SS0_PCS_USB3_REG_BASE,
		.name = "SS0",
		.serdes_tbl = qmp_v4_usb3_ss0_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_serdes_tbl),
		.lane_tbl = qmp_v4_usb3_ss0_lane_tbl,
		.lane_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_lane_tbl),
		.rx_tbl = NULL,
		.rx_tbl_num = 0,
		.pcs_tbl = qmp_v4_usb3_ss0_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_tbl),
		.pcs_misc_tbl = qmp_v4_usb3_ss0_pcs_misc_tbl,
		.pcs_misc_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_misc_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_ss0_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_usb3_tbl),
	},
	[1] = { /* SS1 PHY */
		.com_base = (void *)QMP_PHY_SS1_COM_REG_BASE,
		.qserdes_pll_base = (void *)QMP_PHY_SS1_QSERDES_PLL_REG_BASE,
		.lanea_base = (void *)QMP_PHY_SS1_QSERDES_LA_REG_BASE,
		.rx_base = NULL,
		.laneb_base = (void *)QMP_PHY_SS1_QSERDES_LB_REG_BASE,
		.rxb_base = NULL,
		.pcs_base = (void *)QMP_PHY_SS1_PCS_REG_BASE,
		.pcs_misc_base = (void *)QMP_PHY_SS1_PCS_MISC_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_SS1_PCS_USB3_REG_BASE,
		.name = "SS1",
		.serdes_tbl = qmp_v4_usb3_ss0_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_serdes_tbl),
		.lane_tbl = qmp_v4_usb3_ss0_lane_tbl,
		.lane_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_lane_tbl),
		.rx_tbl = NULL,
		.rx_tbl_num = 0,
		.pcs_tbl = qmp_v4_usb3_ss0_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_tbl),
		.pcs_misc_tbl = qmp_v4_usb3_ss0_pcs_misc_tbl,
		.pcs_misc_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_misc_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_ss0_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_usb3_tbl),
	},
	[2] = { /* SS2 PHY (C2/Tert) */
		.com_base = (void *)QMP_PHY_SS2_COM_REG_BASE,
		.qserdes_pll_base = (void *)QMP_PHY_SS2_QSERDES_PLL_REG_BASE,
		.lanea_base = (void *)QMP_PHY_SS2_QSERDES_LA_REG_BASE,
		.rx_base = NULL,
		.laneb_base = (void *)QMP_PHY_SS2_QSERDES_LB_REG_BASE,
		.rxb_base = NULL,
		.pcs_base = (void *)QMP_PHY_SS2_PCS_REG_BASE,
		.pcs_misc_base = (void *)QMP_PHY_SS2_PCS_MISC_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_SS2_PCS_USB3_REG_BASE,
		.name = "SS2",
		.serdes_tbl = qmp_v4_usb3_ss0_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_serdes_tbl),
		.lane_tbl = qmp_v4_usb3_ss0_lane_tbl,
		.lane_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_lane_tbl),
		.rx_tbl = NULL,
		.rx_tbl_num = 0,
		.pcs_tbl = qmp_v4_usb3_ss0_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_tbl),
		.pcs_misc_tbl = qmp_v4_usb3_ss0_pcs_misc_tbl,
		.pcs_misc_tbl_num = ARRAY_SIZE(qmp_v4_usb3_ss0_pcs_misc_tbl),
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
enum cb_err qmp_usb4_dp_phy_ss_init(int phy_instance, bool polarity_inverse)
{
	unsigned int lane_num = 0;
	if (phy_instance < 0 || phy_instance >= ARRAY_SIZE(qmp_phy_ss_instance)) {
		printk(BIOS_ERR, "Invalid PHY instance %d\n", phy_instance);
		return CB_ERR;
	}

	const struct ss_usb_phy_reg *ss_phy_reg = &qmp_phy_ss_instance[phy_instance];
	struct usb3_phy_pcs_reg_layout *current_pcs_reg = (struct usb3_phy_pcs_reg_layout *)ss_phy_reg->pcs_base;
	struct usb43dp_com_reg_layout *current_com_reg = (struct usb43dp_com_reg_layout *)ss_phy_reg->com_base;

	/* Swap lanes based on polarity */
	if (polarity_inverse)
		lane_num = 1;

	/* Configure SW_PORTSELECT and SW_PORTSELECT_MUX using coreboot style */
	clrsetbits32(&current_com_reg->typec_ctrl,
					SW_PORTSELECT_MASK | SW_PORTSELECT_MUX_MASK,
					(lane_num << SW_PORTSELECT_SHIFT) | SW_PORTSELECT_MUX_MASK);

	/* Step 1: COM registers */
	qcom_qmp_phy_configure(ss_phy_reg->com_base,
						qmp_v4_usb3_ss0_com_tbl,
						ARRAY_SIZE(qmp_v4_usb3_ss0_com_tbl));

	/* Step 1b: Disable LF toggle of USB3 output clocks via USB43DP_AON. */
	{
		struct usb43dp_aon_reg_layout *aon_reg =
			(struct usb43dp_aon_reg_layout *)
			((uintptr_t)ss_phy_reg->com_base + USB43DP_AON_OFFSET_FROM_COM);
		write32(&aon_reg->usb3_aon_toggle_enable, USB3_AON_TOGGLE_DISABLE);
		printk(BIOS_INFO,
		       "QMP PHY %s config: polarity=%s TYPEC_CTRL=0x%08x AON_TOGGLE=0x%08x\n",
		       ss_phy_reg->name, polarity_inverse ? "inverted" : "normal",
		       read32(&current_com_reg->typec_ctrl),
		       read32(&aon_reg->usb3_aon_toggle_enable));
	}

	/* Step 2: power up USB3 PHY */
	write32(&current_pcs_reg->pcs_power_down_control, 0x01);

	/* Step 3: PLL/Serdes registers */
	qcom_qmp_phy_configure(ss_phy_reg->qserdes_pll_base,
						ss_phy_reg->serdes_tbl,
						ss_phy_reg->serdes_tbl_num);

	/* Step 4: PCS MISC registers (USB3_PCS_MISC module, base = PCS_MISC_REG_BASE) */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_misc_base,
						ss_phy_reg->pcs_misc_tbl,
						ss_phy_reg->pcs_misc_tbl_num);

	/* Step 5: PCS registers */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_base,
						ss_phy_reg->pcs_tbl,
						ss_phy_reg->pcs_tbl_num);

	/* Step 6: PCS USB3 registers */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_usb3_base,
						ss_phy_reg->pcs_usb3_tbl,
						ss_phy_reg->pcs_usb3_tbl_num);

	/* Step 7: Lane A registers (USB43DP_QSERDES_LA) */
	qcom_qmp_phy_configure(ss_phy_reg->lanea_base,
						ss_phy_reg->lane_tbl,
						ss_phy_reg->lane_tbl_num);

	/* Step 8: Lane B registers (USB43DP_QSERDES_LB) */
	qcom_qmp_phy_configure(ss_phy_reg->laneb_base,
						ss_phy_reg->lane_tbl,
						ss_phy_reg->lane_tbl_num);

	udelay(100);

	/* perform software reset of USB43DP_COM */
	write32(&current_com_reg->sw_reset, PCS_SW_RESET_DEASSERT);

	write32(&current_pcs_reg->pcs_sw_reset, PCS_SW_RESET_DEASSERT);
	write32(&current_pcs_reg->pcs_start_control, QPHY_PCS_START | QPHY_SERDES_START);

	udelay(100);

	long lock_us = wait_us(10000,
			!(read32(&current_pcs_reg->pcs_pcs_status1) &
			USB3_PCS_PHYSTATUS));

	/* Read the final PLL and PHY status for success or failure reporting. */
	void *cmn_status_addr = (void *)((uintptr_t)ss_phy_reg->qserdes_pll_base + 0x314);
	uint32_t cmn_status  = read32(cmn_status_addr);
	uint32_t phy_status  = read32(&current_pcs_reg->pcs_pcs_status1);

	if (!lock_us) {
		printk(BIOS_ERR,
		       "QMP PHY %s PLL lock failed: CMN_STATUS=0x%08x PCS_STATUS1=0x%08x\n",
		       ss_phy_reg->name, cmn_status, phy_status);
		return CB_ERR;
	}

	printk(BIOS_INFO,
	       "QMP PHY %s locked in %ldus: CMN_STATUS=0x%08x PCS_STATUS1=0x%08x\n",
	       ss_phy_reg->name, lock_us, cmn_status, phy_status);

	return CB_SUCCESS;
}
