/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DISPLAY_MDSS_REG_H_
#define _SOC_DISPLAY_MDSS_REG_H_

#include <types.h>
#include <edid.h>

#define INTF_FLUSH	INTF_FLUSH_1

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
check_member(dsi_regs, test_pattern_gen_ctrl, 0x15c);
check_member(dsi_regs, test_pattern_gen_cmd_dma_init_val, 0x17c);
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

struct mdp_intf_regs {
		uint32_t timing_eng_enable;
		uint32_t intf_config;
		uint32_t intf_hsync_ctl;
		uint32_t intf_vysnc_period_f0;
		uint32_t intf_vysnc_period_f1;
		uint32_t intf_vysnc_pulse_width_f0;
		uint32_t intf_vysnc_pulse_width_f1;
		uint32_t intf_disp_v_start_f0;
		uint32_t intf_disp_v_start_f1;
		uint32_t intf_disp_v_end_f0;
		uint32_t intf_disp_v_end_f1;
		uint32_t intf_active_v_start_f0;
		uint32_t intf_active_v_start_f1;
		uint32_t intf_active_v_end_f0;
		uint32_t intf_active_v_end_f1;
		uint32_t intf_disp_hctl;
		uint32_t intf_active_hctl;
		uint32_t intf_border_color;
		uint32_t intf_underflow_color;
		uint32_t hsync_skew;
		uint32_t polarity_ctl;
		uint32_t test_ctl;
		uint32_t tp_color0;
		uint32_t tp_color1;
		uint32_t intf_config2;
		uint32_t display_data_hctl;
		uint32_t reserved0[10];
		uint32_t intf_panel_format;
		uint32_t reserved1[55];
		uint32_t intf_prof_fetch_start;
		uint32_t reserved2[58];
		uint32_t intf_mux;
};

check_member(mdp_intf_regs, intf_panel_format, 0x90);
check_member(mdp_intf_regs, intf_prof_fetch_start, 0x170);
check_member(mdp_intf_regs, intf_mux, 0x25C);

struct mdp_ctl_regs {
	uint32_t ctl_layer0;
	uint32_t ctl_layer1;
	uint32_t reserved0[3];
	uint32_t ctl_top;
	uint32_t ctl_flush;
	uint32_t ctl_start;
	uint32_t reserved1[53];
	uint32_t ctl_intf_active;
	uint32_t ctl_cdm_active;
	uint32_t ctl_fetch_pipe_active; /* reserved for sc7180 */
	uint32_t reserved2[4];
	uint32_t ctl_intf_flush;
};

check_member(mdp_ctl_regs, ctl_top, 0x14);
check_member(mdp_ctl_regs, ctl_intf_active, 0xF4);
check_member(mdp_ctl_regs, ctl_intf_flush, 0x110);

struct mdp_layer_mixer_regs {
		uint32_t layer_op_mode;
		uint32_t layer_out_size;
		uint32_t layer_border_color_0;
		uint32_t layer_border_color_1;
		uint32_t reserved0[4];
		struct {
			uint32_t layer_blend_op;
			uint32_t layer_blend_const_alpha;
			uint32_t layer_blend_fg_color_fill_color0;
			uint32_t layer_blend_fg_color_fill_color1;
			uint32_t layer_blend_fg_fill_size;
			uint32_t layer_blend_fg_fill_xy;
		} layer_blend[6];
};

struct mdp_sspp_regs {
	uint32_t sspp_src_size;
	uint32_t sspp_src_img_size;
	uint32_t sspp_src_xy;
	uint32_t sspp_out_size;
	uint32_t sspp_out_xy;
	uint32_t sspp_src0;
	uint32_t sspp_src1;
	uint32_t sspp_src2;
	uint32_t sspp_src3;
	uint32_t sspp_src_ystride0;
	uint32_t sspp_src_ystride1;
	uint32_t sspp_tile_frame_size;
	uint32_t sspp_src_format;
	uint32_t sspp_src_unpack_pattern;
	uint32_t sspp_src_op_mode;
	uint32_t reserved0[51];
	uint32_t sspp_sw_pic_ext_c0_req_pixels;
	uint32_t reserved1[3];
	uint32_t sspp_sw_pic_ext_c1c2_req_pixels;
	uint32_t reserved2[3];
	uint32_t sspp_sw_pic_ext_c3_req_pixels;
};

check_member(mdp_sspp_regs, sspp_sw_pic_ext_c0_req_pixels, 0x108);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c1c2_req_pixels, 0x118);
check_member(mdp_sspp_regs, sspp_sw_pic_ext_c3_req_pixels, 0x128);

struct mdss_hw_regs {
	uint32_t hw_version;
};

struct vbif_rt_regs {
	uint32_t reserved0[88];
	uint32_t vbif_out_axi_amemtype_conf0;
	uint32_t vbif_out_axi_amemtype_conf1;
	uint32_t reserved1[250];
	struct {
		uint32_t vbif_xinl_qos_rp_remap;
		uint32_t vbif_xinh_qos_rp_remap;
	} qos_rp_remap[8];
	struct {
		uint32_t vbif_xinl_qos_lvl_remap;
		uint32_t vbif_xinh_qos_lvl_remap;
	} qos_lvl_remap[8];
};

check_member(vbif_rt_regs, vbif_out_axi_amemtype_conf0, 0x160);
check_member(vbif_rt_regs, qos_rp_remap[0], 0x550);

enum {
	MDSS_BASE = 0xAE00000,
};

enum {
	MDP_0_CTL_BASE			= MDSS_BASE + 0x2000,
	MDP_VP_0_SSPP_BASE		= MDSS_BASE + 0x5000,
	MDP_VP_0_LAYER_MIXER_BASE	= MDSS_BASE + 0x45000,
	MDP_1_INTF_BASE			= MDSS_BASE + 0x6b800,
	MDP_VBIF_RT_BASE		= MDSS_BASE + 0xB0000,
	DSI0_CTL_BASE			= MDSS_BASE + 0x94000,
	DSI0_PHY_BASE			= MDSS_BASE + 0x94400,
	DSI0_PHY_DLN0_BASE		= MDSS_BASE + 0x94600,
	DSI0_PHY_DLN1_BASE		= MDSS_BASE + 0x94680,
	DSI0_PHY_DLN2_BASE		= MDSS_BASE + 0x94700,
	DSI0_PHY_DLN3_BASE		= MDSS_BASE + 0x94780,
	DSI0_PHY_CLKLN_BASE		= MDSS_BASE + 0x94800,
	DSI0_PHY_PLL_QLINK_COM		= MDSS_BASE + 0x94a00,
};

/* DSI_0_CLK_CTRL */
enum {
	INTF		= BIT(31),
	PERIPH		= BIT(30),
	CWB		= BIT(28),
	ROT		= BIT(27),
	CDM_0		= BIT(26),
	DMA_3		= BIT(25),
	DMA_2		= BIT(24),
	MERGE_3D	= BIT(23),
	DSC		= BIT(22),
	DSPP_3		= BIT(21),
	LAYER_MIXER_5	= BIT(20),
	DSPP_PA_LUTV_3	= BIT(19),
	VIG_3		= BIT(18),
	CTL		= BIT(17),
	WB		= BIT(16),
	DSPP_2		= BIT(15),
	DSPP_1		= BIT(14),
	DSPP_0		= BIT(13),
	DMA_1		= BIT(12),
	DMA_0		= BIT(11),
	LAYER_MIXER_4	= BIT(10),
	LAYER_MIXER_3	= BIT(9),
	LAYER_MIXER_2	= BIT(8),
	LAYER_MIXER_1	= BIT(7),
	LAYER_MIXER_0	= BIT(6),
	DSPP_PA_LUTV_2	= BIT(5),
	DSPP_PA_LUTV_1	= BIT(4),
	DSPP_PA_LUTV_0	= BIT(3),
	VIG_2		= BIT(2),
	VIG_1		= BIT(1),
	VIG_0		= BIT(0),
};

enum {
	DSI_AHBS_HCLK_ON			= BIT(0),
	DSI_AHBM_SCLK_ON			= BIT(1),
	DSI_PCLK_ON				= BIT(2),
	DSI_DSICLK_ON				= BIT(3),
	DSI_BYTECLK_ON				= BIT(4),
	DSI_ESCCLK_ON				= BIT(5),
	DSI_FORCE_ON_DYN_AHBS_HCLK		= BIT(8),
	DSI_FORCE_ON_DYN_AHBM_HCLK		= BIT(9),
	DSI_FORCE_ON_DYN_DSICLK			= BIT(10),
	DSI_FORCE_ON_DYN_BYTECLK		= BIT(11),
	DSI_AHBS_HCLK_HYSTERISIS1_CTRL		= (3 << 11),
	DSI_AHBM_HCLK_HYSTERISIS1_CTRL		= (3 << 13),
	DSI_DSICLK_HYSTERISIS1_CTRL		= (3 << 15),
	DSI_FORCE_ON_DYN_PCLK			= BIT(20),
	DSI_FORCE_ON_LANE_LAYER_TG_BYTECLK	= BIT(21),
	DSI_DMA_CLK_STOP			= BIT(22),
};

/* DSI_0_INT_CTRL */
enum {
	DSI_CMD_MODE_DMA_DONE_AK		= BIT(0),
	DSI_CMD_MODE_DMA_DONE_STAT		= BIT(0),
	DSI_CMD_MODE_DMA_DONE_MASK		= BIT(1),
	DSI_CMD_MODE_MDP_DONE_AK		= BIT(8),
	DSI_CMD_MODE_MDP_DONE_STAT		= BIT(8),
	DSI_CMD_MODE_MDP_DONE_MASK		= BIT(9),
	DSI_CMD_MDP_STREAM0_DONE_AK		= BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_STAT		= BIT(10),
	DSI_CMD_MDP_STREAM0_DONE_MASK		= BIT(11),
	DSI_VIDEO_MODE_DONE_AK			= BIT(16),
	DSI_VIDEO_MODE_DONE_STAT		= BIT(16),
	DSI_VIDEO_MODE_DONE_MASK		= BIT(17),
	DSI_BTA_DONE_AK				= BIT(20),
	DSI_BTA_DONE_STAT			= BIT(20),
	DSI_BTA_DONE_MASK			= BIT(21),
	DSI_ERROR_AK				= BIT(24),
	DSI_ERROR_STAT				= BIT(24),
	DSI_ERROR_MASK				= BIT(25),
	DSI_DYNAMIC_BLANKING_DMA_DONE_AK	= BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_STAT	= BIT(26),
	DSI_DYNAMIC_BLANKING_DMA_DONE_MASK	= BIT(27),
	DSI_DYNAMIC_REFRESH_DONE_AK		= BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_STAT		= BIT(28),
	DSI_DYNAMIC_REFRESH_DONE_MASK		= BIT(29),
	DSI_DESKEW_DONE_AK			= BIT(30),
	DSI_DESKEW_DONE_STAT			= BIT(30),
	DSI_DESKEW_DONE_MASK			= BIT(31),
};

/* DSI_0_COMMAND_MODE_MDP_DCS_CMD_CTRL */
enum {
	WR_MEM_START		=  255,
	WR_MEM_CONTINUE		=  255 << 8,
	INSERT_DCS_COMMAND	= BIT(16),
};

/* DSI_0_COMMAND_MODE_DMA_CTRL */
enum {
	PACKET_TYPE	 = BIT(24),
	POWER_MODE	 = BIT(26),
	EMBEDDED_MODE	 = BIT(28),
	WC_SEL		 = BIT(29),
	BROADCAST_MASTER = BIT(30),
	BROADCAST_EN	 = BIT(31),
};

/* MDP_VP_0_VIG_0_SSPP_SRC_OP_MODE */
enum {
	BWC_DEC_EN		= BIT(0),
	SW_PIX_EXT_OVERRIDE	= BIT(31),

};

/* MDP_INTF_x_INTF_CONFIG */
enum {
	INTERLACE_MODE		= BIT(0),
	REPEAT_PIXEL		= BIT(1),
	INTERLACE_INIT_SEL	= BIT(2),
	BORDER_ENABLE		= BIT(3),
	EDP_PSR_OVERRIDE_EN	= BIT(7),
	PACK_ALIGN		= BIT(10),
	DSI_VIDEO_STOP_MODE	= BIT(23),
	ACTIVE_H_EN		= BIT(29),
	ACTIVE_V_EN		= BIT(30),
	PROG_FETCH_START_EN	= BIT(31),
};

/* MDP_CTL_0_LAYER_0 */
enum {
	VIG_0_OUT	= BIT(0),
	BORDER_OUT	= BIT(24),
};

/* MDP_CTL_0_FETCH_PIPE_ACTIVE */
enum {
	FETCH_PIPE_VIG0_ACTIVE	= BIT(16),
	FETCH_PIPE_VIG1_ACTIVE	= BIT(17),
};

/* MDP_CTL_0_INTF_ACTIVE*/
enum {
	INTF_ACTIVE_0	= BIT(0),
	INTF_ACTIVE_1	= BIT(1),
	INTF_ACTIVE_5	= BIT(5),
};

/* MDP_CTL_0_INTF_FLUSH */
enum {
	INTF_FLUSH_0	= BIT(0),
	INTF_FLUSH_1	= BIT(1),
	INTF_FLUSH_5	= BIT(5),
};

static struct dsi_regs *const dsi0 = (void *)DSI0_CTL_BASE;
static struct dsi_phy_regs *const dsi0_phy = (void *)DSI0_PHY_BASE;
static struct dsi_phy_pll_qlink_regs *const phy_pll_qlink = (void *)DSI0_PHY_PLL_QLINK_COM;
static struct mdss_hw_regs *const mdss_hw = (void *)MDSS_BASE;
static struct mdp_intf_regs *const mdp_intf = (void *)MDP_1_INTF_BASE;
static struct mdp_ctl_regs *const mdp_ctl = (void *)MDP_0_CTL_BASE;
static struct mdp_layer_mixer_regs *const mdp_layer_mixer = (void *)MDP_VP_0_LAYER_MIXER_BASE;
static struct mdp_sspp_regs *const mdp_sspp = (void *)MDP_VP_0_SSPP_BASE;
static struct vbif_rt_regs *const vbif_rt = (void *)MDP_VBIF_RT_BASE;

void mdp_dsi_video_config(struct edid *edid);
void mdss_intf_tg_setup(struct edid *edid);
void mdp_dsi_video_on(void);
void mdss_ctrl_config(void);

#endif
