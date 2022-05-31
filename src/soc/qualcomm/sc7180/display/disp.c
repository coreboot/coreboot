/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/display/mdssreg.h>

void mdss_intf_tg_setup(struct edid *edid)
{
	uint32_t hsync_period, vsync_period, hsync_start_x, hsync_end_x;
	uint32_t display_hctl, hsync_ctl, display_vstart, display_vend;
	uint32_t mdss_version;

	mdss_version = read32(&mdss_hw->hw_version);
	hsync_period = edid->mode.ha + edid->mode.hbl;
	vsync_period = edid->mode.va + edid->mode.vbl;
	hsync_start_x = edid->mode.hbl - edid->mode.hso;
	hsync_end_x = hsync_period - edid->mode.hso - 1;
	display_vstart = (edid->mode.vbl - edid->mode.vso) * hsync_period;
	display_vend = ((vsync_period - edid->mode.vso) * hsync_period) - 1;
	hsync_ctl = (hsync_period << 16) | edid->mode.hspw;
	display_hctl = (hsync_end_x << 16) | hsync_start_x;

	write32(&mdp_intf->intf_hsync_ctl, hsync_ctl);
	write32(&mdp_intf->intf_vysnc_period_f0, vsync_period * hsync_period);
	write32(&mdp_intf->intf_vysnc_pulse_width_f0, edid->mode.vspw * hsync_period);
	write32(&mdp_intf->intf_disp_hctl, display_hctl);
	write32(&mdp_intf->intf_disp_v_start_f0, display_vstart);
	write32(&mdp_intf->intf_disp_v_end_f0, display_vend);
	write32(&mdp_intf->intf_underflow_color, 0x00);
	write32(&mdp_intf->intf_panel_format, 0x2100);
}

void mdss_ctrl_config(void)
{
	/* Select Video Mode Interface */
	write32(&mdp_ctl->ctl_top, 0x0);

	/* PPB0 to INTF1 */
	write32(&mdp_ctl->ctl_intf_active, INTF_ACTIVE_1);
}
