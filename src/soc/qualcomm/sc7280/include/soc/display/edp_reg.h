/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EDP_REG_H_
#define _EDP_REG_H_

#include <types.h>

struct edp_ahbclk_regs {
	uint32_t hw_version;
	uint32_t reserved0[3];
	uint32_t sw_reset;
	uint32_t phy_ctrl;
	uint32_t clk_ctrl;
	uint32_t clk_active;
	uint32_t interrupt_status;
	uint32_t interrupt_status2;
	uint32_t interrupt_status3;
};

check_member(edp_ahbclk_regs, sw_reset, 0x10);

struct edp_auxclk_regs {
	uint32_t hpd_ctrl;
	uint32_t hpd_int_status;
	uint32_t hpd_int_ack;
	uint32_t hpd_int_mask;
	uint32_t reserved0[2];
	uint32_t hpd_reftimer;
	uint32_t hpd_event_time0;
	uint32_t hpd_event_time1;
	uint32_t reserved1[3];
	uint32_t aux_ctrl;
	uint32_t aux_data;
	uint32_t aux_trans_ctrl;
	uint32_t timeout_count;
	uint32_t aux_limits;
	uint32_t status;
	uint32_t reserved2[22];
	uint32_t interrupt_trans_num;
};
check_member(edp_auxclk_regs, hpd_reftimer, 0x18);
check_member(edp_auxclk_regs, aux_ctrl, 0x30);
check_member(edp_auxclk_regs, interrupt_trans_num, 0xa0);

struct edp_lclk_regs {
	uint32_t mainlink_ctrl;
	uint32_t state_ctrl;
	uint32_t configuration_ctrl;
	uint32_t top_bot_interlaced_num_of_lanes;
	uint32_t software_mvid;
	uint32_t reserved0;
	uint32_t software_nvid;
	uint32_t total_hor_ver;
	uint32_t start_hor_ver_from_sync;
	uint32_t hysnc_vsync_width_polarity;
	uint32_t active_hor_ver;
	uint32_t misc1_misc0;
	uint32_t valid_boundary;
	uint32_t valid_boundary2;
	uint32_t logcial2physical_lane_mapping;
	uint32_t reserved1;
	uint32_t mainlink_ready;
	uint32_t mainlink_levels;
	uint32_t mainlink_levels2;
	uint32_t tu;
};

struct edp_p0clk_regs {
	uint32_t bist_enable;
	uint32_t reserved0[3];
	uint32_t timing_engine_en;
	uint32_t intf_config;
	uint32_t hsync_ctl;
	uint32_t vsync_period_f0;
	uint32_t vsync_period_f1;
	uint32_t vsync_pulse_width_f0;
	uint32_t vsync_pulse_width_f1;
	uint32_t display_v_start_f0;
	uint32_t display_v_start_f1;
	uint32_t display_v_end_f0;
	uint32_t display_v_end_f1;
	uint32_t active_v_start_f0;
	uint32_t active_v_start_f1;
	uint32_t active_v_end_f0;
	uint32_t active_v_end_f1;
	uint32_t display_hctl;
	uint32_t active_hctl;
	uint32_t hsync_skew;
	uint32_t polarity_ctl;
	uint32_t reserved1;
	uint32_t tpg_main_control;
	uint32_t tpg_video_config;
	uint32_t tpg_component_limits;
	uint32_t tpg_rectangle;
	uint32_t tpg_initial_value;
	uint32_t tpg_color_changing_frames;
	uint32_t tpg_rgb_mapping;
	uint32_t dsc_dto;
};

check_member(edp_p0clk_regs, dsc_dto, 0x7c);

struct edp_phy_regs {
	uint32_t revision_id0;
	uint32_t revision_id1;
	uint32_t revision_id2;
	uint32_t revision_id3;
	uint32_t cfg;
	uint32_t cfg1;
	uint32_t cfg2;
	uint32_t pd_ctl;
	uint32_t mode;
	uint32_t aux_cfg[13];
	uint32_t aux_interrupt_mask;
	uint32_t aux_interrupt_clr;
	uint32_t aux_bist_cfg;
	uint32_t aux_bist_prbs_seed;
	uint32_t aux_bist_prbs_poly;
	uint32_t aux_tx_prog_pat_16b_lsb;
	uint32_t aux_tx_prog_pat_16b_msb;
	uint32_t vco_div;
	uint32_t tsync_ovrd;
	uint32_t tx0_tx1_lane_ctl;
	uint32_t tx0_tx1_bist_cfg[4];
	uint32_t tx0_tx1_prbs_seed_byte0;
	uint32_t tx0_tx1_prbs_seed_byte1;
	uint32_t tx0_tx1_bist_pattern0;
	uint32_t tx0_tx1_bist_pattern1;
	uint32_t tx2_tx3_lane_ctl;
	uint32_t tx2_tx3_bist_cfg[4];
	uint32_t tx2_tx3_prbs_seed_byte0;
	uint32_t tx2_tx3_prbs_seed_byte1;
	uint32_t tx2_tx3_bist_pattern0;
	uint32_t tx2_tx3_bist_pattern1;
	uint32_t misr_ctl;
	uint32_t debug_bus_sel;
	uint32_t spare[4];
	uint32_t aux_interrupt_status;
	uint32_t status;
};

struct edp_phy_lane_regs {
	uint32_t tx_clk_buf_enable;
	uint32_t tx_emp_post1_lvl;
	uint32_t tx_post2_emph;
	uint32_t tx_boost_lvl_up_dn;
	uint32_t tx_idle_lvl_large_amp;
	uint32_t tx_drv_lvl;
	uint32_t tx_drv_lvl_offset;
	uint32_t tx_reset_tsync_en;
	uint32_t tx_pre_emph;
	uint32_t tx_interface_select;
	uint32_t tx_tx_band;
	uint32_t tx_slew_cntl;
	uint32_t tx_lpb0_cfg[3];
	uint32_t tx_rescode_lane_tx;
	uint32_t tx_rescode_lane_tx1;
	uint32_t tx_rescode_lane_offset_tx0;
	uint32_t tx_rescode_lane_offset_tx1;
	uint32_t tx_serdes_byp_en_out;
	uint32_t tx_dbg_bus_sel;
	uint32_t tx_transceiver_bias_en;
	uint32_t tx_highz_drvr_en;
	uint32_t tx_tx_pol_inv;
	uint32_t tx_parrate_rec_detect_idle_en;
	uint32_t tx_lane_mode1;
	uint32_t tx_lane_mode2;
	uint32_t tx_atb_sel1;
	uint32_t tx_atb_sel2;
	uint32_t tx_reset_gen_muxes;
	uint32_t tx_tran_drvr_emp_en;
	uint32_t tx_vmode_ctrl1;
	uint32_t tx_lane_dig_config;
	uint32_t tx_ldo_config;
	uint32_t tx_dig_bkup_ctrl;
};

struct edp_phy_pll_regs {
	uint32_t qserdes_com_atb_sel1;
	uint32_t qserdes_com_atb_sel2;
	uint32_t qserdes_com_freq_update;
	uint32_t qserdes_com_bg_timer;
	uint32_t qserdes_com_ssc_en_center;
	uint32_t qserdes_com_ssc_adj_per1;
	uint32_t qserdes_com_ssc_adj_per2;
	uint32_t qserdes_com_ssc_per1;
	uint32_t qserdes_com_ssc_per2;
	uint32_t qserdes_com_ssc_step_size1_mode0;
	uint32_t qserdes_com_ssc_step_size2_mode0;
	uint32_t qserdes_com_ssc_step_size3_mode0;
	uint32_t qserdes_com_ssc_step_size1_mode1;
	uint32_t qserdes_com_ssc_step_size2_mode1;
	uint32_t qserdes_com_ssc_step_size3_mode1;
	uint32_t qserdes_com_post_div;
	uint32_t qserdes_com_post_div_mux;
	uint32_t qserdes_com_bias_en_clkbuflr_en;
	uint32_t qserdes_com_clk_enable1;
	uint32_t qserdes_com_sys_clk_ctrl;
	uint32_t qserdes_com_sysclk_buf_enable;
	uint32_t qserdes_com_pll_en;
	uint32_t qserdes_com_pll_ivco;
	uint32_t qserdes_com_cmn_iterim;
	uint32_t qserdes_com_cmn_iptrim;
	uint32_t qserdes_com_ep_clk_detect_ctrl;
	uint32_t qserdes_com_sysclk_det_comp_status;
	uint32_t qserdes_com_clk_ep_div_mode0;
	uint32_t qserdes_com_clk_ep_div_mode1;
	uint32_t qserdes_com_cp_ctrl_mode0;
	uint32_t qserdes_com_cp_ctrl_mode1;
	uint32_t qserdes_com_pll_rctrl_mode0;
	uint32_t qserdes_com_pll_rctrl_mode1;
	uint32_t qserdes_com_pll_cctrl_mode0;
	uint32_t qserdes_com_pll_cctrl_mode1;
	uint32_t qserdes_com_pll_cntrl;
	uint32_t qserdes_com_bias_en_ctrl_by_psm;
	uint32_t qserdes_com_sysclk_en_sel;
	uint32_t qserdes_com_cml_sysclk_sel;
	uint32_t qserdes_com_resetsm_cntrl;
	uint32_t qserdes_com_resetsm_cntrl2;
	uint32_t qserdes_com_lock_cmp_en;
	uint32_t qserdes_com_lock_cmp_cfg;
	uint32_t qserdes_com_lock_cmp1_mode0;
	uint32_t qserdes_com_lock_cmp2_mode0;
	uint32_t qserdes_com_lock_cmp1_mode1;
	uint32_t qserdes_com_lock_cmp2_mode1;
	uint32_t qserdes_com_dec_start_mode0;
	uint32_t qserdes_com_dec_start_msb_mode0;
	uint32_t qserdes_com_dec_start_mode1;
	uint32_t qserdes_com_dec_start_msb_mode1;
	uint32_t qserdes_com_div_frac_start1_mode0;
	uint32_t qserdes_com_div_frac_start2_mode0;
	uint32_t qserdes_com_div_frac_start3_mode0;
	uint32_t qserdes_com_div_frac_start1_mode1;
	uint32_t qserdes_com_div_frac_start2_mode1;
	uint32_t qserdes_com_div_frac_start3_mode1;
	uint32_t qserdes_com_integloop_initval;
	uint32_t qserdes_com_integloop_en;
	uint32_t qserdes_com_integloop_gain0_mode0;
	uint32_t qserdes_com_integloop_gain1_mode0;
	uint32_t qserdes_com_integloop_gain0_mode1;
	uint32_t qserdes_com_integloop_gain1_mode1;
	uint32_t qserdes_com_integloop_p_path_gain0;
	uint32_t qserdes_com_integloop_p_path_gain1;
	uint32_t qserdes_com_vcoval_deadman_ctrl;
	uint32_t qserdes_com_vco_tune_ctrl;
	uint32_t qserdes_com_vco_tune_map;
	uint32_t qserdes_com_vco_tune1_mode0;
	uint32_t qserdes_com_vco_tune2_mode0;
	uint32_t qserdes_com_vco_tune1_mode1;
	uint32_t qserdes_com_vco_tune2_mode1;
	uint32_t qserdes_com_vco_tune_initval1;
	uint32_t qserdes_com_vco_tune_initval2;
	uint32_t qserdes_com_vco_tune_minval1;
	uint32_t qserdes_com_vco_tune_minval2;
	uint32_t qserdes_com_vco_tune_maxval1;
	uint32_t qserdes_com_vco_tune_maxval2;
	uint32_t qserdes_com_vco_tune_timer1;
	uint32_t qserdes_com_vco_tune_timer2;
	uint32_t qserdes_com_cmn_status;
	uint32_t qserdes_com_reset_sm_status;
	uint32_t qserdes_com_restrim_code_status;
	uint32_t qserdes_com_pllcal_code1_status;
	uint32_t qserdes_com_pllcal_code2_status;
	uint32_t qserdes_com_clk_sel;
	uint32_t qserdes_com_hsclk_sel;
	uint32_t qserdes_com_hsclk_hs_switch_sel;
	uint32_t qserdes_com_integloop_bincode_status;
	uint32_t qserdes_com_pll_analog;
	uint32_t qserdes_com_coreclk_div_mode0;
	uint32_t qserdes_com_coreclk_div_mode1;
	uint32_t qserdes_com_sw_reset;
	uint32_t qserdes_com_core_clk_en;
	uint32_t qserdes_com_c_ready_status;
	uint32_t qserdes_com_cmn_config;
	uint32_t qserdes_com_cmn_rate_override;
	uint32_t qserdes_com_svs_mode_clk_sel;
};

/* EDP_STATE_CTRL */
enum {
	SW_LINK_TRAINING_PATTERN1 = BIT(0),
	SW_LINK_TRAINING_PATTERN2 = BIT(1),
	SW_LINK_TRAINING_PATTERN3 = BIT(2),
	SW_LINK_TRAINING_PATTERN4 = BIT(3),
	SW_LINK_SYMBOL_ERROR_RATE_MEASUREMENT = BIT(4),
	SW_LINK_PRBS7 = BIT(5),
	SW_LINK_TEST_CUSTOM_80BIT_PATTERN = BIT(6),
	SW_SEND_VIDEO = BIT(7),
	SW_PUSH_IDLE = BIT(8),
};

/* EDP_PHY_AUX_INTERRUPT_CLEAR */
enum {
	RX_STOP_ERR = BIT(0),
	RX_DEC_ERR = BIT(1),
	RX_SYNC_ERR = BIT(2),
	RX_ALIGN_ERR = BIT(3),
	TX_REQ_ERR = BIT(4),
	GLOBE_REQ_CLR = BIT(5),
};

enum {
	EDP_CTRL_BASE = 0xAEA0000,
	DP_EDP_PHY_BASE = 0xAEC0000,
};

enum {
	EDP_AHBCLK_BASE = EDP_CTRL_BASE,
	EDP_AUXCLK_BASE = EDP_CTRL_BASE + 0x200,
	EDP_LCLK_BASE = EDP_CTRL_BASE + 0x400,
	EDP_P0CLK_BASE = EDP_CTRL_BASE + 0x1000,
	EDP_PHY_BASE = DP_EDP_PHY_BASE + 0x2A00,
	EDP_PHY_LANE_TX0_BASE = DP_EDP_PHY_BASE + 0x2200,
	EDP_PHY_LANE_TX1_BASE = DP_EDP_PHY_BASE + 0x2600,
	EDP_PHY_PLL_BASE = DP_EDP_PHY_BASE + 0x2000,
};

static struct edp_ahbclk_regs *const edp_ahbclk = (void *)EDP_AHBCLK_BASE;
static struct edp_auxclk_regs *const edp_auxclk = (void *)EDP_AUXCLK_BASE;
static struct edp_lclk_regs *const edp_lclk = (void *)EDP_LCLK_BASE;
static struct edp_p0clk_regs *const edp_p0clk = (void *)EDP_P0CLK_BASE;
static struct edp_phy_regs *const edp_phy = (void *)EDP_PHY_BASE;
static struct edp_phy_lane_regs *const edp_phy_lane_tx0 = (void *)EDP_PHY_LANE_TX0_BASE;
static struct edp_phy_lane_regs *const edp_phy_lane_tx1 = (void *)EDP_PHY_LANE_TX1_BASE;
static struct edp_phy_pll_regs *const edp_phy_pll = (void *)EDP_PHY_PLL_BASE;

#endif
