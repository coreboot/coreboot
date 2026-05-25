/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>

#include <soc/symbols_common.h>

uint32_t calculate_mode_mdp_clk(const struct edid_mode *mode) {
	/* pixel clock = total_h * total_v * refresh. Use it for mode_mdp_clock */
	uint64_t mode_mdp_clk = (uint64_t)mode->pixel_clock * 1000;

	/* clock inefficiency factor of 5% */
	mode_mdp_clk *= MDSS_MDP_CLK_FUDGE_FACTOR_NUMER;
	mode_mdp_clk /= MDSS_MDP_CLK_FUDGE_FACTOR_DENOM;

	return (uint32_t)mode_mdp_clk;
}

void mdss_source_pipe_config(struct edid *edid, uintptr_t dram_display)
{
	uint32_t pipe_count = ((edid->mode.ha > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ||
			(calculate_mode_mdp_clk(&edid->mode) > MDSS_MAX_MDP_CLK)) ? 2 : 1;

	/* Source Dimensions: Per Pipe */
	uint32_t src_h = edid->mode.va;
	uint32_t src_w = (edid->mode.ha) / pipe_count;
	uint32_t src_size_val = (src_h << 16) | src_w;

	uint32_t dst_w = src_w;
	uint32_t dst_size_val = src_size_val;

	/* Offsets */
	uint32_t start_dst_x = 0; /* X offset here, if applicable */
	uint32_t stride_val = (edid->mode.ha) * 4;

	/* Starting Base Address (VIG_0) */
	uintptr_t current_base = MDP_VP_0_SSPP_BASE;

	for (int i = 0; i < pipe_count; i++) {
		struct mdp_sspp_regs *sspp = (struct mdp_sspp_regs *)current_base;

		/* QSEED3 structure at offset 0xA00 (Disabled) */
		struct mdp_qseed3_regs *qseed3 =
			(struct mdp_qseed3_regs *)(current_base + 0xA00);

		/* Clock Control at offset 0x1A00 (gate QSEED clock OFF) */
		uint32_t *clk_ctrl = (uint32_t *)(current_base + 0x1A00);

		/* Clock Control: disable QSEED clock (if enabled ?= 2) */
		write32(clk_ctrl, 0);

		/* SSPP Config */
		write32(&sspp->sspp_src0, dram_display);
		write32(&sspp->sspp_src1, 0);
		write32(&sspp->sspp_src2, 0);
		write32(&sspp->sspp_src3, 0);

		write32(&sspp->sspp_src_ystride0, stride_val);
		write32(&sspp->sspp_src_ystride1, 0);

		write32(&sspp->sspp_src_format, 0x237FF);
		write32(&sspp->sspp_src_unpack_pattern, 0x3020001);

		/* Dual-pipe positioning */
		uint32_t src_x = i * src_w;
		uint32_t out_x = start_dst_x + (i * dst_w);

		/* Rectangles */
		write32(&sspp->sspp_src_xy, src_x);
		write32(&sspp->sspp_src_size, src_size_val);

		write32(&sspp->sspp_out_xy, out_x);
		write32(&sspp->sspp_out_size, dst_size_val);

		/* QSEED3: DISABLE */
		write32(&qseed3->coef_lut_ctrl, 0x0);
		write32(&qseed3->op_mode, 0x0);

		/* Pixel Extensions */
		write32(&sspp->sspp_sw_pix_ext_c0_lr, 0);
		write32(&sspp->sspp_sw_pix_ext_c0_tb, 0);
		write32(&sspp->sspp_sw_pic_ext_c0_req_pixels, src_size_val);

		write32(&sspp->sspp_sw_pix_ext_c1c2_lr, 0);
		write32(&sspp->sspp_sw_pix_ext_c1c2_tb, 0);
		write32(&sspp->sspp_sw_pic_ext_c1c2_req_pixels, src_size_val);

		write32(&sspp->sspp_sw_pix_ext_c3_lr, 0);
		write32(&sspp->sspp_sw_pix_ext_c3_tb, 0);
		write32(&sspp->sspp_sw_pic_ext_c3_req_pixels, src_size_val);

		write32(&sspp->sspp_src_op_mode, SW_PIX_EXT_OVERRIDE);

		/* Next pipe base (VIG_0 -> VIG_1) */
		current_base += 0x2000;
	}
}

void mdss_layer_mixer_setup(struct edid *edid)
{
	uint32_t full_w = edid->mode.ha;
	uint32_t full_h = edid->mode.va;
	bool dual = ((full_w > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ||
		(calculate_mode_mdp_clk(&edid->mode) > MDSS_MAX_MDP_CLK));
	uint32_t lm_w = dual ? (full_w / 2) : full_w;
	uint32_t lm_out_size = (full_h << 16) | (lm_w & 0xFFFF);

	write32(&mdp_layer_mixer->layer_out_size, lm_out_size);
	write32(&mdp_layer_mixer->layer_op_mode, 0x0);

	write32(&mdp_ctl_0->ctl_intf_master, INTF_ACTIVE_5);
	write32(&mdp_ctl_0->ctl_intf_active, INTF_ACTIVE_5);

	for (int i = 0; i < 6; i++) {
		write32(&mdp_layer_mixer->layer_blend[i].layer_blend_op, 0x100);
		write32(&mdp_layer_mixer->layer_blend[i].layer_blend_const_alpha, 0x00ff0000);
	}

	write32(&mdp_ctl_0->ctl_layer0, MDP_CTL_LAYER0_VIG0_STAGE3_CFG);
	write32(&mdp_ctl_0->ctl_fetch_pipe_active, FETCH_PIPE_VIG0_ACTIVE);

	if (dual) {
		struct mdp_layer_mixer_regs *lm1 =
			(struct mdp_layer_mixer_regs *)((uint8_t *)mdp_layer_mixer + 0x1000);

		write32(&lm1->layer_out_size, lm_out_size);
		write32(&lm1->layer_op_mode, LM_LAYER_OP_MODE_SPLIT_RIGHT);

		for (int i = 0; i < 6; i++) {
			write32(&lm1->layer_blend[i].layer_blend_op, 0x100);
			write32(&lm1->layer_blend[i].layer_blend_const_alpha, 0x00ff0000);
		}

		write32(&mdp_ctl_0->ctl_layer1, MDP_CTL_LAYER1_VIG1_STAGE3_CFG);

		write32(&mdp_ctl_0->ctl_fetch_pipe_active,
			FETCH_PIPE_VIG0_ACTIVE | FETCH_PIPE_VIG1_ACTIVE);

		write32(&mdp_ctl_0->ctl_merge_3d_active, 1);
		write32(&mdp_merge_3d_0->mode, 3);
	} else {
		write32(&mdp_ctl_0->ctl_merge_3d_active, 0);
		write32(&mdp_merge_3d_0->mode, 0);
	}
}
