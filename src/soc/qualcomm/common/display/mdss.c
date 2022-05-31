/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>

#define MDSS_MDP_MAX_PREFILL_FETCH 24

static void mdss_source_pipe_config(struct edid *edid)
{
	uint32_t img_size, out_size, stride;
	uint32_t fb_off = 0;
	uint32_t flip_bits = 0;
	uint32_t src_xy = 0;
	uint32_t dst_xy = 0;

	/* write active region size*/
	img_size = (edid->mode.va << 16) | edid->mode.ha;
	out_size = img_size;
	stride = (edid->mode.ha * edid->framebuffer_bits_per_pixel/8);

	if (!fb_off) {	/* left */
		dst_xy = (edid->mode.vborder << 16) | edid->mode.hborder;
		src_xy = dst_xy;
	} else {	/* right */
		dst_xy = (edid->mode.vborder << 16);
		src_xy = (edid->mode.vborder << 16) | fb_off;
	}

	printk(BIOS_INFO, "%s: src=%x fb_off=%x src_xy=%x dst_xy=%x\n",
		   __func__, out_size, fb_off, src_xy, dst_xy);

	write32(&mdp_sspp->sspp_src_ystride0, stride);
	write32(&mdp_sspp->sspp_src_size, out_size);
	write32(&mdp_sspp->sspp_out_size, out_size);
	write32(&mdp_sspp->sspp_src_xy, src_xy);
	write32(&mdp_sspp->sspp_out_xy, dst_xy);

	/* Tight Packing 4bpp Alpha 8-bit A R B G */
	write32(&mdp_sspp->sspp_src_format, 0x000236ff);
	write32(&mdp_sspp->sspp_src_unpack_pattern, 0x03020001);

	flip_bits |= SW_PIX_EXT_OVERRIDE;
	write32(&mdp_sspp->sspp_sw_pic_ext_c0_req_pixels, out_size);
	write32(&mdp_sspp->sspp_sw_pic_ext_c1c2_req_pixels, out_size);
	write32(&mdp_sspp->sspp_sw_pic_ext_c3_req_pixels, out_size);
	write32(&mdp_sspp->sspp_src_op_mode, flip_bits);
}

static void mdss_vbif_setup(void)
{
	write32(&vbif_rt->vbif_out_axi_amemtype_conf0, 0x33333333);
	write32(&vbif_rt->vbif_out_axi_amemtype_conf1, 0x00333333);
}

static void mdss_intf_fetch_start_config(struct edid *edid)
{
	uint32_t v_total, h_total, fetch_start, vfp_start;
	uint32_t prefetch_avail, prefetch_needed;
	uint32_t fetch_enable = PROG_FETCH_START_EN;

	/*
	 * MDP programmable fetch is for MDP with rev >= 1.05.
	 * Programmable fetch is not needed if vertical back porch
	 * plus vertical pulse width plus extra line for the extra h_total
	 * added during fetch start is >= 24.
	 */
	if ((edid->mode.vbl - edid->mode.vso + 1) >= MDSS_MDP_MAX_PREFILL_FETCH)
		return;

	/*
	 * Fetch should always be outside the active lines. If the fetching
	 * is programmed within active region, hardware behavior is unknown.
	 */
	v_total = edid->mode.va + edid->mode.vbl;
	h_total = edid->mode.ha + edid->mode.hbl;
	vfp_start = edid->mode.va + edid->mode.vbl - edid->mode.vso;
	prefetch_avail = v_total - vfp_start;
	prefetch_needed = MDSS_MDP_MAX_PREFILL_FETCH - edid->mode.vbl + edid->mode.vso;

	/*
	 * In some cases, vertical front porch is too high. In such cases limit
	 * the mdp fetch lines  as the last (25 - vbp - vpw) lines of
	 * vertical front porch.
	 */
	if (prefetch_avail > prefetch_needed)
		prefetch_avail = prefetch_needed;

	fetch_start = (v_total - prefetch_avail) * h_total + h_total + 1;
	write32(&mdp_intf->intf_prof_fetch_start, fetch_start);
	write32(&mdp_intf->intf_config, fetch_enable);
}

static void mdss_layer_mixer_setup(struct edid *edid)
{
	uint32_t mdp_rgb_size;
	uint32_t left_staging_level;

	/* write active region size*/
	mdp_rgb_size = (edid->mode.va << 16) | edid->mode.ha;

	write32(&mdp_layer_mixer->layer_out_size, mdp_rgb_size);
	write32(&mdp_layer_mixer->layer_op_mode, 0x0);
	for (int i = 0; i < 6; i++) {
		write32(&mdp_layer_mixer->layer_blend[i].layer_blend_op, 0x100);
		write32(&mdp_layer_mixer->layer_blend[i].layer_blend_const_alpha, 0x00ff0000);
	}

	/* Enable border fill */
	left_staging_level = BORDER_OUT;
	left_staging_level |= VIG_0_OUT;

	/* Base layer for layer mixer 0 */
	write32(&mdp_ctl->ctl_layer0, left_staging_level);
}

static void mdss_vbif_qos_remapper_setup(void)
{
	/*
	 * VBIF remapper registers are used for translating internal display hardware
	 * priority level (from 0 to 7) into system fabric priority level.
	 * These remapper settings are defined for all the clients which corresponds
	 * to the xin clients connected to SSPP on VBIF.
	 */
	write32(&vbif_rt->qos_rp_remap[0].vbif_xinl_qos_rp_remap, 0x00000003);
	write32(&vbif_rt->qos_rp_remap[1].vbif_xinl_qos_rp_remap, 0x11111113);
	write32(&vbif_rt->qos_rp_remap[2].vbif_xinl_qos_rp_remap, 0x22222224);
	write32(&vbif_rt->qos_rp_remap[3].vbif_xinl_qos_rp_remap, 0x33333334);
	write32(&vbif_rt->qos_rp_remap[4].vbif_xinl_qos_rp_remap, 0x44444445);
	write32(&vbif_rt->qos_rp_remap[7].vbif_xinl_qos_rp_remap, 0x77777776);
	write32(&vbif_rt->qos_lvl_remap[0].vbif_xinl_qos_lvl_remap, 0x00000003);
	write32(&vbif_rt->qos_lvl_remap[1].vbif_xinl_qos_lvl_remap, 0x11111113);
	write32(&vbif_rt->qos_lvl_remap[2].vbif_xinl_qos_lvl_remap, 0x22222224);
	write32(&vbif_rt->qos_lvl_remap[3].vbif_xinl_qos_lvl_remap, 0x33333334);
	write32(&vbif_rt->qos_lvl_remap[4].vbif_xinl_qos_lvl_remap, 0x44444445);
	write32(&vbif_rt->qos_lvl_remap[5].vbif_xinl_qos_lvl_remap, 0x77777776);
}

void mdp_dsi_video_config(struct edid *edid)
{
	mdss_intf_tg_setup(edid);
	mdss_intf_fetch_start_config(edid);
	mdss_vbif_setup();
	mdss_vbif_qos_remapper_setup();
	mdss_source_pipe_config(edid);
	mdss_layer_mixer_setup(edid);
	mdss_ctrl_config();
	write32(&mdp_intf->intf_mux, 0x0F0000);
}

void mdp_dsi_video_on(void)
{
	uint32_t ctl0_reg_val;

	ctl0_reg_val = VIG_0 | LAYER_MIXER_0 | CTL | INTF;
	write32(&mdp_ctl->ctl_intf_flush, INTF_FLUSH);
	write32(&mdp_ctl->ctl_flush, ctl0_reg_val);
}
