/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>
#include <console/console.h>
#include <soc/display/edp_reg.h>
#include <soc/qcom_spmi.h>
#include <gpio.h>

#define MDSS_MDP_MAX_PREFILL_FETCH 24

void intf_tg_setup(struct edid *edid, const struct mdss_panel_config *cfg)
{
	const struct edid_mode *m;
	uint32_t active_ha, h_total, h_start, h_end, h_data_end;
	uint32_t v_total, v_start, v_pw;

	if (!edid)
		return;

	m = &edid->mode;
	if (!m->ha || !m->va)
		return;

	v_total = m->va + m->vbl;
	v_pw    = m->vspw;

	if (cfg->dsc_ratio_x1000 > 0) {
		/*
		 * DSC path: INTF sees compressed active width per interface.
		 * For 2-2-2: each INTF sees ha/dsi_count pixels.
		 * h_total = compressed(ha/dsi_count) + hbl
		 * VSYNC_PERIOD uses h_total*2 (pixel clock domain).
		 */
		u32 ha_per_intf = m->ha / cfg->dsi_count;
		active_ha  = mdss_dsc_compressed_ha(ha_per_intf, cfg->dsc_ratio_x1000);
		h_total    = active_ha + m->hbl;
		h_start    = m->hspw + (m->hbl - m->hso - m->hspw);
		h_end      = h_start + active_ha - 1;
		h_data_end = h_start + active_ha / 2 - 1;
		v_start    = v_pw + (m->vbl - m->vso - v_pw);

		write32(&mdp_intf->intf_mux, 0xF0000);
		write32(&mdp_intf->intf_hsync_ctl, (h_total << 16) | m->hspw);
		/*
		 * VSYNC_PERIOD = v_total_dsi * h_total (UEFI formula).
		 * DSI operates in h_total*2 pixel clock domain for DSC:
		 *   v_total_dsi = (va+vbl) * dsi_count * 2
		 * For 2-2-1: v_total_dsi = 2370*1*2=4740, h_total=504 -> period=2388960
		 * For 2-2-2: v_total_dsi = 2270*2*2=9080, h_total=840 -> period=7627200=0x7461C0
		 * Source: UEFI hal_mdp_interface.c + HLOS golden dump
		 */
		u32 v_total_dsi = v_total * cfg->dsi_count * 2;
		u32 vsync_period = v_total_dsi * h_total;
		write32(&mdp_intf->intf_vysnc_period_f0, vsync_period);
		write32(&mdp_intf->intf_vysnc_pulse_width_f0, v_pw * h_total);
		write32(&mdp_intf->intf_disp_v_start_f0, v_start * h_total);
		write32(&mdp_intf->intf_disp_v_end_f0,
			(v_start + m->va - 1) * h_total + h_total - 1);
		write32(&mdp_intf->intf_active_v_start_f0, v_start * h_total);
		write32(&mdp_intf->intf_active_v_end_f0,
			(v_start + m->va - 1) * h_total + h_total - 1);
		write32(&mdp_intf->intf_disp_hctl,    (h_end << 16) | h_start);
		write32(&mdp_intf->intf_active_hctl,  (h_end << 16) | h_start);
		write32(&mdp_intf->display_data_hctl, (h_data_end << 16) | h_start);
		write32(&mdp_intf->intf_underflow_color, 0xFF);
		write32(&mdp_intf->polarity_ctl,
			(m->phsync ? 0x1 : 0x0) | (m->pvsync ? 0x2 : 0x0));
		write32(&mdp_intf->intf_panel_format, 0x2100);
		write32(&mdp_intf->intf_config2,
			INTF_CONFIG2_DCE_DATA_COMPRESS |
			INTF_CONFIG2_DATA_HCTL_EN |
			INTF_CONFIG2_DATABUS_WIDEN);

		/* Dual DSI (2-2-2): mirror all INTF_1 timing to INTF_2 */
		if (cfg->dsi_count > 1) {
			/*
			 * INTF_2_MUX: SEL=1 (PP1) for INTF_2, SPLIT_SEL=0xF (disabled)
			 * UEFI: SEL = ePingPongId[0] - HAL_MDP_PINGPONG_0
			 * INTF_1->PP0->SEL=0->0x0F0000, INTF_2->PP1->SEL=1->0x0F0001
			 */
			write32(&mdp_intf2->intf_mux, 0x0F0001);
			write32(&mdp_intf2->intf_hsync_ctl, (h_total << 16) | m->hspw);
			write32(&mdp_intf2->intf_vysnc_period_f0, vsync_period);
			write32(&mdp_intf2->intf_vysnc_pulse_width_f0, v_pw * h_total);
			write32(&mdp_intf2->intf_disp_v_start_f0, v_start * h_total);
			write32(&mdp_intf2->intf_disp_v_end_f0,
				(v_start + m->va - 1) * h_total + h_total - 1);
			write32(&mdp_intf2->intf_active_v_start_f0, v_start * h_total);
			write32(&mdp_intf2->intf_active_v_end_f0,
				(v_start + m->va - 1) * h_total + h_total - 1);
			write32(&mdp_intf2->intf_disp_hctl,    (h_end << 16) | h_start);
			write32(&mdp_intf2->intf_active_hctl,  (h_end << 16) | h_start);
			write32(&mdp_intf2->display_data_hctl, (h_data_end << 16) | h_start);
			write32(&mdp_intf2->intf_underflow_color, 0xFF);
			write32(&mdp_intf2->polarity_ctl,
				(m->phsync ? 0x1 : 0x0) | (m->pvsync ? 0x2 : 0x0));
			write32(&mdp_intf2->intf_panel_format, 0x2100);
			write32(&mdp_intf2->intf_config2,
				INTF_CONFIG2_DCE_DATA_COMPRESS |
				INTF_CONFIG2_DATA_HCTL_EN |
				INTF_CONFIG2_DATABUS_WIDEN);
		}
	} else {
		/* eDP path: no DSC, wide_bus=2 divides h_total */
		uint32_t wide_bus = 2;
		uint32_t full_h_total = m->ha + m->hbl;
		h_total    = full_h_total / wide_bus;
		h_start    = m->hbl / wide_bus;
		uint32_t h_pw = m->hspw / wide_bus;
		h_end      = h_total - 1;
		v_start    = m->vbl;

		write32(&mdp_edp_intf->intf_mux, 0xF0000);
		write32(&mdp_edp_intf->intf_hsync_ctl, (h_total << 16) | h_pw);
		write32(&mdp_edp_intf->intf_vysnc_period_f0, v_total * h_total);
		write32(&mdp_edp_intf->intf_vysnc_pulse_width_f0, v_pw * h_total);
		write32(&mdp_edp_intf->intf_disp_v_start_f0, v_start * h_total);
		write32(&mdp_edp_intf->intf_disp_v_end_f0,
			(uint32_t)(((uint64_t)v_total * (uint64_t)h_total) - 1ULL));
		uint32_t disp_hctl = (h_end << 16) | h_start;
		write32(&mdp_edp_intf->intf_disp_hctl,    disp_hctl);
		write32(&mdp_edp_intf->display_data_hctl, disp_hctl);
		write32(&mdp_edp_intf->polarity_ctl,
			(m->phsync ? 0x1 : 0x0) | (m->pvsync ? 0x2 : 0x0));
		write32(&mdp_edp_intf->intf_panel_format, 0x2100);
		write32(&mdp_edp_intf->intf_prof_fetch_start, 0);
	}
}

/*
 * intf_fetch_start_config() - computed fetch start.
 * DSC path: formula from purwa UEFI MDPConfig.c InterfaceFetchStartConfig().
 * eDP path: prefill-based formula (original eDP implementation).
 */
void intf_fetch_start_config(struct edid *edid, const struct mdss_panel_config *cfg)
{
	uint32_t fetch_start = 0;

	if (cfg->dsc_ratio_x1000 > 0)
		return;

	/* eDP path: prefill-based formula */
	uint32_t wide_bus = 2;
	uint32_t v_total  = edid->mode.va + edid->mode.vbl;
	uint32_t vfp_start = edid->mode.va + edid->mode.vbl - edid->mode.vso;
	uint32_t prefetch_avail = v_total - vfp_start;
	uint32_t fetch_enable = PROG_FETCH_START_EN;
	if (((edid->mode.ha + edid->mode.hbl) % wide_bus) != 0)
		return;
	uint32_t h_total = (edid->mode.ha + edid->mode.hbl) / wide_bus;

	if (prefetch_avail >= MDSS_MDP_MAX_PREFILL_FETCH) {
		fetch_start = 0;
		fetch_enable = 0;
	} else {
		uint32_t prefetch_needed = MDSS_MDP_MAX_PREFILL_FETCH;
		fetch_start = (v_total - prefetch_needed) * h_total + h_total + 1;
		fetch_enable = PROG_FETCH_START_EN;
	}

	write32(&mdp_edp_intf->intf_prof_fetch_start, fetch_start);
	write32(&mdp_edp_intf->intf_config, fetch_enable);
}

void merge_3d_active(struct edid *edid, const struct mdss_panel_config *cfg)
{
	bool dual = (cfg->pipe_count > 1);

	if (dual)
		write32(&mdp_ctl_0->merge_3d_flush, 0x1);

	/*
	 * MDSS_INTF_FLUSH includes all active interfaces:
	 *   2-2-1/eDP: BIT(intf_num)
	 *   2-2-2:     BIT(intf_num) | BIT(intf_num+1)
	 */
	write32(&mdp_ctl_0->ctl_intf_flush, MDSS_INTF_FLUSH(cfg));
	write32(&mdp_ctl_0->periph_flush,   MDSS_INTF_FLUSH(cfg));

	u32 flush_mask = FLUSH_INTF | FLUSH_PERIPH | FLUSH_CTL | FLUSH_LM0 | FLUSH_VIG0;

	if (dual) {
		flush_mask |= FLUSH_LM1 | FLUSH_VIG1;
		if (cfg->dsc_ratio_x1000 == 0)
			flush_mask |= FLUSH_MERGE_3D;
	}

	if (mdp_ctl_0->dsc_active)
		flush_mask |= FLUSH_DSC;

	write32(&mdp_ctl_0->ctl_flush, flush_mask);
}
