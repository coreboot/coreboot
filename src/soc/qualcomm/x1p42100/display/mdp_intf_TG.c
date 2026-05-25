/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/display/edp_reg.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>

#define MDSS_MDP_MAX_PREFILL_FETCH 24

void intf_tg_setup(struct edid *edid)
{
	const struct edid_mode *m;
	uint32_t full_h_total, full_h_start, full_h_pw;
	uint32_t h_total, h_start, h_pw, h_end;
	uint32_t v_total, v_start, v_pw;

	/* 2 because of Databus widen, 2 pixels per clock.
	HSYNC Period would be divided by 2 */
	uint32_t wide_bus = 2;

	if (!edid)
		return;

	m = &edid->mode;
	if (!m->ha || !m->va)
		return;

	/* Full (non-split) timing from EDID */
	full_h_total = m->ha + m->hbl;
	full_h_start = m->hbl;
	full_h_pw = m->hspw;

	v_total = m->va + m->vbl;
	v_start = m->vbl;
	v_pw = m->vspw;

	/* Per-pipe horizontal timing */
	h_total = full_h_total / wide_bus;
	h_start = full_h_start / wide_bus;
	h_pw = full_h_pw / wide_bus;
	h_end = h_total - 1;

	write32(&mdp_intf->intf_mux, 0xF0000); /* use ping_pong 0 & disable split */

	write32(&mdp_intf->intf_hsync_ctl, (h_total << 16) | (h_pw));

	write32(&mdp_intf->intf_vysnc_period_f0, v_total * h_total);

	write32(&mdp_intf->intf_vysnc_pulse_width_f0, v_pw * h_total);

	write32(&mdp_intf->intf_disp_v_start_f0, v_start * h_total);

	write32(&mdp_intf->intf_disp_v_end_f0,
		(uint32_t)(((uint64_t)v_total * (uint64_t)h_total) - 1ULL));

	/* same packed value written to both regs (as before) */
	uint32_t disp_hctl = (h_end << 16) | (h_start);
	write32(&mdp_intf->intf_disp_hctl, disp_hctl);
	write32(&mdp_intf->display_data_hctl, disp_hctl);

	write32(&mdp_intf->polarity_ctl, (m->phsync ? 0x1 : 0x0) | (m->pvsync ? 0x2 : 0x0));

	write32(&mdp_intf->intf_panel_format, 0x2100); /* Color Format : RGB */
	write32(&mdp_intf->intf_prof_fetch_start, 0);
}

void intf_fetch_start_config(struct edid *edid)
{
	uint32_t v_total, h_total, fetch_start, vfp_start;
	uint32_t prefetch_avail, prefetch_needed;
	uint32_t fetch_enable = PROG_FETCH_START_EN;

	/* 2 because of Databus widen, 2 pixels per clock.
	HSYNC Period would be divided by 2 */
	uint32_t wide_bus = 2;

	v_total = edid->mode.va + edid->mode.vbl;

	/* Per-pipe horizontal total (match TG programming) */
	if (((edid->mode.ha + edid->mode.hbl) % wide_bus) != 0)
		return;
	h_total = (edid->mode.ha + edid->mode.hbl) / wide_bus;

	vfp_start = edid->mode.va + edid->mode.vbl - edid->mode.vso;

	prefetch_avail = v_total - vfp_start;

	if (prefetch_avail >= MDSS_MDP_MAX_PREFILL_FETCH) {
		fetch_start = 0;
		fetch_enable = 0;
	} else {
		prefetch_needed = MDSS_MDP_MAX_PREFILL_FETCH;
		fetch_start = (v_total - prefetch_needed) * h_total + h_total + 1;
		fetch_enable = PROG_FETCH_START_EN;
	}

	write32(&mdp_intf->intf_prof_fetch_start, fetch_start);
	write32(&mdp_intf->intf_config, fetch_enable);
}

void merge_3d_active(struct edid *edid)
{
	bool dual = (edid->mode.ha > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ||
		(calculate_mode_mdp_clk(&edid->mode) > MDSS_MAX_MDP_CLK);

	if (dual)
		write32(&mdp_ctl_0->merge_3d_flush, 0x1);

	write32(&mdp_ctl_0->ctl_intf_flush, 0x20);
	write32(&mdp_ctl_0->periph_flush, 0x20);

	u32 flush_mask = FLUSH_INTF | FLUSH_PERIPH | FLUSH_CTL | FLUSH_LM0 | FLUSH_VIG0;

	if (dual)
		flush_mask |= FLUSH_MERGE_3D | FLUSH_LM1 | FLUSH_VIG1;

	write32(&mdp_ctl_0->ctl_flush, flush_mask);

	write32(&edp_lclk->vsc_db16_db17_db18_pb8, 0x10100);
	write32(&edp_lclk->compression_mode_ctrl, 0x2800);
	write32(&mdp_intf->intf_config2, 0x111);
	write32(&edp_lclk->db_ctrl, 0x01);
	write32(&mdp_intf->intf_config, 0x800000);
}
