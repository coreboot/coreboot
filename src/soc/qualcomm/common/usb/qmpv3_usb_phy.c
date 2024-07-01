/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <timer.h>
#include <soc/usb/qmp_usb_phy.h>
#include <soc/addressmap.h>

/* Only for QMP V3 PHY - QSERDES COM registers */
struct usb3_phy_qserdes_com_reg_layout {
	u8 _reserved1[16];
	u32 com_ssc_en_center;
	u32 com_ssc_adj_per1;
	u32 com_ssc_adj_per2;
	u32 com_ssc_per1;
	u32 com_ssc_per2;
	u32 com_ssc_step_size1;
	u32 com_ssc_step_size2;
	u8 _reserved2[8];
	u32 com_bias_en_clkbuflr_en;
	u32 com_sys_clk_enable1;
	u32 com_sys_clk_ctrl;
	u32 com_sysclk_buf_enable;
	u32 com_pll_en;
	u32 com_pll_ivco;
	u8 _reserved3[20];
	u32 com_cp_ctrl_mode0;
	u8 _reserved4[4];
	u32 com_pll_rctrl_mode0;
	u8 _reserved5[4];
	u32 com_pll_cctrl_mode0;
	u8 _reserved6[12];
	u32 com_sysclk_en_sel;
	u8 _reserved7[8];
	u32 com_resetsm_ctrl2;
	u32 com_lock_cmp_en;
	u32 com_lock_cmp_cfg;
	u32 com_lock_cmp1_mode0;
	u32 com_lock_cmp2_mode0;
	u32 com_lock_cmp3_mode0;
	u8 _reserved8[12];
	u32 com_dec_start_mode0;
	u8 _reserved9[4];
	u32 com_div_frac_start1_mode0;
	u32 com_div_frac_start2_mode0;
	u32 com_div_frac_start3_mode0;
	u8 _reserved10[20];
	u32 com_integloop_gain0_mode0;
	u32 com_integloop_gain1_mode0;
	u8 _reserved11[16];
	u32 com_vco_tune_map;
	u32 com_vco_tune1_mode0;
	u32 com_vco_tune2_mode0;
	u8 _reserved12[60];
	u32 com_clk_select;
	u32 com_hsclk_sel;
	u8 _reserved13[8];
	u32 com_coreclk_div_mode0;
	u8 _reserved14[8];
	u32 com_core_clk_en;
	u32 com_c_ready_status;
	u32 com_cmn_config;
	u32 com_cmn_rate_override;
	u32 com_svs_mode_clk_sel;
};
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_en_center, 0x010);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_adj_per1, 0x014);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_adj_per2, 0x018);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_per1, 0x01c);
check_member(usb3_phy_qserdes_com_reg_layout, com_ssc_per2, 0x020);
check_member(usb3_phy_qserdes_com_reg_layout, com_bias_en_clkbuflr_en, 0x034);
check_member(usb3_phy_qserdes_com_reg_layout, com_pll_ivco, 0x048);
check_member(usb3_phy_qserdes_com_reg_layout, com_cp_ctrl_mode0, 0x060);
check_member(usb3_phy_qserdes_com_reg_layout, com_sysclk_en_sel, 0x080);
check_member(usb3_phy_qserdes_com_reg_layout, com_resetsm_ctrl2, 0x08c);
check_member(usb3_phy_qserdes_com_reg_layout, com_dec_start_mode0, 0x0b0);
check_member(usb3_phy_qserdes_com_reg_layout, com_div_frac_start1_mode0, 0x0b8);
check_member(usb3_phy_qserdes_com_reg_layout, com_integloop_gain0_mode0, 0x0d8);
check_member(usb3_phy_qserdes_com_reg_layout, com_vco_tune_map, 0x0f0);
check_member(usb3_phy_qserdes_com_reg_layout, com_clk_select, 0x138);
check_member(usb3_phy_qserdes_com_reg_layout, com_coreclk_div_mode0, 0x148);
check_member(usb3_phy_qserdes_com_reg_layout, com_core_clk_en, 0x154);
check_member(usb3_phy_qserdes_com_reg_layout, com_svs_mode_clk_sel, 0x164);

/* Only for QMP V3 PHY - TX registers */
struct usb3_phy_qserdes_tx_reg_layout {
	u8 _reserved1[68];
	u32 tx_res_code_lane_offset_tx;
	u32 tx_res_code_lane_offset_rx;
	u8 _reserved2[20];
	u32 tx_highz_drvr_en;
	u8 _reserved3[40];
	u32 tx_lane_mode_1;
	u8 _reserved4[20];
	u32 tx_rcv_detect_lvl_2;
};
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_tx, 0x044);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_res_code_lane_offset_rx, 0x048);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_highz_drvr_en, 0x060);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_lane_mode_1, 0x08c);
check_member(usb3_phy_qserdes_tx_reg_layout, tx_rcv_detect_lvl_2, 0x0a4);

/* Only for QMP V3 PHY - RX registers */
struct usb3_phy_qserdes_rx_reg_layout {
	u8 _reserved1[8];
	u32 rx_ucdr_fo_gain;
	u32 rx_ucdr_so_gain_half;
	u8 _reserved2[32];
	u32 rx_ucdr_fastlock_fo_gain;
	u32 rx_ucdr_so_saturtn_and_en;
	u8 _reserved3[12];
	u32 rx_ucdr_pi_cntrls;
	u8 _reserved4[120];
	u32 rx_vga_cal_ctrl2;
	u8 _reserved5[16];
	u32 rx_rx_equ_adap_ctrl2;
	u32 rx_rx_equ_adap_ctrl3;
	u32 rx_rx_equ_adap_ctrl4;
	u8 _reserved6[24];
	u32 rx_rx_eq_offset_adap_ctrl1;
	u32 rx_rx_offset_adap_ctrl2;
	u32 rx_sigdet_enables;
	u32 rx_sigdet_ctrl;
	u8 _reserved7[4];
	u32 rx_sigdet_deglitch_ctrl;
	u32 rx_rx_band;
	u8 _reserved8[80];
	u32 rx_rx_mode_00;
};
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fo_gain, 0x008);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_gain_half, 0x00c);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_fastlock_fo_gain, 0x030);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_so_saturtn_and_en, 0x034);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_ucdr_pi_cntrls, 0x044);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_vga_cal_ctrl2, 0x0c0);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adap_ctrl2, 0x0d4);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adap_ctrl3, 0x0d8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_equ_adap_ctrl4, 0x0dc);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_eq_offset_adap_ctrl1, 0x0f8);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_offset_adap_ctrl2, 0x0fc);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_enables, 0x100);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_ctrl, 0x104);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_sigdet_deglitch_ctrl, 0x10c);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_band, 0x110);
check_member(usb3_phy_qserdes_rx_reg_layout, rx_rx_mode_00, 0x164);

/* Only for QMP V3 PHY - PCS registers */
struct usb3_phy_pcs_reg_layout {
	u32 pcs_sw_reset;
	u32 pcs_power_down_control;
	u32 pcs_start_control;
	u32 pcs_txmgn_v0;
	u32 pcs_txmgn_v1;
	u32 pcs_txmgn_v2;
	u32 pcs_txmgn_v3;
	u32 pcs_txmgn_v4;
	u32 pcs_txmgn_ls;
	u32 pcs_txdeemph_m6db_v0;
	u32 pcs_txdeemph_m3p5db_v0;
	u32 pcs_txdeemph_m6db_v1;
	u32 pcs_txdeemph_m3p5db_v1;
	u32 pcs_txdeemph_m6db_v2;
	u32 pcs_txdeemph_m3p5db_v2;
	u32 pcs_txdeemph_m6db_v3;
	u32 pcs_txdeemph_m3p5db_v3;
	u32 pcs_txdeemph_m6db_v4;
	u32 pcs_txdeemph_m3p5db_v4;
	u32 pcs_txdeemph_m6db_ls;
	u32 pcs_txdeemph_m3p5db_ls;
	u8 _reserved1[8];
	u32 pcs_rate_slew_cntrl;
	u8 _reserved2[4];
	u32 pcs_power_state_config2;
	u8 _reserved3[8];
	u32 pcs_rcvr_dtct_dly_p1u2_l;
	u32 pcs_rcvr_dtct_dly_p1u2_h;
	u32 pcs_rcvr_dtct_dly_u3_l;
	u32 pcs_rcvr_dtct_dly_u3_h;
	u32 pcs_lock_detect_config1;
	u32 pcs_lock_detect_config2;
	u32 pcs_lock_detect_config3;
	u32 pcs_tsync_rsync_time;
	u8 _reserved4[16];
	u32 pcs_pwrup_reset_dly_time_auxclk;
	u8 _reserved5[12];
	u32 pcs_lfps_ecstart_eqtlock;
	u8 _reserved6[4];
	u32 pcs_rxeqtraining_wait_time;
	u32 pcs_rxeqtraining_run_time;
	u8 _reserved7[4];
	u32 pcs_fll_ctrl1;
	u32 pcs_fll_ctrl2;
	u32 pcs_fll_cnt_val_l;
	u32 pcs_fll_cnt_val_h_tol;
	u32 pcs_fll_man_code;
	u32 pcs_autonomous_mode_ctrl;
	u8 _reserved8[152];
	u32 pcs_ready_status;
	u8 _reserved9[96];
	u32 pcs_rx_sigdet_lvl;
	u8 _reserved10[48];
	u32 pcs_refgen_req_config1;
	u32 pcs_refgen_req_config2;
};
check_member(usb3_phy_pcs_reg_layout, pcs_sw_reset, 0x000);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_v0, 0x00c);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_v1, 0x010);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_v2, 0x014);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_v3, 0x018);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_v4, 0x01c);
check_member(usb3_phy_pcs_reg_layout, pcs_txmgn_ls, 0x020);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_v0, 0x024);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_v0, 0x028);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_v1, 0x02c);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_v1, 0x030);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_v2, 0x034);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_v2, 0x038);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_v3, 0x03c);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_v3, 0x040);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_v4, 0x044);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_v4, 0x048);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m6db_ls, 0x04c);
check_member(usb3_phy_pcs_reg_layout, pcs_txdeemph_m3p5db_ls, 0x050);
check_member(usb3_phy_pcs_reg_layout, pcs_rate_slew_cntrl, 0x05c);
check_member(usb3_phy_pcs_reg_layout, pcs_power_state_config2, 0x064);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_l, 0x070);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_p1u2_h, 0x074);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_u3_l, 0x078);
check_member(usb3_phy_pcs_reg_layout, pcs_rcvr_dtct_dly_u3_h, 0x07c);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config1, 0x080);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config2, 0x084);
check_member(usb3_phy_pcs_reg_layout, pcs_lock_detect_config3, 0x088);
check_member(usb3_phy_pcs_reg_layout, pcs_pwrup_reset_dly_time_auxclk, 0x0a0);
check_member(usb3_phy_pcs_reg_layout, pcs_rxeqtraining_wait_time, 0x0b8);
check_member(usb3_phy_pcs_reg_layout, pcs_fll_cnt_val_h_tol, 0x0d0);
check_member(usb3_phy_pcs_reg_layout, pcs_autonomous_mode_ctrl, 0x0d8);
check_member(usb3_phy_pcs_reg_layout, pcs_ready_status, 0x174);
check_member(usb3_phy_pcs_reg_layout, pcs_refgen_req_config2, 0x210);

static struct usb3_phy_qserdes_com_reg_layout *const qserdes_com_reg_layout =
	(void *)QMP_PHY_QSERDES_COM_REG_BASE;
static struct usb3_phy_qserdes_tx_reg_layout *const qserdes_tx_reg_layout =
	(void *)QMP_PHY_QSERDES_TX_REG_BASE;
static struct usb3_phy_qserdes_rx_reg_layout *const qserdes_rx_reg_layout =
	(void *)QMP_PHY_QSERDES_RX_REG_BASE;
static struct usb3_phy_pcs_reg_layout *const pcs_reg_layout =
	(void *)QMP_PHY_PCS_REG_BASE;


static const struct qmp_phy_init_tbl qmp_v3_usb3_serdes_tbl[] = {
	{&qserdes_com_reg_layout->com_pll_ivco, 0x07},
	{&qserdes_com_reg_layout->com_sysclk_en_sel, 0x14},
	{&qserdes_com_reg_layout->com_bias_en_clkbuflr_en, 0x08},
	{&qserdes_com_reg_layout->com_clk_select, 0x30},
	{&qserdes_com_reg_layout->com_sys_clk_ctrl, 0x02},
	{&qserdes_com_reg_layout->com_resetsm_ctrl2, 0x08},
	{&qserdes_com_reg_layout->com_cmn_config, 0x16},
	{&qserdes_com_reg_layout->com_svs_mode_clk_sel, 0x01},
	{&qserdes_com_reg_layout->com_hsclk_sel, 0x80},
	{&qserdes_com_reg_layout->com_dec_start_mode0, 0x82},
	{&qserdes_com_reg_layout->com_div_frac_start1_mode0, 0xab},
	{&qserdes_com_reg_layout->com_div_frac_start2_mode0, 0xea},
	{&qserdes_com_reg_layout->com_div_frac_start3_mode0, 0x02},
	{&qserdes_com_reg_layout->com_cp_ctrl_mode0, 0x06},
	{&qserdes_com_reg_layout->com_pll_rctrl_mode0, 0x16},
	{&qserdes_com_reg_layout->com_pll_cctrl_mode0, 0x36},
	{&qserdes_com_reg_layout->com_integloop_gain1_mode0, 0x00},
	{&qserdes_com_reg_layout->com_integloop_gain0_mode0, 0x3f},
	{&qserdes_com_reg_layout->com_vco_tune2_mode0, 0x01},
	{&qserdes_com_reg_layout->com_vco_tune1_mode0, 0xc9},
	{&qserdes_com_reg_layout->com_coreclk_div_mode0, 0x0a},
	{&qserdes_com_reg_layout->com_lock_cmp3_mode0, 0x00},
	{&qserdes_com_reg_layout->com_lock_cmp2_mode0, 0x34},
	{&qserdes_com_reg_layout->com_lock_cmp1_mode0, 0x15},
	{&qserdes_com_reg_layout->com_lock_cmp_en, 0x04},
	{&qserdes_com_reg_layout->com_core_clk_en, 0x00},
	{&qserdes_com_reg_layout->com_lock_cmp_cfg, 0x00},
	{&qserdes_com_reg_layout->com_vco_tune_map, 0x00},
	{&qserdes_com_reg_layout->com_sysclk_buf_enable, 0x0a},
	{&qserdes_com_reg_layout->com_ssc_en_center, 0x01},
	{&qserdes_com_reg_layout->com_ssc_per1, 0x31},
	{&qserdes_com_reg_layout->com_ssc_per2, 0x01},
	{&qserdes_com_reg_layout->com_ssc_adj_per1, 0x00},
	{&qserdes_com_reg_layout->com_ssc_adj_per2, 0x00},
	{&qserdes_com_reg_layout->com_ssc_step_size1, 0x85},
	{&qserdes_com_reg_layout->com_ssc_step_size2, 0x07},
};

static const struct qmp_phy_init_tbl qmp_v3_usb3_tx_tbl[] = {
	{&qserdes_tx_reg_layout->tx_highz_drvr_en, 0x10},
	{&qserdes_tx_reg_layout->tx_rcv_detect_lvl_2, 0x12},
	{&qserdes_tx_reg_layout->tx_lane_mode_1, 0x16},
	{&qserdes_tx_reg_layout->tx_res_code_lane_offset_rx, 0x09},
	{&qserdes_tx_reg_layout->tx_res_code_lane_offset_tx, 0x06},
};

static const struct qmp_phy_init_tbl qmp_v3_usb3_rx_tbl[] = {
	{&qserdes_rx_reg_layout->rx_ucdr_fastlock_fo_gain, 0x0b},
	{&qserdes_rx_reg_layout->rx_rx_equ_adap_ctrl2, 0x0f},
	{&qserdes_rx_reg_layout->rx_rx_equ_adap_ctrl3, 0x4e},
	{&qserdes_rx_reg_layout->rx_rx_equ_adap_ctrl4, 0x18},
	{&qserdes_rx_reg_layout->rx_rx_eq_offset_adap_ctrl1, 0x77},
	{&qserdes_rx_reg_layout->rx_rx_offset_adap_ctrl2, 0x80},
	{&qserdes_rx_reg_layout->rx_sigdet_ctrl, 0x03},
	{&qserdes_rx_reg_layout->rx_sigdet_deglitch_ctrl, 0x16},
	{&qserdes_rx_reg_layout->rx_ucdr_so_saturtn_and_en, 0x75},
	{&qserdes_rx_reg_layout->rx_ucdr_pi_cntrls, 0x80},
	{&qserdes_rx_reg_layout->rx_ucdr_fo_gain, 0x0a},
	{&qserdes_rx_reg_layout->rx_ucdr_so_gain_half, 0x06},
	{&qserdes_rx_reg_layout->rx_sigdet_enables, 0x00},
};

static const struct qmp_phy_init_tbl qmp_v3_usb3_pcs_tbl[] = {
	/* FLL settings */
	{&pcs_reg_layout->pcs_fll_ctrl2, 0x83},
	{&pcs_reg_layout->pcs_fll_cnt_val_l, 0x09},
	{&pcs_reg_layout->pcs_fll_cnt_val_h_tol, 0xa2},
	{&pcs_reg_layout->pcs_fll_man_code, 0x40},
	{&pcs_reg_layout->pcs_fll_ctrl1, 0x02},

	/* Lock Det settings */
	{&pcs_reg_layout->pcs_lock_detect_config1, 0xd1},
	{&pcs_reg_layout->pcs_lock_detect_config2, 0x1f},
	{&pcs_reg_layout->pcs_lock_detect_config3, 0x47},
	{&pcs_reg_layout->pcs_power_state_config2, 0x1b},

	{&pcs_reg_layout->pcs_rx_sigdet_lvl, 0xba},
	{&pcs_reg_layout->pcs_txmgn_v0, 0x9f},
	{&pcs_reg_layout->pcs_txmgn_v1, 0x9f},
	{&pcs_reg_layout->pcs_txmgn_v2, 0xb7},
	{&pcs_reg_layout->pcs_txmgn_v3, 0x4e},
	{&pcs_reg_layout->pcs_txmgn_v4, 0x65},
	{&pcs_reg_layout->pcs_txmgn_ls, 0x6b},
	{&pcs_reg_layout->pcs_txdeemph_m6db_v0, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_v0, 0x0d},
	{&pcs_reg_layout->pcs_txdeemph_m6db_v1, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_v1, 0x0d},
	{&pcs_reg_layout->pcs_txdeemph_m6db_v2, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_v2, 0x0d},
	{&pcs_reg_layout->pcs_txdeemph_m6db_v3, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_v3, 0x1d},
	{&pcs_reg_layout->pcs_txdeemph_m6db_v4, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_v4, 0x0d},
	{&pcs_reg_layout->pcs_txdeemph_m6db_ls, 0x15},
	{&pcs_reg_layout->pcs_txdeemph_m3p5db_ls, 0x0d},
	{&pcs_reg_layout->pcs_rate_slew_cntrl, 0x02},
	{&pcs_reg_layout->pcs_pwrup_reset_dly_time_auxclk, 0x04},
	{&pcs_reg_layout->pcs_tsync_rsync_time, 0x44},
	{&pcs_reg_layout->pcs_rcvr_dtct_dly_p1u2_l, 0xe7},
	{&pcs_reg_layout->pcs_rcvr_dtct_dly_p1u2_h, 0x03},
	{&pcs_reg_layout->pcs_rcvr_dtct_dly_u3_l, 0x40},
	{&pcs_reg_layout->pcs_rcvr_dtct_dly_u3_h, 0x00},
	{&pcs_reg_layout->pcs_rxeqtraining_wait_time, 0x75},
	{&pcs_reg_layout->pcs_lfps_ecstart_eqtlock, 0x86},
	{&pcs_reg_layout->pcs_rxeqtraining_run_time, 0x13},
};

struct ss_usb_phy_reg qmp_v3_usb_phy = {
	.serdes_tbl =		qmp_v3_usb3_serdes_tbl,
	.serdes_tbl_num	=	ARRAY_SIZE(qmp_v3_usb3_serdes_tbl),
	.tx_tbl =		qmp_v3_usb3_tx_tbl,
	.tx_tbl_num =		ARRAY_SIZE(qmp_v3_usb3_tx_tbl),
	.rx_tbl =		qmp_v3_usb3_rx_tbl,
	.rx_tbl_num =		ARRAY_SIZE(qmp_v3_usb3_rx_tbl),
	.pcs_tbl =		qmp_v3_usb3_pcs_tbl,
	.pcs_tbl_num =		ARRAY_SIZE(qmp_v3_usb3_pcs_tbl),
	.qmp_pcs_reg =		(void *)QMP_PHY_PCS_REG_BASE,
};

static void qcom_qmp_phy_configure(const struct qmp_phy_init_tbl tbl[],
				int num)
{
	int i;
	const struct qmp_phy_init_tbl *t = tbl;

	if (!t)
		return;

	for (i = 0; i < num; i++, t++)
		write32(t->address, t->val);
}

void ss_qmp_phy_init(void)
{
	struct ss_usb_phy_reg *ss_phy_reg;

	ss_phy_reg = &qmp_v3_usb_phy;
	/* power up USB3 PHY */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_power_down_control, 0x01);

	 /* Serdes configuration */
	qcom_qmp_phy_configure(ss_phy_reg->serdes_tbl,
		ss_phy_reg->serdes_tbl_num);
	/* Tx, Rx, and PCS configurations */
	qcom_qmp_phy_configure(ss_phy_reg->tx_tbl, ss_phy_reg->tx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->rx_tbl, ss_phy_reg->rx_tbl_num);
	qcom_qmp_phy_configure(ss_phy_reg->pcs_tbl, ss_phy_reg->pcs_tbl_num);

	/* perform software reset of PCS/Serdes */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_sw_reset, 0x00);
	/* start PCS/Serdes to operation mode */
	write32(&ss_phy_reg->qmp_pcs_reg->pcs_start_control, 0x03);

	/*
	 * Wait for PHY initialization to be done
	 * PCS_STATUS: wait for 1ms for PHY STATUS;
	 * SW can continuously check for PHYSTATUS = 1.b0.
	 */
	long lock_us = wait_us(10000,
			!(read32(&ss_phy_reg->qmp_pcs_reg->pcs_ready_status) &
			USB3_PCS_PHYSTATUS));
	if (!lock_us)
		printk(BIOS_ERR, "QMP PHY PLL LOCK fails:\n");
	else
		printk(BIOS_DEBUG, "QMP PHY initialized and locked in %ldus\n",
				lock_us);
}
