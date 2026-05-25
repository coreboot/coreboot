// SPDX-License-Identifier: GPL-2.0-only
#ifndef _SOC_DISPLAY_MDSS_REG_H_
#define _SOC_DISPLAY_MDSS_REG_H_

#include <types.h>
#include <edid.h>
#include <soc/addressmap.h>

#define INTF_FLUSH INTF_FLUSH_5

#define MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH 2560U
#define MDSS_MAX_MDP_CLK 575000000U
#define MDSS_MDP_CLK_FUDGE_FACTOR_NUMER 105U
#define MDSS_MDP_CLK_FUDGE_FACTOR_DENOM 100U

#ifndef WRITE32_LOG
#define WRITE32_LOG(addr, val)                                                               \
	do {                                                                                 \
		uintptr_t __addr_tmp = (uintptr_t)(addr);                                    \
		uint32_t __val_tmp = (uint32_t)(val);                                        \
		write32((void *)__addr_tmp, __val_tmp);                                      \
		printk(BIOS_DEBUG, "[%s:%d] write32 addr=%p val=0x%X\n", __func__, __LINE__, \
		       (void *)__addr_tmp, (unsigned int)__val_tmp);                         \
	} while (0)
#endif

struct dsi_regs {
	uint32_t hw_version;
	uint32_t ctrl;
	uint32_t reserved0[2];
	uint32_t video_mode_ctrl;
	uint32_t reserved1[4];
	uint32_t video_mode_active_h;
	uint32_t video_mode_active_v;
	uint32_t video_mode_active_total;
	uint32_t video_mode_active_hsync;
	uint32_t video_mode_active_vsync;
	uint32_t video_mode_active_vsync_vpos;
	uint32_t cmd_mode_dma_ctrl;
	uint32_t cmd_mode_mdp_ctrl;
	uint32_t cmd_mode_mdp_dcs_cmd_ctrl;
	uint32_t dma_cmd_offset;
	uint32_t dma_cmd_length;
	uint32_t reserved2[2];
	uint32_t cmd_mode_mdp_stream0_ctrl;
	uint32_t cmd_mode_mdp_stream0_total;
	uint32_t cmd_mode_mdp_stream1_ctrl;
	uint32_t cmd_mode_mdp_stream1_total;
	uint32_t reserved4[7];
	uint32_t trig_ctrl;
	uint32_t reserved5[2];
	uint32_t cmd_mode_dma_sw_trigger;
	uint32_t reserved6[3];
	uint32_t misr_cmd_ctrl;
	uint32_t misr_video_ctrl;
	uint32_t lane_status;
	uint32_t lane_ctrl;
	uint32_t reserved7[3];
	uint32_t hs_timer_ctrl;
	uint32_t timeout_status;
	uint32_t clkout_timing_ctrl;
	uint32_t eot_packet;
	uint32_t eot_packet_ctrl;
	uint32_t reserved8[15];
	uint32_t err_int_mask0;
	uint32_t int_ctrl;
	uint32_t iobist_ctrl;
	uint32_t soft_reset;
	uint32_t clk_ctrl;
	uint32_t reserved9[15];
	uint32_t test_pattern_gen_ctrl;
	uint32_t reserved10[7];
	uint32_t test_pattern_gen_cmd_dma_init_val;
	uint32_t reserved11[14];
	uint32_t cmd_mode_mdp_ctrl2;
	uint32_t reserved12[12];
	uint32_t tpg_dma_fifo_reset;
	uint32_t reserved13[44];
	uint32_t video_compression_mode_ctrl;
	uint32_t video_compression_mode_ctrl2;
	uint32_t cmd_compression_mode_ctrl;
	uint32_t cmd_compression_mode_ctrl2;
	uint32_t cmd_compression_mode_ctrl3;
};

check_member(dsi_regs, video_mode_active_h, 0x24);
check_member(dsi_regs, cmd_mode_mdp_stream0_ctrl, 0x58);
check_member(dsi_regs, trig_ctrl, 0x84);
check_member(dsi_regs, cmd_mode_dma_sw_trigger, 0x90);
check_member(dsi_regs, misr_cmd_ctrl, 0xA0);
check_member(dsi_regs, hs_timer_ctrl, 0xBC);
check_member(dsi_regs, err_int_mask0, 0x10C);
check_member(dsi_regs, test_pattern_gen_ctrl, 0x15C);
check_member(dsi_regs, test_pattern_gen_cmd_dma_init_val, 0x17C);
check_member(dsi_regs, cmd_mode_mdp_ctrl2, 0x1B8);
check_member(dsi_regs, tpg_dma_fifo_reset, 0x1EC);
check_member(dsi_regs, video_compression_mode_ctrl, 0x2A0);

struct dsi_phy_regs {
	uint32_t phy_cmn_revision_id0;
	uint32_t reserved0[3];
	uint32_t phy_cmn_clk_cfg0;
	uint32_t phy_cmn_clk_cfg1;
	uint32_t phy_cmn_glbl_ctrl;
	uint32_t phy_cmn_rbuf_ctrl;
	uint32_t phy_cmn_vreg_ctrl;
	uint32_t phy_cmn_ctrl0;
	uint32_t phy_cmn_ctrl1;
	uint32_t phy_cmn_ctrl2;
	uint32_t phy_cmn_lane_cfg0;
	uint32_t phy_cmn_lane_cfg1;
	uint32_t phy_cmn_pll_ctrl;
	uint32_t reserved1[23];
	uint32_t phy_cmn_dsi_lane_ctrl0;
	uint32_t reserved2[4];
	uint32_t phy_cmn_timing_ctrl[12];
	uint32_t reserved3[4];
	uint32_t phy_cmn_phy_status;
	uint32_t reserved4[68];
	struct {
		uint32_t dln0_cfg[4];
		uint32_t dln0_test_datapath;
		uint32_t dln0_pin_swap;
		uint32_t dln0_hstx_str_ctrl;
		uint32_t dln0_offset_top_ctrl;
		uint32_t dln0_offset_bot_ctrl;
		uint32_t dln0_lptx_str_ctrl;
		uint32_t dln0_lprx_ctrl;
		uint32_t dln0_tx_dctrl;
		uint32_t reserved5[20];
	} phy_ln_regs[5];
};

check_member(dsi_phy_regs, phy_cmn_clk_cfg0, 0x10);
check_member(dsi_phy_regs, phy_cmn_dsi_lane_ctrl0, 0x98);
check_member(dsi_phy_regs, phy_cmn_timing_ctrl[0], 0xAC);
check_member(dsi_phy_regs, phy_cmn_phy_status, 0xEC);
check_member(dsi_phy_regs, phy_ln_regs[0], 0x200);
check_member(dsi_phy_regs, phy_ln_regs[1], 0x280);
check_member(dsi_phy_regs, phy_ln_regs[2], 0x300);
check_member(dsi_phy_regs, phy_ln_regs[3], 0x380);
check_member(dsi_phy_regs, phy_ln_regs[4], 0x400);

struct dsi_phy_pll_qlink_regs {
	uint32_t pll_analog_ctrls_one;
	uint32_t pll_analog_ctrls_two;
	uint32_t pll_int_loop_settings;
	uint32_t pll_int_loop_settings_two;
	uint32_t pll_analog_ctrls_three;
	uint32_t pll_analog_ctrls_four;
	uint32_t pll_int_loop_ctrls;
	uint32_t pll_dsm_divider;
	uint32_t pll_feedback_divider;
	uint32_t pll_system_muxes;
	uint32_t pll_freq_update_ctrl_overrides;
	uint32_t pll_cmode;
	uint32_t pll_cal_settings;
	uint32_t pll_band_sel_cal_timer_low;
	uint32_t pll_band_sel_cal_timer_high;
	uint32_t pll_band_sel_cal_settings;
	uint32_t pll_band_sel_min;
	uint32_t pll_band_sel_max;
	uint32_t pll_band_sel_pfilt;
	uint32_t pll_band_sel_ifilt;
	uint32_t pll_band_sel_cal_settings_two;
	uint32_t pll_band_sel_cal_settings_three;
	uint32_t pll_band_sel_cal_settings_four;
	uint32_t pll_band_sel_icode_high;
	uint32_t pll_band_sel_icode_low;
	uint32_t pll_freq_detect_settings_one;
	uint32_t pll_freq_detect_thresh;
	uint32_t pll_freq_det_refclk_high;
	uint32_t pll_freq_det_refclk_low;
	uint32_t pll_freq_det_pllclk_high;
	uint32_t pll_freq_det_pllclk_low;
	uint32_t pll_pfilt;
	uint32_t pll_ifilt;
	uint32_t pll_pll_gain;
	uint32_t pll_icode_low;
	uint32_t pll_icode_high;
	uint32_t pll_lockdet;
	uint32_t pll_outdiv;
	uint32_t pll_fastlock_ctrl;
	uint32_t pll_pass_out_override_one;
	uint32_t pll_pass_out_override_two;
	uint32_t pll_core_override;
	uint32_t pll_core_input_override;
	uint32_t pll_rate_change;
	uint32_t pll_digital_timers;
	uint32_t pll_digital_timers_two;
	uint32_t pll_decimal_div_start;
	uint32_t pll_frac_div_start_low;
	uint32_t pll_frac_div_start_mid;
	uint32_t pll_frac_div_start_high;
	uint32_t pll_dec_frac_muxes;
	uint32_t pll_decimal_div_start_1;
	uint32_t pll_frac_div_start_low1;
	uint32_t pll_frac_div_start_mid1;
	uint32_t pll_frac_div_start_high1;
	uint32_t reserve0[4];
	uint32_t pll_mash_ctrl;
	uint32_t reserved1[6];
	uint32_t pll_ssc_mux_ctrl;
	uint32_t pll_ssc_stepsize_low1;
	uint32_t pll_ssc_stepsize_high1;
	uint32_t pll_ssc_div_per_low_1;
	uint32_t pll_ssc_div_per_high_1;
	uint32_t pll_ssc_adjper_low_1;
	uint32_t pll_ssc_adjper_high_1;
	uint32_t reserved2[6];
	uint32_t pll_ssc_ctrl;
	uint32_t pll_outdiv_rate;
	uint32_t pll_lockdet_rate[2];
	uint32_t pll_prop_gain_rate[2];
	uint32_t pll_band_set_rate[2];
	uint32_t pll_gain_ifilt_band[2];
	uint32_t pll_fl_int_gain_pfilt_band[2];
	uint32_t pll_pll_fastlock_en_band;
	uint32_t reserved9[3];
	uint32_t pll_freq_tune_accum_init_mux;
	uint32_t pll_lock_override;
	uint32_t pll_lock_delay;
	uint32_t pll_lock_min_delay;
	uint32_t pll_clock_inverters;
	uint32_t pll_spare_and_jpc_overrides;
	uint32_t pll_bias_ctrl_1;
	uint32_t pll_bias_ctrl_2;
	uint32_t pll_alog_obsv_bus_ctrl_1;
	uint32_t pll_common_status_one;
};

check_member(dsi_phy_pll_qlink_regs, pll_mash_ctrl, 0xEC);
check_member(dsi_phy_pll_qlink_regs, pll_ssc_mux_ctrl, 0x108);
check_member(dsi_phy_pll_qlink_regs, pll_ssc_ctrl, 0x13C);
check_member(dsi_phy_pll_qlink_regs, pll_freq_tune_accum_init_mux, 0x17C);

// MDSS_BASE + 0x3A000 (0xAE3A000)
struct mdp_intf_regs {
	uint32_t timing_eng_enable;          // 0x000
	uint32_t intf_config;                // 0x004
	uint32_t intf_hsync_ctl;             // 0x008
	uint32_t intf_vysnc_period_f0;       // 0x00C
	uint32_t intf_vysnc_period_f1;       // 0x010
	uint32_t intf_vysnc_pulse_width_f0;  // 0x014
	uint32_t intf_vysnc_pulse_width_f1;  // 0x018
	uint32_t intf_disp_v_start_f0;       // 0x01C
	uint32_t intf_disp_v_start_f1;       // 0x020
	uint32_t intf_disp_v_end_f0;         // 0x024
	uint32_t intf_disp_v_end_f1;         // 0x028
	uint32_t intf_active_v_start_f0;     // 0x02C
	uint32_t intf_active_v_start_f1;     // 0x030
	uint32_t intf_active_v_end_f0;       // 0x034
	uint32_t intf_active_v_end_f1;       // 0x038
	uint32_t intf_disp_hctl;             // 0x03C
	uint32_t intf_active_hctl;           // 0x040
	uint32_t intf_border_color;          // 0x044
	uint32_t intf_underflow_color;       // 0x048
	uint32_t hsync_skew;                 // 0x04C
	uint32_t polarity_ctl;               // 0x050
	uint32_t test_ctl;                   // 0x054
	uint32_t tp_color0;                  // 0x058
	uint32_t tp_color1;                  // 0x05C
	uint32_t intf_config2;               // 0x060
	uint32_t display_data_hctl;          // 0x064
	uint32_t reserved0[10];              // 0x068-0x08C
	uint32_t intf_panel_format;          // 0x090
	uint32_t reserved1[55];              // 0x094-0x16C
	uint32_t intf_prof_fetch_start;      // 0x170
	uint32_t reserved2a[20];             // 0x174-0x1C0
	uint32_t intf_intr_status;           // 0x1C4
	uint32_t intf_intr_clear;            // 0x1C8
	uint32_t reserved2b[9];              // 0x1CC-0x1EC
	uint32_t throttling_count;           // 0x1F0
	uint32_t throttling_count_msb;       // 0x1F4
	uint32_t reserved2c[2];              // 0x1F8-0x1FC
	uint32_t wd_timer_0_ltj_ctl;         // 0x200
	uint32_t wd_timer_0_ltj_ctl_1;       // 0x204
	uint32_t wd_timer_0_main_max_status; // 0x208
	uint32_t reserved2d[1];              // 0x20C
	uint32_t vsync_timestamp_ctrl;       // 0x210
	uint32_t vsync_timestamp0;           // 0x214
	uint32_t vsync_timestamp1;           // 0x218
	uint32_t mdp_vsync_timestamp0;       // 0x21C
	uint32_t mdp_vsync_timestamp1;       // 0x220
	uint32_t wd_timer_0_jitter_ctl;      // 0x224
	uint32_t wd_timer_0_ltj_slope;       // 0x228
	uint32_t wd_timer_0_ltj_max;         // 0x22C
	uint32_t wd_timer_0_ctl;             // 0x230
	uint32_t wd_timer_0_ctl2;            // 0x234
	uint32_t wd_timer_0_load_value;      // 0x238
	uint32_t wd_timer_0_status_value;    // 0x23C
	uint32_t wd_timer_0_ltj_int_status;  // 0x240
	uint32_t wd_timer_0_ltj_frac_status; // 0x244
	uint32_t wd_timer_0_lfsr_status;     // 0x248
	uint32_t edp_psr_override2;          // 0x24C
	uint32_t prog_line_intr_conf;        // 0x250
	uint32_t edp_psr_update_ctrl;        // 0x254
	uint32_t edp_psr_override;           // 0x258
	uint32_t intf_mux;                   // 0x25C
};

check_member(mdp_intf_regs, intf_panel_format, 0x90);
check_member(mdp_intf_regs, intf_prof_fetch_start, 0x170);
check_member(mdp_intf_regs, intf_mux, 0x25C);

// Base: MDP_0_CTL_BASE = 0xAE16000
struct mdp_ctl_regs {
	uint32_t ctl_layer0;                 // 0x000
	uint32_t ctl_layer1;                 // 0x004
	uint32_t layer_2;                    // 0x008
	uint32_t layer_3;                    // 0x00C
	uint32_t layer_4;                    // 0x010
	uint32_t ctl_top;                    // 0x014
	uint32_t ctl_flush;                  // 0x018
	uint32_t ctl_start;                  // 0x01C
	uint32_t pad0[1];                    // 0x020
	uint32_t layer_5;                    // 0x024
	uint32_t pad1[2];                    // 0x028-0x02C
	uint32_t sw_reset;                   // 0x030
	uint32_t pad2[2];                    // 0x034-0x038
	uint32_t ltm_start_dep;              // 0x03C
	uint32_t ctl_layer_0_ext;            // 0x040
	uint32_t ctl_layer_1_ext;            // 0x044
	uint32_t layer_2_ext;                // 0x048
	uint32_t layer_3_ext;                // 0x04C
	uint32_t layer_4_ext;                // 0x050
	uint32_t layer_5_ext;                // 0x054
	uint32_t pad3[2];                    // 0x058-0x05C
	uint32_t sw_reset_override;          // 0x060
	uint32_t ctl_status;                 // 0x064
	uint32_t sw_reset_ctrl;              // 0x068
	uint32_t pad4[1];                    // 0x06C
	uint32_t ctl_layer_0_ext2;           // 0x070
	uint32_t ctl_layer_1_ext2;           // 0x074
	uint32_t layer_2_ext2;               // 0x078
	uint32_t layer_3_ext2;               // 0x07C
	uint32_t layer_4_ext2;               // 0x080
	uint32_t layer_5_ext2;               // 0x084
	uint32_t pad5[2];                    // 0x088-0x08C
	uint32_t flush_mask;                 // 0x090
	uint32_t null_flush_status;          // 0x094
	uint32_t null_flush_clear;           // 0x098
	uint32_t flush_complete;             // 0x09C
	uint32_t layer_0_ext3;               // 0x0A0
	uint32_t layer_1_ext3;               // 0x0A4
	uint32_t layer_2_ext3;               // 0x0A8
	uint32_t layer_3_ext3;               // 0x0AC
	uint32_t layer_4_ext3;               // 0x0B0
	uint32_t layer_5_ext3;               // 0x0B4
	uint32_t ctl_layer_0_ext4;           // 0x0B8
	uint32_t ctl_layer_1_ext4;           // 0x0BC
	uint32_t layer_2_ext4;               // 0x0C0
	uint32_t layer_3_ext4;               // 0x0C4
	uint32_t layer_4_ext4;               // 0x0C8
	uint32_t layer_5_ext4;               // 0x0CC
	uint32_t ctl_prepare;                // 0x0D0
	uint32_t lut_dma_trigger;            // 0x0D4
	uint32_t lut_dma_queue_0_flush_ctrl; // 0x0D8
	uint32_t lut_dma_queue_1_flush_ctrl; // 0x0DC
	uint32_t lut_dma_start_dep;          // 0x0E0
	uint32_t ctl_merge_3d_active;        // 0x0E4
	uint32_t dsc_active;                 // 0x0E8
	uint32_t wb_active;                  // 0x0EC
	uint32_t cwb_active;                 // 0x0F0
	uint32_t ctl_intf_active;            // 0x0F4
	uint32_t ctl_cdm_active;             // 0x0F8
	uint32_t ctl_fetch_pipe_active;      // 0x0FC
	uint32_t merge_3d_flush;             // 0x100
	uint32_t dsc_flush;                  // 0x104
	uint32_t wb_flush;                   // 0x108
	uint32_t cwb_flush;                  // 0x10C
	uint32_t ctl_intf_flush;             // 0x110
	uint32_t cdm_flush;                  // 0x114
	uint32_t lut_dma_sb_flush_ctrl;      // 0x118
	uint32_t pad6[3];                    // 0x11C-0x124
	uint32_t periph_flush;               // 0x128
	uint32_t pad7[2];                    // 0x12C-0x130
	uint32_t ctl_intf_master;            // 0x134
	uint32_t uidle_active;               // 0x138
	uint32_t dspp_0_flush;               // 0x13C
	uint32_t dspp_1_flush;               // 0x140
	uint32_t dspp_2_flush;               // 0x144
	uint32_t dspp_3_flush;               // 0x148
	uint32_t dspp_4_flush;               // 0x14C
	uint32_t merge_3d_flush_complete;    // 0x150
	uint32_t dsc_flush_complete;         // 0x154
	uint32_t cwb_flush_complete;         // 0x158
	uint32_t intf_flush_complete;        // 0x15C
	uint32_t periph_flush_complete;      // 0x160
	uint32_t wb_flush_complete;          // 0x164
};

check_member(mdp_ctl_regs, ctl_top, 0x14);
check_member(mdp_ctl_regs, ctl_intf_active, 0xF4);
check_member(mdp_ctl_regs, ctl_intf_flush, 0x110);
check_member(mdp_ctl_regs, ctl_intf_master, 0x134);

// 0xAE4F000
struct mdp_merge_3d_regs {
	uint32_t reserved0;
	uint32_t mode; // 0x004
};

struct mdp_layer_mixer_regs {
	uint32_t layer_op_mode;        // 0x000
	uint32_t layer_out_size;       // 0x004
	uint32_t layer_border_color_0; // 0x008
	uint32_t layer_border_color_1; // 0x00C
	uint32_t layer_config_0x10;    // 0x010
	uint32_t reserved0[3];         // 0x014, 0x018, 0x01C
	struct {
		uint32_t layer_blend_op;                   // 0x00
		uint32_t layer_blend_const_alpha;          // 0x04
		uint32_t layer_blend_fg_color_fill_color0; // 0x08
		uint32_t layer_blend_fg_color_fill_color1; // 0x0C
		uint32_t layer_blend_fg_fill_size;         // 0x10
		uint32_t layer_blend_fg_fill_xy;           // 0x14
	} layer_blend[6]; // 0x020 start, array of 6 stages (0..5)
};

struct mdp_sspp_regs {                            // MDP_VP_0_SSPP_BASE = MDSS_BASE + 0x5000
	uint32_t sspp_src_size;                   // 0x0000
	uint32_t sspp_src_img_size;               // 0x0004
	uint32_t sspp_src_xy;                     // 0x0008
	uint32_t sspp_out_size;                   // 0x000C
	uint32_t sspp_out_xy;                     // 0x0010
	uint32_t sspp_src0;                       // 0x0014
	uint32_t sspp_src1;                       // 0x0018
	uint32_t sspp_src2;                       // 0x001C
	uint32_t sspp_src3;                       // 0x0020
	uint32_t sspp_src_ystride0;               // 0x0024
	uint32_t sspp_src_ystride1;               // 0x0028
	uint32_t sspp_tile_frame_size;            // 0x002C
	uint32_t sspp_src_format;                 // 0x0030
	uint32_t sspp_src_unpack_pattern;         // 0x0034
	uint32_t sspp_src_op_mode;                // 0x0038
	uint32_t reserved0_a[9];                  // 0x003C-0x05F
	uint32_t sspp_danger_lut;                 // 0x0060
	uint32_t sspp_safe_lut;                   // 0x0064
	uint32_t reserved0_b[1];                  // 0x0068
	uint32_t sspp_qos_ctrl;                   // 0x006C
	uint32_t reserved0_c[1];                  // 0x0070
	uint32_t sspp_creq_lut_0;                 // 0x0074
	uint32_t sspp_creq_lut_1;                 // 0x0078
	uint32_t reserved0_d[33];                 // 0x007C-0x0FF
	uint32_t sspp_sw_pix_ext_c0_lr;           // 0x0100
	uint32_t sspp_sw_pix_ext_c0_tb;           // 0x0104
	uint32_t sspp_sw_pic_ext_c0_req_pixels;   // 0x0108
	uint32_t reserved1[1];                    // 0x010C
	uint32_t sspp_sw_pix_ext_c1c2_lr;         // 0x0110
	uint32_t sspp_sw_pix_ext_c1c2_tb;         // 0x0114
	uint32_t sspp_sw_pic_ext_c1c2_req_pixels; // 0x0118
	uint32_t reserved2[1];                    // 0x011C
	uint32_t sspp_sw_pix_ext_c3_lr;           // 0x0120
	uint32_t sspp_sw_pix_ext_c3_tb;           // 0x0124
	uint32_t sspp_sw_pic_ext_c3_req_pixels;   // 0x0128
};

check_member(mdp_sspp_regs, sspp_sw_pic_ext_c0_req_pixels, 0x108);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c1c2_req_pixels, 0x118);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c3_req_pixels, 0x128);

// Defined relative to Pipe Base + 0xA00
struct mdp_qseed3_regs {
	uint32_t reserved_0;        // 0xA00
	uint32_t op_mode;           // 0xA04
	uint32_t reserved_1[2];     // 0xA08-0xA0C
	uint32_t phase_step_y_h;    // 0xA10
	uint32_t phase_step_y_v;    // 0xA14
	uint32_t phase_step_uv_h;   // 0xA18
	uint32_t phase_step_uv_v;   // 0xA1C
	uint32_t preload;           // 0xA20
	uint32_t reserved_2[7];     // 0xA24-0xA3F
	uint32_t src_size_y_rgb_a;  // 0xA40
	uint32_t src_size_uv;       // 0xA44
	uint32_t dst_size;          // 0xA48
	uint32_t coef_lut_ctrl;     // 0xA4C
	uint32_t reserved_3[16];    // 0xA50-0xA8F
	uint32_t phase_init_y_h;    // 0xA90
	uint32_t phase_init_y_v;    // 0xA94
	uint32_t phase_init_uv_h;   // 0xA98
	uint32_t phase_init_uv_v;   // 0xA9C
	uint32_t reserved_4[24];    // 0xAA0-0xAFF
	uint32_t coef_lut_data[33]; // 0xB00-0xB80
};

struct mdss_hw_regs {
	uint32_t hw_version;
};

check_member(mdss_hw_regs, hw_version, 0x0);

struct vbif_rt_regs {
	uint32_t _rsvd_0000;                             // 0x0000
	uint32_t vbif_clkon;                             // 0x0004
	uint32_t _rsvd_0008_00D4[(0x00D8 - 0x0008) / 4]; // 0x0008-0x00D4
	uint32_t vbif_ddr_out_max_burst;                 // 0x00D8
	uint32_t _rsvd_00DC_0120[(0x0124 - 0x00DC) / 4]; // 0x00DC-0x0120
	uint32_t vbif_round_robin_qos_arb;               // 0x0124
	uint32_t _rsvd_0128_015C[(0x0160 - 0x0128) / 4]; // 0x0128-0x015C
	uint32_t vbif_out_axi_amemtype_conf0;            // 0x0160
	uint32_t vbif_out_axi_amemtype_conf1;            // 0x0164
	uint32_t _rsvd_0168_016C[(0x0170 - 0x0168) / 4]; // 0x0168-0x016C
	uint32_t vbif_out_axi_ashared;                   // 0x0170
	uint32_t vbif_out_axi_innershared;               // 0x0174
	uint32_t vbif_out_axi_aooo_en;                   // 0x0178
	uint32_t vbif_out_axi_aooo;                      // 0x017C
	uint32_t _rsvd_0180_054C[(0x0550 - 0x0180) / 4]; // 0x0180-0x054C
	struct {
		uint32_t vbif_xinl_qos_rp_remap; // 0x0550 + idx*0x8
		uint32_t vbif_xinh_qos_rp_remap; // 0x0554 + idx*0x8
	} qos_rp_remap[8];
	struct {
		uint32_t vbif_xinl_qos_lvl_remap; // 0x0590 + idx*0x8
		uint32_t vbif_xinh_qos_lvl_remap; // 0x0594 + idx*0x8
	} qos_lvl_remap[8];
};

struct vbif_nrt_regs {
	uint32_t _rsvd_0000;                             // 0x0000
	uint32_t vbif_clkon;                             // 0x0004
	uint32_t vbif_clk_force_ctrl0;                   // 0x0008
	uint32_t vbif_clk_force_ctrl1;                   // 0x000C
	uint32_t vbif_qos_override_en;                   // 0x0010
	uint32_t vbif_prilvl_act_prop_en;                // 0x0014
	uint32_t vbif_qos_override_reqpri0;              // 0x0018
	uint32_t _rsvd_001C;                             // 0x001C
	uint32_t vbif_qos_override_prilvl0;              // 0x0020
	uint32_t _rsvd_0024_00A8[(0x00AC - 0x0024) / 4]; // 0x0024-0x00A8
	uint32_t vbif_write_gather_en;                   // 0x00AC
	uint32_t vbif_in_rd_lim_conf0;                   // 0x00B0
	uint32_t _rsvd_00B4_00BC[(0x00C0 - 0x00B4) / 4]; // 0x00B4-0x00BC
	uint32_t vbif_in_wr_lim_conf0;                   // 0x00C0
	uint32_t _rsvd_00C4_00CC[(0x00D0 - 0x00C4) / 4]; // 0x00C4-0x00CC
	uint32_t vbif_out_rd_lim_conf0;                  // 0x00D0
	uint32_t vbif_out_wr_lim_conf0;                  // 0x00D4
	uint32_t vbif_ddr_out_max_burst;                 // 0x00D8
	uint32_t _rsvd_00DC_00EC[(0x00F0 - 0x00DC) / 4]; // 0x00DC-0x00EC
	uint32_t vbif_arb_ctl;                           // 0x00F0
	uint32_t vbif_ddr_arb_conf0;                     // 0x00F4
	uint32_t _rsvd_00F8_00FC[(0x0100 - 0x00F8) / 4]; // 0x00F8-0x00FC
	uint32_t vbif_fixed_arb_conf0;                   // 0x0100
	uint32_t _rsvd_0104;                             // 0x0104
	uint32_t vbif_arb_lcb;                           // 0x0108
	uint32_t _rsvd_010C_0120[(0x0124 - 0x010C) / 4]; // 0x010C-0x0120
	uint32_t vbif_round_robin_qos_arb;               // 0x0124
	uint32_t _rsvd_0128_015C[(0x0160 - 0x0128) / 4]; // 0x0128-0x015C
	uint32_t vbif_out_axi_amemtype_conf0;            // 0x0160
	uint32_t vbif_out_axi_amemtype_conf1;            // 0x0164
	uint32_t vbif_out_axi_ainst_override_en;         // 0x0168
	uint32_t vbif_out_axi_ainst;                     // 0x016C
	uint32_t vbif_out_axi_ashared;                   // 0x0170
	uint32_t vbif_out_axi_innershared;               // 0x0174
	uint32_t vbif_out_axi_aooo_en;                   // 0x0178
	uint32_t vbif_out_axi_aooo;                      // 0x017C
};

check_member(vbif_rt_regs, vbif_out_axi_amemtype_conf0, 0x160);
check_member(vbif_rt_regs, qos_rp_remap[0], 0x550);

struct mdp_periph_top1_regs {
	/* 0x000-0x014: reserved area up to first known register */
	uint32_t reserved_000[0x018u / 4]; // 0x000-0x014

	/* 0x018: HDMI/DP core select */
	uint32_t hdmi_dp_core_select; // 0x018

	/* 0x01C-0x090: reserved area until HPD select */
	uint32_t reserved_01C[(0x094u - (0x018u + 4)) / 4];

	/* 0x094: DP HPD select */
	uint32_t dp_hpd_select; // 0x094
};

enum {
	INTF = BIT(31),
	PERIPH = BIT(30),
	CWB = BIT(28),
	ROT = BIT(27),
	CDM_0 = BIT(26),
	DMA_3 = BIT(25),
	DMA_2 = BIT(24),
	MERGE_3D = BIT(23),
	DSC = BIT(22),
	DSPP_3 = BIT(21),
	LAYER_MIXER_5 = BIT(20),
	DSPP_PA_LUTV_3 = BIT(19),
	VIG_3 = BIT(18),
	CTL = BIT(17),
	WB = BIT(16),
	DSPP_2 = BIT(15),
	DSPP_1 = BIT(14),
	DSPP_0 = BIT(13),
	DMA_1 = BIT(12),
	DMA_0 = BIT(11),
	LAYER_MIXER_4 = BIT(10),
	LAYER_MIXER_3 = BIT(9),
	LAYER_MIXER_2 = BIT(8),
	LAYER_MIXER_1 = BIT(7),
	LAYER_MIXER_0 = BIT(6),
	DSPP_PA_LUTV_2 = BIT(5),
	DSPP_PA_LUTV_1 = BIT(4),
	DSPP_PA_LUTV_0 = BIT(3),
	VIG_2 = BIT(2),
	VIG_1 = BIT(1),
	VIG_0 = BIT(0),
};

enum {
	DSI_AHBS_HCLK_ON = BIT(0),
	DSI_AHBM_SCLK_ON = BIT(1),
	DSI_PCLK_ON = BIT(2),
	DSI_DSICLK_ON = BIT(3),
	DSI_BYTECLK_ON = BIT(4),
	DSI_ESCCLK_ON = BIT(5),
	DSI_FORCE_ON_DYN_AHBS_HCLK = BIT(8),
	DSI_FORCE_ON_DYN_AHBM_HCLK = BIT(9),
	DSI_FORCE_ON_DYN_DSICLK = BIT(10),
	DSI_FORCE_ON_DYN_BYTECLK = BIT(11),
	DSI_AHBS_HCLK_HYSTERISIS1_CTRL = (3 << 11),
	DSI_AHBM_HCLK_HYSTERISIS1_CTRL = (3 << 13),
	DSI_DSICLK_HYSTERISIS1_CTRL = (3 << 15),
	DSI_FORCE_ON_DYN_PCLK = BIT(20),
	DSI_FORCE_ON_LANE_LAYER_TG_BYTECLK = BIT(21),
	DSI_DMA_CLK_STOP = BIT(22),
};

enum {
	DSI_CMD_MODE_DMA_DONE_AK = BIT(0),
	DSI_CMD_MODE_DMA_DONE_STAT = BIT(0),
	DSI_CMD_MODE_DMA_DONE_MASK = BIT(1),
	DSI_CMD_MODE_MDP_DONE_AK = BIT(8),
	DSI_CMD_MODE_MDP_DONE_STAT = BIT(8),
	DSI_CMD_MODE_MDP_DONE_MASK = BIT(9),
	DSI_CMD_MDP_STREAM0_DONE_AK = BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_STAT = BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_MASK = BIT(11),
	DSI_VIDEO_MODE_DONE_AK = BIT(16),
	DSI_VIDEO_MODE_DONE_STAT = BIT(16),
	DSI_VIDEO_MODE_DONE_MASK = BIT(17),
	DSI_BTA_DONE_AK = BIT(20),
	DSI_BTA_DONE_STAT = BIT(20),
	DSI_BTA_DONE_MASK = BIT(21),
	DSI_ERROR_AK = BIT(24),
	DSI_ERROR_STAT = BIT(24),
	DSI_ERROR_MASK = BIT(25),
	DSI_DYNAMIC_BLANKING_DMA_DONE_AK = BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_STAT = BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_MASK = BIT(27),
	DSI_DYNAMIC_REFRESH_DONE_AK = BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_STAT = BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_MASK = BIT(29),
	DSI_DESKEW_DONE_AK = BIT(30),
	DSI_DESKEW_DONE_STAT = BIT(30),
	DSI_DESKEW_DONE_MASK = BIT(31),
};

enum {
	WR_MEM_START = 255,
	WR_MEM_CONTINUE = 255 << 8,
	INSERT_DCS_COMMAND = BIT(16),
};

enum {
	PACKET_TYPE = BIT(24),
	POWER_MODE = BIT(26),
	EMBEDDED_MODE = BIT(28),
	WC_SEL = BIT(29),
	BROADCAST_MASTER = BIT(30),
	BROADCAST_EN = BIT(31),
};

enum {
	BWC_DEC_EN = BIT(0),
	SW_PIX_EXT_OVERRIDE = BIT(31),
};

enum {
	INTERLACE_MODE = BIT(0),
	REPEAT_PIXEL = BIT(1),
	INTERLACE_INIT_SEL = BIT(2),
	BORDER_ENABLE = BIT(3),
	EDP_PSR_OVERRIDE_EN = BIT(7),
	PACK_ALIGN = BIT(10),
	DSI_VIDEO_STOP_MODE = BIT(23),
	ACTIVE_H_EN = BIT(29),
	ACTIVE_V_EN = BIT(30),
	PROG_FETCH_START_EN = BIT(31),
};

enum {
	VIG_0_OUT = BIT(0),
	VIG_1_OUT = BIT(0),
	BORDER_OUT = BIT(24),
};

enum {
	FETCH_PIPE_VIG0_ACTIVE = BIT(16),
	FETCH_PIPE_VIG1_ACTIVE = BIT(17),
};

enum {
	INTF_ACTIVE_0 = BIT(0),
	INTF_ACTIVE_1 = BIT(1),
	INTF_ACTIVE_5 = BIT(5),
};

enum {
	INTF_FLUSH_0 = BIT(0),
	INTF_FLUSH_1 = BIT(1),
	INTF_FLUSH_5 = BIT(5),
};

enum mdss_flush_bits {
	FLUSH_INTF        = BIT(31),
	FLUSH_PERIPH      = BIT(30),
	FLUSH_MERGE_3D    = BIT(23),
	FLUSH_CTL         = BIT(17),
	FLUSH_LM1         = BIT(7),
	FLUSH_LM0         = BIT(6),
	FLUSH_VIG1        = BIT(1),
	FLUSH_VIG0        = BIT(0),
};

#define MDP_CTL_LAYER_VIG0_OUT_SHIFT   0
#define MDP_CTL_LAYER_VIG1_OUT_SHIFT   3
#define MDP_CTL_LAYER_BORDER_OUT      BIT(24)
#define MDP_LM_STAGE3_FG              0x5

#define MDP_CTL_LAYER0_VIG0_STAGE3_CFG \
	((MDP_LM_STAGE3_FG << MDP_CTL_LAYER_VIG0_OUT_SHIFT) | \
	MDP_CTL_LAYER_BORDER_OUT)

#define MDP_CTL_LAYER1_VIG1_STAGE3_CFG \
	((MDP_LM_STAGE3_FG << MDP_CTL_LAYER_VIG1_OUT_SHIFT) | \
	MDP_CTL_LAYER_BORDER_OUT)

#define LM_LAYER_OP_MODE_SPLIT_RIGHT	BIT(31)

enum {
	MDSS_HW_REG_BASE = MDSS_BASE + 0x00000000,
	MDSS_HW_REG_BASE_SIZE = 0x1000,
	MDSS_HW_REG_BASE_USED = 0x154,
	MDSS_HW_REG_BASE_OFFS = 0x00000000,
};
enum edp_init_values {
	EDP_HPD_INT_ACK_CLEAR_ALL = 0x0000000F,    // Clear any pending HPD interrupts
	EDP_AUX_CTRL_ENABLE_BASIC = 0x00000011,    // AUX_CTRL = 0x11
	EDP_AUX_CTRL_ENABLE_RST = 0x00000013,      // AUX_CTRL = 0x13 (AUX reset toggle)
	EDP_HPD_REFTIMER_ENABLE_0013 = 0x00010013, // REFTIMER_ENABLE + interval
	EDP_HPD_CTRL_ENABLE = 0x00000001,          // Enable HPD logic with default polarity
	MDP_DP_HPD_SELECT_HPDDPT3 = 0x00000322,    // Select HPDDPT3
	MDP_HDMI_DP_CORE_SELECT_DP = 0x00000001,   // Select DP core
	EDP_AUX_TIMEOUT_COUNT_FFFF = 0x0000FFFF,   // Timeout count
	EDP_AUX_LIMITS_FFFF = 0x0000FFFF,          // Retry, NACK/DEFER, read-zero limits
	READ_WAIT2_WR_WAIT1 = 0x00000012,
	AHB2EDPPHY_AHB2PHY_AHB2PHY_TOP_CFG = 0xAEC6010,
};

#define MDP_SSPP_TOP0_REG_BASE                 (MDSS_BASE + 0x00001000)
#define HWIO_MDP_INTR_EN_ADDR                  (MDP_SSPP_TOP0_REG_BASE + 0x10) // 0xAE01010
#define HWIO_MDP_INTR_STATUS_ADDR              (MDP_SSPP_TOP0_REG_BASE + 0x14) // 0xAE01014
#define HWIO_MDP_INTR_CLEAR_ADDR               (MDP_SSPP_TOP0_REG_BASE + 0x18) // 0xAE01018
#define HWIO_MDP_INTR_EN_INTF_5_VSYNC_BMSK     0x800000
#define HWIO_MDP_INTR_EN_PING_PONG_0_DONE_BMSK 0x100

#define GPIO_PANEL_POWER_ON GPIO(70)  // INTERNAL_EDP_REG_EN_GPIO
#define GPIO_PANEL_HPD      GPIO(119) // INTERNAL_EDP_HPD_GPIO

static struct dsi_regs *const dsi0 = (void *)DSI0_CTL_BASE;
static struct dsi_phy_regs *const dsi0_phy = (void *)DSI0_PHY_BASE;
static struct dsi_phy_pll_qlink_regs *const phy_pll_qlink = (void *)DSI0_PHY_PLL_QLINK_COM;

static struct mdp_intf_regs *const mdp_intf = (void *)MDP_5_INTF_BASE;
static struct mdp_ctl_regs *const mdp_ctl_0 = (void *)MDP_0_CTL_BASE;
static struct mdp_layer_mixer_regs *const mdp_layer_mixer = (void *)MDP_VP_0_LAYER_MIXER_BASE;
static struct mdp_sspp_regs *const mdp_sspp = (void *)MDP_VP_0_SSPP_BASE;
static struct vbif_rt_regs *const vbif_rt = (void *)MDP_VBIF_RT_BASE;
static struct mdp_merge_3d_regs *const mdp_merge_3d_0 = (void *)0xAE4F000;
static struct vbif_nrt_regs *const vbif_nrt = (struct vbif_nrt_regs *)VBIF_NRT_BASE;
static struct mdp_periph_top1_regs *const mdp_periph_top1 =
	(struct mdp_periph_top1_regs *)MDP_PERIPH_TOP1;

void merge_3d_active(struct edid *edid);
void configure_vbif_qos(void);
void intf_tg_setup(struct edid *edid);
void intf_fetch_start_config(struct edid *edid);
void mdss_source_pipe_config(struct edid *edid, uintptr_t dram_display);
void mdss_layer_mixer_setup(struct edid *edid);
uint32_t calculate_mode_mdp_clk(const struct edid_mode *mode);

#endif // _SOC_DISPLAY_MDSS_REG_H_
