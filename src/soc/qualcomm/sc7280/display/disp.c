/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/display/mdssreg.h>

void enable_mdss_clk(void)
{
	mdss_clock_enable(GCC_DISP_AHB);

	// enable gdsc before enabling clocks.
	clock_enable_gdsc(MDSS_CORE_GDSC);

	mdss_clock_enable(GCC_DISP_HF_AXI);
	mdss_clock_enable(GCC_DISP_SF_AXI);
	mdss_clock_enable(MDSS_CLK_AHB);
	mdss_clock_configure(MDSS_CLK_MDP, 400 * MHz, 0, 0, 0, 0, 0);
	mdss_clock_enable(MDSS_CLK_MDP);
	mdss_clock_configure(MDSS_CLK_VSYNC, 0, 0, 0, 0, 0, 0);
	mdss_clock_enable(MDSS_CLK_VSYNC);
}

void mdss_intf_tg_setup(struct edid *edid)
{
	uint32_t hsync_period, vsync_period;
	uint32_t active_vstart, active_vend, active_hctl;
	uint32_t display_hctl, hsync_ctl, display_vstart, display_vend;

	hsync_period = edid->mode.ha + edid->mode.hbl;
	vsync_period = edid->mode.va + edid->mode.vbl;
	display_vstart = edid->mode.vbl * hsync_period + edid->mode.hbl;
	display_vend = (vsync_period * hsync_period) - 1;
	hsync_ctl = (hsync_period << 16) | edid->mode.hspw;
	display_hctl = edid->mode.hbl | (hsync_period - 1) << 16;
	active_vstart = edid->mode.vbl * hsync_period;
	active_vend = display_vend;
	active_hctl = display_hctl;

	write32(&mdp_intf->intf_active_v_start_f0, active_vstart);
	write32(&mdp_intf->intf_active_v_end_f0, active_vend);
	write32(&mdp_intf->intf_active_hctl, active_hctl);
	write32(&mdp_intf->display_data_hctl, display_hctl);
	write32(&mdp_intf->intf_hsync_ctl, hsync_ctl);
	write32(&mdp_intf->intf_vysnc_period_f0, vsync_period * hsync_period);
	write32(&mdp_intf->intf_vysnc_pulse_width_f0, edid->mode.vspw * hsync_period);
	write32(&mdp_intf->intf_disp_hctl, display_hctl);
	write32(&mdp_intf->intf_disp_v_start_f0, display_vstart);
	write32(&mdp_intf->intf_disp_v_end_f0, display_vend);
	write32(&mdp_intf->intf_underflow_color, 0x00);
}

void mdss_ctrl_config(void)
{
	/* Select vigo pipe active */
	write32(&mdp_ctl->ctl_fetch_pipe_active, FETCH_PIPE_VIG0_ACTIVE);

	/* PPB0 to INTF1 */
	write32(&mdp_ctl->ctl_intf_active, INTF_ACTIVE_5);

}
