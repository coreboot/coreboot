/*
* This file is part of the coreboot project.
*
* Copyright 2013 Google Inc.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

/* This code was originally generated using an i915tool program. It has been
 * improved by hand.
 */

#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/intel/gma/i915.h>
#include <arch/io.h>
#include <variant/mainboard.h>

/* this function will either be renamed or subsumed into ./gma.c:i915_lightup */
void runio(struct intel_dp *dp);

void runio(struct intel_dp *dp)
{
	u8 read_val;

	intel_dp_wait_panel_power_control(0xabcd0008);

	/* vbios spins at this point. Some haswell weirdness? */
	intel_dp_wait_panel_power_control(0xabcd0008);

	/* This should be a function like intel_panel_enable_backlight
	   However, we are not sure how the value 0x3a9 comes up.
	   It has to do something with PWM frequency */
	gtt_write(BLC_PWM_CPU_CTL,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL2,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL1,BLM_PCH_PWM_ENABLE);

	gtt_write(DEIIR,0x00008000);
	intel_dp_wait_reg(DEIIR, 0x00000000);

	gtt_write(DSPSTRIDE(dp->plane),dp->stride);

	intel_dp_sink_dpms(dp, 0);

	intel_dp_get_max_downspread(dp, &read_val);

	intel_dp_set_m_n_regs(dp);

	intel_dp_set_resolution(dp);

	gtt_write(PIPESRC(dp->pipe),dp->pipesrc);
	gtt_write(PIPECONF(dp->transcoder),0x00000000);
	gtt_write(PCH_TRANSCONF(dp->pipe),0x00000000);

	mainboard_set_port_clk_dp(dp);
	gtt_write(DSPSTRIDE(dp->plane),dp->stride);
	gtt_write(DSPCNTR(dp->plane),DISPLAY_PLANE_ENABLE|DISPPLANE_BGRX888);
	gtt_write(DEIIR,0x00000080);

	gtt_write(TRANS_DDI_FUNC_CTL_EDP,dp->flags);
	gtt_write(PIPECONF(dp->transcoder),PIPECONF_ENABLE|PIPECONF_DITHER_EN);

	intel_dp_wait_panel_power_control(0xabcd000a);

	/* what is this doing? Not sure yet. */
	intel_dp_i2c_write(dp, 0x0);
	intel_dp_i2c_read(dp, &read_val);
	intel_dp_i2c_write(dp, 0x04);
	intel_dp_i2c_read(dp, &read_val);
	intel_dp_i2c_write(dp, 0x7e);
	intel_dp_i2c_read(dp, &read_val);

	/* this needs to be a call to a function */
	gtt_write(DP_A, DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE | DP_SYNC_VS_HIGH |0x00000091);
	gtt_write(DP_TP_CTL(dp->port),DP_TP_CTL_ENABLE | DP_TP_CTL_ENHANCED_FRAME_ENABLE);
	gtt_write(DP_A, DP_PORT_EN | DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_ENHANCED_FRAMING | DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE | DP_SYNC_VS_HIGH |0x80040091);

	/* we may need to move these *after* power well power up and *before* PCH_PP_CONTROL in gma.c */
	gtt_write(PCH_PP_ON_DELAYS, PANEL_PORT_SELECT_LVDS |(/* PANEL_POWER_UP_DELAY_MASK */0x1 << 16)|(/* PANEL_LIGHT_ON_DELAY_MASK */0xa << 0)|0x0001000a);
	gtt_write(PCH_PP_ON_DELAYS, PANEL_PORT_SELECT_LVDS |(/* PANEL_POWER_UP_DELAY_MASK */0x7d0 << 16)|(/* PANEL_LIGHT_ON_DELAY_MASK */0xa << 0)|0x07d0000a);

	intel_dp_set_bw(dp);
	intel_dp_set_lane_count(dp);

	mainboard_train_link(dp);

	/* need a function: intel_ddi_set_tp or similar */
	gtt_write(DP_TP_CTL(dp->port),DP_TP_CTL_ENABLE | DP_TP_CTL_ENHANCED_FRAME_ENABLE | DP_TP_CTL_LINK_TRAIN_IDLE);
	gtt_write(DP_TP_CTL(dp->port),DP_TP_CTL_ENABLE | DP_TP_CTL_ENHANCED_FRAME_ENABLE | DP_TP_CTL_LINK_TRAIN_NORMAL);

	gtt_write(BLC_PWM_CPU_CTL,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL2,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL1,0x80000000);

	/* some of this is not needed. */
	gtt_write(DIGITAL_PORT_HOTPLUG_CNTRL, DIGITAL_PORTA_HOTPLUG_ENABLE );

	gtt_write(SDEIIR,0x00000000);
	gtt_write(DEIIR,0x00000000);
	gtt_write(DEIIR,0x00008000);
	intel_dp_wait_reg(DEIIR, 0x00000000);

	gtt_write(DSPSTRIDE(dp->plane),dp->stride);
	gtt_write(PIPESRC(dp->pipe),dp->pipesrc);

	gtt_write(DEIIR,0x00000080);
	intel_dp_wait_reg(DEIIR, 0x00000000);

	gtt_write(DSPSTRIDE(dp->plane),dp->stride);
	gtt_write(DSPCNTR(dp->plane),DISPLAY_PLANE_ENABLE | DISPPLANE_BGRX888);

	gtt_write(PCH_PP_CONTROL,EDP_BLC_ENABLE | PANEL_POWER_RESET | PANEL_POWER_ON);

	gtt_write(SDEIIR,0x00000000);
	gtt_write(SDEIIR,0x00000000);
	gtt_write(DEIIR,0x00000000);
}
