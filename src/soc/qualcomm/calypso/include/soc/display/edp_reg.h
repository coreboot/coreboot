/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_EDP_REG_H__
#define __SOC_QUALCOMM_CALYPSO_EDP_REG_H__

#include <types.h>
#include <soc/addressmap.h>

struct __packed edp_ahbclk_regs {   // 0xAF6C000
	uint32_t hw_version;        // 0x0000
	uint32_t reserved0[3];      // 0x0004, 0x0008, 0x000C
	uint32_t sw_reset;          // 0x0010
	uint32_t phy_ctrl;          // 0x0014
	uint32_t clk_ctrl;          // 0x0018
	uint32_t clk_active;        // 0x001C
	uint32_t interrupt_status;  // 0x0020
	uint32_t interrupt_status2; // 0x0024
	uint32_t interrupt_status3; // 0x0028
};
check_member(edp_ahbclk_regs, sw_reset, 0x10);

struct __packed edp_auxclk_regs {     // 0xAEA0200
	uint32_t hpd_ctrl;            // 0x0000
	uint32_t hpd_int_status;      // 0x0004
	uint32_t hpd_int_ack;         // 0x0008
	uint32_t hpd_int_mask;        // 0x000C
	uint32_t reserved0[2];        // 0x0010, 0x0014
	uint32_t hpd_reftimer;        // 0x0018
	uint32_t hpd_event_time0;     // 0x001C
	uint32_t hpd_event_time1;     // 0x0020
	uint32_t reserved1[3];        // 0x0024, 0x0028, 0x002C
	uint32_t aux_ctrl;            // 0x0030
	uint32_t aux_data;            // 0x0034
	uint32_t aux_trans_ctrl;      // 0x0038
	uint32_t timeout_count;       // 0x003C
	uint32_t aux_limits;          // 0x0040
	uint32_t status;              // 0x0044
	uint32_t reserved2[22];       // 0x0048 - 0x009C
	uint32_t interrupt_trans_num; // 0x00A0
};
check_member(edp_auxclk_regs, hpd_reftimer, 0x18);
check_member(edp_auxclk_regs, aux_ctrl, 0x30);
check_member(edp_auxclk_regs, interrupt_trans_num, 0xa0);

struct __packed edp_lclk_regs {                   // 0xAEA0400
	uint32_t mainlink_ctrl;                   // 0x0000
	uint32_t state_ctrl;                      // 0x0004
	uint32_t configuration_ctrl;              // 0x0008
	uint32_t top_bot_interlaced_num_of_lanes; // 0x000C
	uint32_t software_mvid;                   // 0x0010
	uint32_t reserved0;                       // 0x0014
	uint32_t software_nvid;                   // 0x0018
	uint32_t total_hor_ver;                   // 0x001C
	uint32_t start_hor_ver_from_sync;         // 0x0020
	uint32_t hysnc_vsync_width_polarity;      // 0x0024
	uint32_t active_hor_ver;                  // 0x0028
	uint32_t misc1_misc0;                     // 0x002C
	uint32_t valid_boundary;                  // 0x0030
	uint32_t valid_boundary2;                 // 0x0034
	uint32_t logcial2physical_lane_mapping;   // 0x0038
	uint32_t reserved1;                       // 0x003C
	uint32_t mainlink_ready;                  // 0x0040
	uint32_t mainlink_levels;                 // 0x0044
	uint32_t mainlink_levels2;                // 0x0048
	uint32_t tu;                              // 0x004C
	uint32_t db_ctrl;                         // 0x0050
	uint32_t compliance_scramble_rest;        // 0x0054
	uint32_t phy_bist_lane0_cfg;              // 0x0058

	uint32_t reserved2[72]; // 0x005C .. 0x0178

	uint32_t vsc_db16_db17_db18_pb8; // 0x017C (abs 0xAEA057C)
	uint32_t compression_mode_ctrl;  // 0x0180 (abs 0xAEA0580)
};
check_member(edp_lclk_regs, tu, 0x4C);

struct __packed edp_p0clk_regs {            // 0xAEA1000
	uint32_t bist_enable;               // 0x0000
	uint32_t reserved0[3];              // 0x0004, 0x0008, 0x000C
	uint32_t timing_engine_en;          // 0x0010
	uint32_t intf_config;               // 0x0014
	uint32_t hsync_ctl;                 // 0x0018
	uint32_t vsync_period_f0;           // 0x001C
	uint32_t vsync_period_f1;           // 0x0020
	uint32_t vsync_pulse_width_f0;      // 0x0024
	uint32_t vsync_pulse_width_f1;      // 0x0028
	uint32_t display_v_start_f0;        // 0x002C
	uint32_t display_v_start_f1;        // 0x0030
	uint32_t display_v_end_f0;          // 0x0034
	uint32_t display_v_end_f1;          // 0x0038
	uint32_t active_v_start_f0;         // 0x003C
	uint32_t active_v_start_f1;         // 0x0040
	uint32_t active_v_end_f0;           // 0x0044
	uint32_t active_v_end_f1;           // 0x0048
	uint32_t display_hctl;              // 0x004C
	uint32_t active_hctl;               // 0x0050
	uint32_t hsync_skew;                // 0x0054
	uint32_t polarity_ctl;              // 0x0058
	uint32_t reserved1;                 // 0x005C
	uint32_t tpg_main_control;          // 0x0060
	uint32_t tpg_video_config;          // 0x0064
	uint32_t tpg_component_limits;      // 0x0068
	uint32_t tpg_rectangle;             // 0x006C
	uint32_t tpg_initial_value;         // 0x0070
	uint32_t tpg_color_changing_frames; // 0x0074
	uint32_t tpg_rgb_mapping;           // 0x0078
	uint32_t dsc_dto;                   // 0x007C
	uint32_t rgb_mapping;               // 0x0080
	uint32_t dsc_dto_count;             // 0x0084
	uint32_t async_fifo_config;         // 0x0088
};
check_member(edp_p0clk_regs, dsc_dto, 0x7c);

struct __packed edp_phy_regs {            // 0xAEC5A00
	uint32_t revision_id0;            // 0x0000
	uint32_t revision_id1;            // 0x0004
	uint32_t revision_id2;            // 0x0008
	uint32_t revision_id3;            // 0x000C
	uint32_t cfg;                     // 0x0010
	uint32_t cfg1;                    // 0x0014
	uint32_t cfg2;                    // 0x0018  DP_PHY_CFG_2
	uint32_t pd_ctl;                  // 0x001C  DP_PHY_PD_CTL
	uint32_t mode;                    // 0x0020  DP_PHY_MODE
	uint32_t aux_cfg[13];             // 0x0024 - 0x0054  AUX_CFG0..12
	uint32_t aux_interrupt_mask;      // 0x0058
	uint32_t aux_interrupt_clr;       // 0x005C
	uint32_t aux_bist_cfg;            // 0x0060
	uint32_t aux_bist_prbs_seed;      // 0x0064
	uint32_t aux_bist_prbs_poly;      // 0x0068
	uint32_t aux_tx_prog_pat_16b_lsb; // 0x006C
	uint32_t aux_tx_prog_pat_16b_msb; // 0x0070
	uint32_t vco_div;                 // 0x0074
	uint32_t tsync_ovrd;              // 0x0078
	uint32_t tx0_tx1_lane_ctl;        // 0x007C
	uint32_t reserved_80[16];         // 0x0080 - 0x00BC  (PCS_BIST regs, not used)
	uint32_t tx2_tx3_lane_ctl;        // 0x00C0
	uint32_t misr_ctl;                // 0x00C4
	uint32_t debug_bus_sel;           // 0x00C8
	uint32_t auxless_cfg1;            // 0x00CC  DP_PHY_AUXLESS_CFG1
	uint32_t auxless_cfg2;            // 0x00D0  DP_PHY_AUXLESS_CFG2
	uint32_t lfps_period;             // 0x00D4  DP_PHY_LFPS_PERIOD
	uint32_t lfps_cyc;                // 0x00D8  DP_PHY_LFPS_CYC
	uint32_t auxless_setup_cyc;       // 0x00DC  DP_PHY_AUXLESS_SETUP_CYC
	uint32_t auxless_silence_cyc;     // 0x00E0  DP_PHY_AUXLESS_SILENCE_CYC
	uint32_t reserved_e4[3];          // 0x00E4 - 0x00EC  (LN0/LN1 DRV_LVL, not used)
	uint32_t ldo_cfg;                 // 0x00F0  DP_PHY_LDO_CFG
	uint32_t spare0;                  // 0x00F4  DP_PHY_SPARE0
	uint32_t reserved_f8[3];          // 0x00F8 - 0x0100  (SPARE1..3)
	uint32_t reserved_104[3];         // 0x0104 - 0x010C  (AUXLESS_STATUS, AUX_INT_STATUS)
	uint32_t status;                  // 0x0110  DP_PHY_STATUS
};

struct __packed edp_phy_lane_regs {             // 0xAEC5200
check_member(edp_phy_regs, cfg2, 0x18);
check_member(edp_phy_regs, pd_ctl, 0x1C);
check_member(edp_phy_regs, mode, 0x20);
check_member(edp_phy_regs, aux_cfg, 0x24);
check_member(edp_phy_regs, aux_interrupt_mask, 0x58);
check_member(edp_phy_regs, vco_div, 0x74);
check_member(edp_phy_regs, tx0_tx1_lane_ctl, 0x7C);
check_member(edp_phy_regs, tx2_tx3_lane_ctl, 0xC0);
check_member(edp_phy_regs, auxless_cfg1, 0xCC);
check_member(edp_phy_regs, auxless_setup_cyc, 0xDC);
check_member(edp_phy_regs, auxless_silence_cyc, 0xE0);
check_member(edp_phy_regs, ldo_cfg, 0xF0);
check_member(edp_phy_regs, spare0, 0xF4);
check_member(edp_phy_regs, status, 0x110);
	uint32_t tx_clk_buf_enable;             // 0x00
	uint32_t tx_emp_post1_lvl;              // 0x04
	uint32_t tx_post2_emph;                 // 0x08
	uint32_t tx_boost_lvl_up_dn;            // 0x0C
	uint32_t tx_idle_lvl_large_amp;         // 0x10
	uint32_t tx_drv_lvl;                    // 0x14
	uint32_t tx_drv_lvl_offset;             // 0x18
	uint32_t tx_reset_tsync_en;             // 0x1C
	uint32_t tx_pre_emph;                   // 0x20
	uint32_t tx_interface_select;           // 0x24
	uint32_t tx_tx_band;                    // 0x28
	uint32_t tx_slew_cntl;                  // 0x2C
	uint32_t tx_lpb0_cfg[3];                // 0x30, 0x34, 0x38
	uint32_t tx_rescode_lane_tx;            // 0x3C
	uint32_t tx_rescode_lane_tx1;           // 0x40
	uint32_t tx_rescode_lane_offset_tx0;    // 0x44
	uint32_t tx_rescode_lane_offset_tx1;    // 0x48
	uint32_t tx_serdes_byp_en_out;          // 0x4C
	uint32_t tx_dbg_bus_sel;                // 0x50
	uint32_t tx_transceiver_bias_en;        // 0x54
	uint32_t tx_highz_drvr_en;              // 0x58
	uint32_t tx_tx_pol_inv;                 // 0x5C
	uint32_t tx_parrate_rec_detect_idle_en; // 0x60
	uint32_t tx_lane_mode1;                 // 0x64
	uint32_t tx_lane_mode2;                 // 0x68
	uint32_t tx_atb_sel1;                   // 0x6C
	uint32_t tx_atb_sel2;                   // 0x70
	uint32_t tx_reset_gen_muxes;            // 0x74
	uint32_t tx_tran_drvr_emp_en;           // 0x78
	uint32_t tx_vmode_ctrl1;                // 0x7C
	uint32_t tx_lane_dig_config;            // 0x80
	uint32_t tx_ldo_config;                 // 0x84
	uint32_t tx_dig_bkup_ctrl;              // 0x88
	uint32_t reserved0[15];                 // 0x8C - 0xC7
	uint32_t tx_dcc0_ctrl;                  // 0xC8  TXn_DCC0_CTRL
	uint32_t reserved1[1];                  // 0xCC
	uint32_t tx_dcc1_ctrl;                  // 0xD0  TXn_DCC1_CTRL
	uint32_t reserved2[3];                  // 0xD4 - 0xDC
	uint32_t tx_dcc_done;                   // 0xE0  TXn_DCC_DONE
};
check_member(edp_phy_lane_regs, tx_dig_bkup_ctrl, 0x88);
check_member(edp_phy_lane_regs, tx_dcc0_ctrl, 0xC8);
check_member(edp_phy_lane_regs, tx_dcc1_ctrl, 0xD0);
check_member(edp_phy_lane_regs, tx_dcc_done, 0xE0);

struct __packed edp_phy_pll_regs {                       // 0xAEC5000
	uint32_t qserdes_com_ssc_step_size1_mode1;       // 0x00
	uint32_t qserdes_com_ssc_step_size2_mode1;       // 0x04
	uint32_t qserdes_com_ssc_step_size3_mode1;       // 0x08
	uint32_t qserdes_com_clk_ep_div_mode1;           // 0x0C
	uint32_t qserdes_com_cp_ctrl_mode1;              // 0x10
	uint32_t qserdes_com_pll_rctrl_mode1;            // 0x14
	uint32_t qserdes_com_pll_cctrl_mode1;            // 0x18
	uint32_t qserdes_com_coreclk_div_mode1;          // 0x1C
	uint32_t qserdes_com_lock_cmp1_mode1;            // 0x20
	uint32_t qserdes_com_lock_cmp2_mode1;            // 0x24
	uint32_t qserdes_com_dec_start_mode1;            // 0x28
	uint32_t qserdes_com_dec_start_msb_mode1;        // 0x2C
	uint32_t qserdes_com_div_frac_start1_mode1;      // 0x30
	uint32_t qserdes_com_div_frac_start2_mode1;      // 0x34
	uint32_t qserdes_com_div_frac_start3_mode1;      // 0x38
	uint32_t qserdes_com_hsclk_sel;                  // 0x3C
	uint32_t qserdes_com_integloop_gain0_mode1;      // 0x40
	uint32_t qserdes_com_integloop_gain1_mode1;      // 0x44
	uint32_t qserdes_com_vco_tune1_mode1;            // 0x48
	uint32_t qserdes_com_vco_tune2_mode1;            // 0x4C
	uint32_t qserdes_com_bin_vcocal_cmp_code1_mode1; // 0x50
	uint32_t qserdes_com_bin_vcocal_cmp_code2_mode1; // 0x54
	uint32_t qserdes_com_bin_vcocal_cmp_code1_mode0; // 0x58
	uint32_t qserdes_com_bin_vcocal_cmp_code2_mode0; // 0x5C
	uint32_t qserdes_com_ssc_step_size1_mode0;       // 0x60
	uint32_t qserdes_com_ssc_step_size2_mode0;       // 0x64
	uint32_t qserdes_com_ssc_step_size3_mode0;       // 0x68
	uint32_t qserdes_com_clk_ep_div_mode0;           // 0x6C
	uint32_t qserdes_com_cp_ctrl_mode0;              // 0x70
	uint32_t qserdes_com_pll_rctrl_mode0;            // 0x74
	uint32_t qserdes_com_pll_cctrl_mode0;            // 0x78
	uint32_t qserdes_com_coreclk_div_mode0;          // 0x7C
	uint32_t qserdes_com_lock_cmp1_mode0;            // 0x80
	uint32_t qserdes_com_lock_cmp2_mode0;            // 0x84
	uint32_t qserdes_com_dec_start_mode0;            // 0x88
	uint32_t qserdes_com_dec_start_msb_mode0;        // 0x8C
	uint32_t qserdes_com_div_frac_start1_mode0;      // 0x90
	uint32_t qserdes_com_div_frac_start2_mode0;      // 0x94
	uint32_t qserdes_com_div_frac_start3_mode0;      // 0x98
	uint32_t qserdes_com_hsclk_hs_switch_sel;        // 0x9C
	uint32_t qserdes_com_integloop_gain0_mode0;      // 0xA0
	uint32_t qserdes_com_integloop_gain1_mode0;      // 0xA4
	uint32_t qserdes_com_vco_tune1_mode0;            // 0xA8
	uint32_t qserdes_com_vco_tune2_mode0;            // 0xAC
	uint32_t qserdes_com_atb_sel1;                   // 0xB0
	uint32_t qserdes_com_atb_sel2;                   // 0xB4
	uint32_t qserdes_com_freq_update;                // 0xB8
	uint32_t qserdes_com_bg_timer;                   // 0xBC
	uint32_t qserdes_com_ssc_en_center;              // 0xC0
	uint32_t qserdes_com_ssc_adj_per1;               // 0xC4
	uint32_t qserdes_com_ssc_adj_per2;               // 0xC8
	uint32_t qserdes_com_ssc_per1;                   // 0xCC
	uint32_t qserdes_com_ssc_per2;                   // 0xD0
	uint32_t qserdes_com_post_div;                   // 0xD4
	uint32_t qserdes_com_post_div_mux;               // 0xD8
	uint32_t qserdes_com_bias_en_clkbuflr_en;        // 0xDC
	uint32_t qserdes_com_clk_enable1;                // 0xE0
	uint32_t qserdes_com_sys_clk_ctrl;               // 0xE4
	uint32_t qserdes_com_sysclk_buf_enable;          // 0xE8
	uint32_t qserdes_com_pll_en;                     // 0xEC
	uint32_t qserdes_com_debug_bus_ovrd;             // 0xF0
	uint32_t qserdes_com_pll_ivco;                   // 0xF4
	uint32_t qserdes_com_pll_ivco_mode1;             // 0xF0
	uint32_t qserdes_com_cmn_iterim;                 // 0xFC
	uint32_t qserdes_com_cmn_iptrim;                 // 0x100
	uint32_t qserdes_com_ep_clk_detect_ctrl;         // 0x104
	uint32_t qserdes_com_pll_cntrl;                  // 0x108
	uint32_t qserdes_com_bias_en_ctrl_by_psm;        // 0x10C
	uint32_t qserdes_com_sysclk_en_sel;              // 0x110
	uint32_t qserdes_com_cml_sysclk_sel;             // 0x114
	uint32_t qserdes_com_resetsm_cntrl;              // 0x118
	uint32_t qserdes_com_resetsm_cntrl2;             // 0x11C
	uint32_t qserdes_com_lock_cmp_en;                // 0x120
	uint32_t qserdes_com_lock_cmp_cfg;               // 0x124
	uint32_t qserdes_com_integloop_initval;          // 0x128
	uint32_t qserdes_com_integloop_en;               // 0x12C
	uint32_t qserdes_com_integloop_p_path_gain0;     // 0x130
	uint32_t qserdes_com_integloop_p_path_gain1;     // 0x134
	uint32_t qserdes_com_vcoval_deadman_ctrl;        // 0x138
	uint32_t qserdes_com_vco_tune_ctrl;              // 0x13C
	uint32_t qserdes_com_vco_tune_map;               // 0x140
	uint32_t qserdes_com_vco_tune_initval1;          // 0x144
	uint32_t qserdes_com_vco_tune_initval2;          // 0x148
	uint32_t qserdes_com_vco_tune_minval1;           // 0x14C
	uint32_t qserdes_com_vco_tune_minval2;           // 0x150
	uint32_t qserdes_com_vco_tune_maxval1;           // 0x154
	uint32_t qserdes_com_vco_tune_maxval2;           // 0x158
	uint32_t qserdes_com_vco_tune_timer1;            // 0x15C
	uint32_t qserdes_com_vco_tune_timer2;            // 0x160
	uint32_t qserdes_com_clk_sel;                    // 0x164
	uint32_t qserdes_com_pll_analog;                 // 0x168
	uint32_t qserdes_com_sw_reset;                   // 0x16C
	uint32_t qserdes_com_core_clk_en;                // 0x170
	uint32_t qserdes_com_cmn_config_1;                // 0x174

	/*
	 * 3nm (EDP2/Glymur) relocates several registers vs. 4nm; the gap
	 * below (0x178-0x2F0) has no vendor confirmation for calypso and is
	 * never written by the PLL config sequence, so it is left as
	 * reserved padding rather than carrying forward 4nm's guessed field
	 * names/offsets.
	 */
	uint32_t reserved_178[2];                         // 0x178 - 0x17C
	uint32_t qserdes_com_svs_mode_clk_sel;            // 0x180
	uint32_t reserved_184[4];                         // 0x184 - 0x190
	uint32_t qserdes_com_cmn_mode_contd1;             // 0x194
	uint32_t reserved_198[63];                        // 0x198 - 0x290
	uint32_t qserdes_com_cmn_mode_contd3;             // 0x294
	uint32_t reserved_298[16];                        // 0x298 - 0x2D4
	uint32_t qserdes_com_dcc_cal_1;                   // 0x2D8
	uint32_t reserved_2dc;                            // 0x2DC
	uint32_t qserdes_com_dcc_cal_3;                   // 0x2E0
	uint32_t reserved_2e4[3];                         // 0x2E4 - 0x2EC
	uint32_t qserdes_com_psm_cal_en;                  // 0x2F0
	uint32_t qserdes_com_clk_fwd_config_1;            // 0x2F4
	uint32_t reserved_2f8;                            // 0x2F8
	uint32_t qserdes_com_ip_ctrl_and_dp_sel;          // 0x2FC
	uint32_t reserved_300[5];                         // 0x300 - 0x310
	uint32_t qserdes_com_cmn_status;                  // 0x314
	uint32_t reserved_318[9];                         // 0x318 - 0x338
	uint32_t qserdes_com_c_ready_status;              // 0x33C
};
check_member(edp_phy_pll_regs, qserdes_com_bias_en_clkbuflr_en, 0xdc);
check_member(edp_phy_pll_regs, qserdes_com_svs_mode_clk_sel, 0x180);
check_member(edp_phy_pll_regs, qserdes_com_cmn_mode_contd1, 0x194);
check_member(edp_phy_pll_regs, qserdes_com_cmn_mode_contd3, 0x294);
check_member(edp_phy_pll_regs, qserdes_com_dcc_cal_1, 0x2D8);
check_member(edp_phy_pll_regs, qserdes_com_dcc_cal_3, 0x2E0);
check_member(edp_phy_pll_regs, qserdes_com_psm_cal_en, 0x2F0);
check_member(edp_phy_pll_regs, qserdes_com_clk_fwd_config_1, 0x2F4);
check_member(edp_phy_pll_regs, qserdes_com_ip_ctrl_and_dp_sel, 0x2FC);
check_member(edp_phy_pll_regs, qserdes_com_cmn_status, 0x314);
check_member(edp_phy_pll_regs, qserdes_com_c_ready_status, 0x33C);

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
	eDP_SAFE_EXIT_LVL_LANE1 = 14,
	eDP_SAFE_EXIT_LVL_LANE2 = 8,
	eDP_SAFE_EXIT_LVL_LANE4 = 5,
};

enum {
	COLOR_DEPTH_6BPC = 6,
	COLOR_DEPTH_8BPC = 8,
	COLOR_DEPTH_10BPC = 10,
};

enum {
	EDP_AHBCLK_BASE = EDP_CTRL_BASE,			//0x0AF6C000
	EDP_AUXCLK_BASE = EDP_CTRL_BASE + 0x200,		//0x0AF6C200
	EDP_LCLK_BASE = 	0xAF6D000,			//0x0AF6D000    EDP_CTRL_BASE + 0x400
	EDP_P0CLK_BASE = 	0xAF6E000, 			//EDP_CTRL_BASE + 0x1000,
	EDP_PHY_LANE_TX0_BASE = 0xFAA400,		        //0xAEC5200
	EDP_PHY_LANE_TX1_BASE = 0xFAA800, 		        //0xAEC5600
	EDP_PHY_BASE = 			0xFAAC00,		//0xAEC5A00,
	EDP_PHY_PLL_BASE =  	0xFAA000, 			//0x00FAA000
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
