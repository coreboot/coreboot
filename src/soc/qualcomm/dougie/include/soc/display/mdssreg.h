/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_DOUGIE_DISPLAY_MDSSREG_H_
#define _SOC_QUALCOMM_DOUGIE_DISPLAY_MDSSREG_H_

#include <types.h>
#include <edid.h>

#define INTF_FLUSH      INTF_FLUSH_5

struct mdp_intf_regs {
	uint32_t timing_eng_enable;
	uint32_t intf_config;
	uint32_t intf_hsync_ctl;
	uint32_t intf_vsync_period_f0;
	uint32_t intf_vsync_period_f1;
	uint32_t intf_vsync_pulse_width_f0;
	uint32_t intf_vsync_pulse_width_f1;
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
	uint32_t ctl_fetch_pipe_active;
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
	MDP_0_CTL_BASE			= MDSS_BASE + 0x16000,
	MDP_VP_0_SSPP_BASE		= MDSS_BASE + 0x5000,
	MDP_VP_0_LAYER_MIXER_BASE	= MDSS_BASE + 0x45000,
	MDP_5_INTF_BASE			= MDSS_BASE + 0x3A000,
	MDP_VBIF_RT_BASE		= MDSS_BASE + 0xB0000,
};

/* MDP_CTL_0_FLUSH */
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

/* MDP_CTL_0_INTF_ACTIVE */
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

static struct mdss_hw_regs *const mdss_hw = (void *)MDSS_BASE;
static struct mdp_intf_regs *const mdp_intf = (void *)MDP_5_INTF_BASE;
static struct mdp_ctl_regs *const mdp_ctl = (void *)MDP_0_CTL_BASE;
static struct mdp_layer_mixer_regs *const mdp_layer_mixer = (void *)MDP_VP_0_LAYER_MIXER_BASE;
static struct mdp_sspp_regs *const mdp_sspp = (void *)MDP_VP_0_SSPP_BASE;
static struct vbif_rt_regs *const vbif_rt = (void *)MDP_VBIF_RT_BASE;

void mdss_intf_tg_setup(struct edid *edid);
void mdp_dsi_video_config(struct edid *edid);
void mdp_dsi_video_on(void);
void enable_mdss_clk(void);
void mdss_ctrl_config(void);

#endif /* _SOC_QUALCOMM_DOUGIE_DISPLAY_MDSSREG_H_ */
