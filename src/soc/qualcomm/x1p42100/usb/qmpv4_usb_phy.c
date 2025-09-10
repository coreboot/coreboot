/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <timer.h>
#include <delay.h>
#include <soc/usb/qmp_usb_phy.h>
#include <soc/addressmap.h>

/* Only for QMP V4 PHY - QSERDES COM registers */
struct usb3_phy_qserdes_com_reg_layout {
	u32 com_ssc_step_size1_mode1;
	u32 com_ssc_step_size2_mode1;
	u32 com_ssc_step_size3_mode1;
	u32 com_clk_ep_div_mode1;
	u32 com_cp_ctrl_mode1;
	u32 com_pll_rctrl_mode1;
	u32 com_pll_cctrl_mode1;
	u32 com_coreclk_div_mode1;
	u32 com_lock_cmp1_mode1;
	u32 com_lock_cmp2_mode1;
	u32 com_dec_start_mode1;
	u32 com_dec_start_msb_mode1;
	u32 com_div_frac_start1_mode1;
	u32 com_div_frac_start2_mode1;
	u32 com_div_frac_start3_mode1;
	u32 com_hsclk_sel_1;
	u32 com_integloop_gain0_mode1;
	u32 com_integloop_gain1_mode1;
	u32 com_vco_tune1_mode1;
	u32 com_vco_tune2_mode1;
	u32 com_bin_vcocal_cmp_code1_mode1;
	u32 com_bin_vcocal_cmp_code2_mode1;
	u32 com_bin_vcocal_cmp_code1_mode0;
	u32 com_bin_vcocal_cmp_code2_mode0;
	u32 com_ssc_step_size1_mode0;
	u32 com_ssc_step_size2_mode0;
	u32 com_ssc_step_size3_mode0;
	u32 com_clk_ep_div_mode0;
	u32 com_cp_ctrl_mode0;
	u32 com_pll_rctrl_mode0;
	u32 com_pll_cctrl_mode0;
	u32 com_coreclk_div_mode0;
	u32 com_lock_cmp1_mode0;
	u32 com_lock_cmp2_mode0;
	u32 com_dec_start_mode0;
	u32 com_dec_start_msb_mode0;
	u32 com_div_frac_start1_mode0;
	u32 com_div_frac_start2_mode0;
	u32 com_div_frac_start3_mode0;
	u32 com_hsclk_hs_switch_sel_1;
	u32 com_integloop_gain0_mode0;
	u32 com_integloop_gain1_mode0;
	u32 com_vco_tune1_mode0;
	u32 com_vco_tune2_mode0;
	u32 com_atb_sel1;
	u32 com_atb_sel2;
	u32 com_freq_update;
	u32 com_bg_timer;
	u32 com_ssc_en_center;
	u32 com_ssc_adj_per1;
	u32 com_ssc_adj_per2;
	u32 com_ssc_per1;
	u32 com_ssc_per2;
	u8 _reserved1[20];
	u32 com_sysclk_buf_enable;
	u8 _reserved2[36];
	u32 com_sysclk_en_sel;
	u8 _reserved3[16];
	u32 com_lock_cmp_cfg;
	u8 _reserved4[24];
	u32 com_vco_tune_map;
	u8 _reserved5[44];
	u32 com_core_clk_en;
	u32 com_cmn_config_1;
	u8 _reserved6[44];
	u32 com_auto_gain_adj_ctrl_1;
	u32 com_auto_gain_adj_ctrl_2;
	u32 com_auto_gain_adj_ctrl_3;
	u32 com_auto_gain_adj_ctrl_4;
	u32 com_additional_misc;
};

check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_step_size1_mode1, 0x00);
check_member(usb3_phy_qserdes_com_reg_layout, com_sysclk_buf_enable, 0xe8);
check_member(usb3_phy_qserdes_com_reg_layout, com_sysclk_en_sel, 0x110);
check_member(usb3_phy_qserdes_com_reg_layout, com_lock_cmp_cfg, 0x124);
check_member(usb3_phy_qserdes_com_reg_layout, com_integloop_gain0_mode0, 0x0a0);
check_member(usb3_phy_qserdes_com_reg_layout, com_vco_tune_map, 0x0140);
check_member(usb3_phy_qserdes_com_reg_layout, com_core_clk_en, 0x170);
check_member(usb3_phy_qserdes_com_reg_layout, com_auto_gain_adj_ctrl_1, 0x1a4);

/* Only for QMP V4 PHY - TX registers */
struct usb3_phy_qserdes_tx_reg_layout {
	u8 _reserved1[52];
	u32 tx_res_code_lane_tx;
	u32 tx_res_code_lane_rx;
	u32 tx_res_code_lane_offset_tx;
	u32 tx_res_code_lane_offset_rx;
	u8 _reserved2[64];
	u32 tx_lane_mode_1;
	u32 tx_lane_mode_2;
	u32 tx_lane_mode_3;
	u32 tx_lane_mode_4;
	u32 tx_lane_mode_5;
	u8 _reserved3[12];
	u32 tx_rcv_detect_lvl_2;
	u8 _reserved4[60];
	u32 tx_pi_qec_ctrl;
};
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_tx, 0x034);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1, 0x084);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_rcv_detect_lvl_2, 0x0a4);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_pi_qec_ctrl, 0x0e4);

/* Only for QMP V4 PHY - RX registers */
struct usb3_phy_qserdes_rx_reg_layout {
	u8 _reserved1[8];
	u32 rx_ucdr_fo_gain;
	u8 _reserved2[8];
	u32 rx_ucdr_so_gain;
	u8 _reserved3[24];
	u32 rx_ucdr_fastlock_fo_gain;
	u32 rx_ucdr_so_saturation_and_enable;
	u32 rx_ucdr_fo_to_so_delay;
	u32 rx_ucdr_fastlock_count_low;
	u32 rx_ucdr_fastlock_count_high;
	u32 rx_ucdr_pi_controls;
	u32 rx_ucdr_pi_ctrl2;
	u32 rx_ucdr_sb2_thresh1;
	u32 rx_ucdr_sb2_thresh2;
	u32 rx_ucdr_sb2_gain1;
	u32 rx_ucdr_sb2_gain2;
	u32 rx_aux_control;
	u32 rx_aux_data_tcoarse_tfine;
	u8 _reserved4[112];
	u32 rx_vga_cal_cntrl1;
	u32 rx_vga_cal_cntrl2;
	u32 rx_gm_cal;
	u8 _reserved5[12];
	u32 rx_rx_equ_adaptor_cntrl2;
	u32 rx_rx_equ_adaptor_cntrl3;
	u32 rx_rx_equ_adaptor_cntrl4;
	u32 rx_rx_idac_tsettle_low;
	u32 rx_rx_idac_tsettle_high;
	u8 _reserved6[16];
	u32 rx_rx_eq_offset_adaptor_cntrl1;
	u32 rx_rx_offset_adaptor_cntrl2;
	u32 rx_sigdet_enables;
	u32 rx_sigdet_cntrl;
	u32 rx_sigdet_lvl;
	u32 rx_sigdet_deglitch_cntrl;
	u8 _reserved7[52];
	u32 rx_rx_mode_00_low;
	u32 rx_rx_mode_00_high;
	u32 rx_rx_mode_00_high2;
	u32 rx_rx_mode_00_high3;
	u32 rx_rx_mode_00_high4;
	u32 rx_rx_mode_01_low;
	u32 rx_rx_mode_01_high;
	u32 rx_rx_mode_01_high2;
	u32 rx_rx_mode_01_high3;
	u32 rx_rx_mode_01_high4;
	u32 rx_rx_mode_10_low;
	u32 rx_rx_mode_10_high;
	u32 rx_rx_mode_10_high2;
	u32 rx_rx_mode_10_high3;
	u32 rx_rx_mode_10_high4;
	u8 _reserved8[8];
	u32 rx_dfe_en_timer;
	u32 rx_dfe_ctle_post_cal_offset;
	u32 rx_dcc_ctrl1;
	u32 rx_dcc_ctrl2;
	u32 rx_vth_code;
	u8 _reserved9[48];
	u32 rx_sigdet_cal_ctrl1;
	u8 _reserved10[16];
	u32 rx_sigdet_cal_trim;
};

check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain, 0x014);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_fo_gain, 0x030);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl1, 0x0d4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl2, 0x0ec);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl3, 0x0f0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl4, 0x0f4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_eq_offset_adaptor_cntrl1, 0x110);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cntrl, 0x11c);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_low, 0x15c);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dfe_en_timer, 0x1a0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_dcc_ctrl1, 0x1a8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vth_code, 0x1b0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cal_trim, 0x1f8);

/* Only for QMP V4 PHY - PCS registers */
struct usb3_phy_pcs_reg_layout {
	u32 pcs_sw_reset;
	u8 _reserved1[16];
	u32 pcs_pcs_status1;
	u8 _reserved2[40];
	u32 pcs_power_down_control;
	u32 pcs_start_control;
	u8 _reserved3[124];
	u32 pcs_lock_detect_config1;
	u32 pcs_lock_detect_config2;
	u32 pcs_lock_detect_config3;
	u8 _reserved4[8];
	u32 pcs_lock_detect_config6;
	u32 pcs_refgen_req_config1;
	u8 _reserved5[168];
	u32 pcs_rx_sigdet_lvl;
	u8 _reserved6[4];
	u32 pcs_rcvr_dtct_dly_p1u2_l;
	u32 pcs_rcvr_dtct_dly_p1u2_h;
	u8 _reserved7[24];
	u32 pcs_cdr_reset_time;
	u8 _reserved8[12];
	u32 pcs_align_detect_config1;
	u32 pcs_align_detect_config2;
	u8 _reserved9[8];
	u32 pcs_pcs_tx_rx_config;
	u8 _reserved10[8];
	u32 pcs_eq_config1;
	u8 _reserved11[12];
	u32 pcs_eq_config5;
};

check_member(usb3_phy_pcs_reg_layout, pcs_pcs_status1, 0x014);
check_member(usb3_phy_pcs_reg_layout, pcs_power_down_control, 0x040);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config1, 0x0c4);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config6, 0x0d8);
check_member(usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl, 0x188);
check_member(usb3_phy_pcs_reg_layout, pcs_cdr_reset_time, 0x1b0);
check_member(usb3_phy_pcs_reg_layout, pcs_align_detect_config1, 0x1c0);
check_member(usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config, 0x1d0);
check_member(usb3_phy_pcs_reg_layout, pcs_eq_config1, 0x1dc);
check_member(usb3_phy_pcs_reg_layout, pcs_eq_config5, 0x1ec);


struct usb3_phy_pcs_usb3_reg_layout {
	u32 reserved[6];
	u32 pcs_usb3_lfps_det_high_count_val;
	u32 reserved1[8];
	u32 pcs_usb3_rxeqtraining_dfe_time_s2;
	u32 pcs_usb3_rcvr_dtct_dly_u3_l;
	u32 pcs_usb3_rcvr_dtct_dly_u3_h;
};

/* Definition of the shared initialization tables using offsets */
static const qmp_phy_init_tbl_t qmp_v4_usb3_serdes_tbl[] = {
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_step_size1_mode1), 0xc0},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_step_size2_mode1), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_cp_ctrl_mode1), 0x02},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_pll_rctrl_mode1), 0x16},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_pll_cctrl_mode1), 0x36},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_coreclk_div_mode1), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_lock_cmp1_mode1), 0x16},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_lock_cmp2_mode1), 0x41},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_dec_start_mode1), 0x41},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start1_mode1), 0x55},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start2_mode1), 0x75},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start3_mode1), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_hsclk_sel_1), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_vco_tune1_mode1), 0x25},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_vco_tune2_mode1), 0x02},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_bin_vcocal_cmp_code1_mode1), 0x5c},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_bin_vcocal_cmp_code2_mode1), 0x0f},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_bin_vcocal_cmp_code1_mode0), 0x5c},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_bin_vcocal_cmp_code2_mode0), 0x0f},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_step_size1_mode0), 0xc0},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_step_size2_mode0), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_cp_ctrl_mode0), 0x02},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_pll_rctrl_mode0), 0x16},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_pll_cctrl_mode0), 0x36},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_lock_cmp1_mode0), 0x08},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_lock_cmp2_mode0), 0x1a},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_dec_start_mode0), 0x41},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start1_mode0), 0x55},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start2_mode0), 0x75},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_div_frac_start3_mode0), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_vco_tune1_mode0), 0x25},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_vco_tune2_mode0), 0x02},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_bg_timer), 0x0a},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_en_center), 0x01},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_per1), 0x62},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_ssc_per2), 0x02},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_sysclk_buf_enable), 0x0a},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_sysclk_en_sel), 0x1a},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_lock_cmp_cfg), 0x14},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_vco_tune_map), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_core_clk_en), 0x20},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_cmn_config_1), 0x16},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_auto_gain_adj_ctrl_1), 0xb6},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_auto_gain_adj_ctrl_2), 0x4b},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_auto_gain_adj_ctrl_3), 0x37},
	{(u32)offsetof(struct usb3_phy_qserdes_com_reg_layout, com_additional_misc), 0x0c},
};

static const qmp_phy_init_tbl_t qmp_v4_usb3_tx_tbl[] = {
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_tx), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_rx), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_tx), 0x1f},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_rx), 0x09},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1), 0xf5},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_3), 0x3f},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_4), 0x3f},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_5), 0x5f},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_rcv_detect_lvl_2), 0x12},
	{(u32)offsetof(struct usb3_phy_qserdes_tx_reg_layout, tx_pi_qec_ctrl), 0x21},
};

static const qmp_phy_init_tbl_t qmp_v4_usb3_rx_tbl[] = {
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fo_gain), 0x0a},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain), 0x06},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_fo_gain), 0x2f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_saturation_and_enable), 0x7f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_count_low), 0xff},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_count_high), 0x0f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_controls), 0x99},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_thresh1), 0x08},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_thresh2), 0x08},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_gain1), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_ucdr_sb2_gain2), 0x0a},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_aux_data_tcoarse_tfine), 0xa0},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl1), 0x54},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_cntrl2), 0x0f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_gm_cal), 0x13},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl2), 0x0f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl3), 0x4a},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adaptor_cntrl4), 0x0a},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_idac_tsettle_low), 0x07},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_idac_tsettle_high), 0x00},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_eq_offset_adaptor_cntrl1), 0x47},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cntrl), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_deglitch_cntrl), 0x0e},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_low), 0x3f},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_high), 0xbf},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_high2), 0xff},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_high3), 0xdf},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00_high4), 0xed},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_01_low), 0xdc},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_01_high), 0x5c},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_01_high2), 0x9c},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_01_high3), 0x1d},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_01_high4), 0x09},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dfe_en_timer), 0x04},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dfe_ctle_post_cal_offset), 0x38},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_dcc_ctrl1), 0x0c},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_vth_code), 0x10},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cal_ctrl1), 0x14},
	{(u32)offsetof(struct usb3_phy_qserdes_rx_reg_layout, rx_sigdet_cal_trim), 0x08},
};

static const qmp_phy_init_tbl_t qmp_v4_usb3_pcs_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config1), 0xc4},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config2), 0x89},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config3), 0x20},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_lock_detect_config6), 0x13},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_refgen_req_config1), 0x21},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rx_sigdet_lvl), 0xaa},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l), 0xe7},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h), 0x03},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_cdr_reset_time), 0x0a},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config1), 0x88},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_align_detect_config2), 0x13},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_pcs_tx_rx_config), 0x0c},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config1), 0x4b},
	{(u32)offsetof(struct usb3_phy_pcs_reg_layout, pcs_eq_config5), 0x10},
};

static const qmp_phy_init_tbl_t qmp_v4_usb3_pcs_usb3_tbl[] = {
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_lfps_det_high_count_val), 0xf8},
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rxeqtraining_dfe_time_s2), 0x07},
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rcvr_dtct_dly_u3_l), 0x40},
	{(u32)offsetof(struct usb3_phy_pcs_usb3_reg_layout, pcs_usb3_rcvr_dtct_dly_u3_h), 0x00},
};

/* Global instances of PHY data with their respective configuration tables */
static const struct ss_usb_phy_reg qmp_phy_instances[] = {
	{
		.com_base = (void *)QMP_PHY_MP0_QSERDES_COM_REG_BASE,
		.tx_base = (void *)QMP_PHY_MP0_QSERDES_TX_REG_BASE,
		.rx_base = (void *)QMP_PHY_MP0_QSERDES_RX_REG_BASE,
		.pcs_base = (void *)QMP_PHY_MP0_PCS_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_MP0_PCS_USB3_REG_BASE,
		.name = "MP0",
		.serdes_tbl = qmp_v4_usb3_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_serdes_tbl),
		.tx_tbl = qmp_v4_usb3_tx_tbl,
		.tx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_tx_tbl),
		.rx_tbl = qmp_v4_usb3_rx_tbl,
		.rx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_rx_tbl),
		.pcs_tbl = qmp_v4_usb3_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_pcs_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_pcs_usb3_tbl),
	},
	{
		.com_base = (void *)QMP_PHY_MP1_QSERDES_COM_REG_BASE,
		.tx_base = (void *)QMP_PHY_MP1_QSERDES_TX_REG_BASE,
		.rx_base = (void *)QMP_PHY_MP1_QSERDES_RX_REG_BASE,
		.pcs_base = (void *)QMP_PHY_MP1_PCS_REG_BASE,
		.pcs_usb3_base = (void *)QMP_PHY_MP1_PCS_USB3_REG_BASE,
		.name = "MP1",
		.serdes_tbl = qmp_v4_usb3_serdes_tbl,
		.serdes_tbl_num = ARRAY_SIZE(qmp_v4_usb3_serdes_tbl),
		.tx_tbl = qmp_v4_usb3_tx_tbl,
		.tx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_tx_tbl),
		.rx_tbl = qmp_v4_usb3_rx_tbl,
		.rx_tbl_num = ARRAY_SIZE(qmp_v4_usb3_rx_tbl),
		.pcs_tbl = qmp_v4_usb3_pcs_tbl,
		.pcs_tbl_num = ARRAY_SIZE(qmp_v4_usb3_pcs_tbl),
		.pcs_usb3_tbl = qmp_v4_usb3_pcs_usb3_tbl,
		.pcs_usb3_tbl_num = ARRAY_SIZE(qmp_v4_usb3_pcs_usb3_tbl),
	},
};

/* Modified configure function to accept a base address for the current block */
static void qcom_qmp_phy_configure(void *base_addr, const qmp_phy_init_tbl_t tbl[], unsigned int num)
{
	int i;
	const qmp_phy_init_tbl_t *t = tbl;

	if (!t)
		return;

	for (i = 0; i < num; i++, t++)
		write32((void *)((unsigned long)base_addr + t->offset), t->val);
}

/* Unified PHY initialization function that takes PHY instance data */
static bool ss_qmp_phy_init_common(const struct ss_usb_phy_reg *ss_phy_reg)
{
	bool ret = true;
	/* Use the PCS base address from ss_phy_reg to cast to the PCS register layout */
	struct usb3_phy_pcs_reg_layout *current_pcs_reg = (struct usb3_phy_pcs_reg_layout *)ss_phy_reg->pcs_base;

	printk(BIOS_DEBUG, "QMP PHY %s init\n", ss_phy_reg->name);

	/* power up USB3 PHY */
	write32(&current_pcs_reg->pcs_power_down_control, 0x01);

	/* Serdes configuration */
	qcom_qmp_phy_configure(ss_phy_reg->com_base, ss_phy_reg->serdes_tbl,
		ss_phy_reg->serdes_tbl_num);

	/* Tx, Rx, and PCS configurations */
	qcom_qmp_phy_configure(ss_phy_reg->pcs_base, ss_phy_reg->pcs_tbl,
		ss_phy_reg->pcs_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->tx_base, ss_phy_reg->tx_tbl,
		ss_phy_reg->tx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->rx_base, ss_phy_reg->rx_tbl,
		ss_phy_reg->rx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->pcs_usb3_base, ss_phy_reg->pcs_usb3_tbl,
		ss_phy_reg->pcs_usb3_tbl_num);

	udelay(100);

	/* perform software reset of PCS/Serdes */
	write32(&current_pcs_reg->pcs_sw_reset, 0x00);
	/* start PCS/Serdes to operation mode */
	write32(&current_pcs_reg->pcs_start_control, 0x03);

	udelay(100);
	/*
	 * Wait for PHY initialization to be done
	 * PCS_STATUS: wait for 1ms for PHY STATUS;
	 * SW can continuously check for PHYSTATUS = 1.b0.
	 */
	long lock_us = wait_us(10000,
			!(read32(&current_pcs_reg->pcs_pcs_status1)&
			USB3_PCS_PHYSTATUS));
	if (!lock_us) {
		ret = false;
		printk(BIOS_ERR, "QMP PHY %s PLL LOCK fails:\n", ss_phy_reg->name);
	} else {
		printk(BIOS_DEBUG, "QMP PHY %s initialized and locked in %ldus\n",
				ss_phy_reg->name, lock_us);
	}
	return ret;
}

/* Unified ss_qmp_phy_init function to initialize a specific PHY instance
Pass 0 for MP0, or 1 for MP1. */
bool ss_qmp_phy_init(u32 phy_idx)
{
	bool ret = true;

	/* Ensure the provided index is valid */
	if (phy_idx >= ARRAY_SIZE(qmp_phy_instances)) {
		printk(BIOS_ERR, "Invalid PHY index provided: %u\n", phy_idx);
		return false;
	}

	if (!ss_qmp_phy_init_common(&qmp_phy_instances[phy_idx])) {
		ret = false;
	}
	return ret;
}
