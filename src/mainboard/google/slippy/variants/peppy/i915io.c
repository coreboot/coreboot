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

#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/intel/gma/i915.h>
#include <arch/io.h>
#include <variant/mainboard.h>

void runio(struct intel_dp *dp,  int verbose);
void runio(struct intel_dp *dp,  int verbose)
{
	u8 read_val;
	gtt_write(DDI_BUF_CTL_A,
		  DDI_BUF_IS_IDLE|DDI_A_4_LANES|DDI_PORT_WIDTH_X1|DDI_INIT_DISPLAY_DETECTED
		  |0x00000091);

	intel_prepare_ddi();

	gtt_write(BLC_PWM_CPU_CTL,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL2,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL1,0x80000000);

	gtt_write(DEIIR,0x00008000);
	intel_dp_wait_reg(DEIIR, 0x00000000);
	gtt_write(DSPSTRIDE(dp->plane), dp->stride);
	gtt_write(DSPADDR(dp->plane), 0x00000000);

	printk(BIOS_SPEW, "DP_SET_POWER");

	intel_dp_sink_dpms(dp, 0);

	intel_dp_set_m_n_regs(dp);

	intel_dp_get_max_downspread(dp, &read_val);

	intel_dp_set_resolution(dp);

	gtt_write(PIPESRC(dp->pipe),dp->pipesrc);
        gtt_write(PIPECONF(dp->transcoder),0x00000000);
        gtt_write(PCH_TRANSCONF(dp->pipe),0x00000000);

	mainboard_set_port_clk_dp(dp);

	gtt_write(DSPSTRIDE(dp->plane),dp->stride);
        gtt_write(DSPCNTR(dp->plane),DISPLAY_PLANE_ENABLE|DISPPLANE_BGRX888);

	gtt_write(DEIIR,0x00000080);
	intel_dp_wait_reg(DEIIR, 0x00000000);

	/* There is some reason we removed these three calls from
	 * slippy/gma.c -- I dont remember why!! */
	gtt_write(PF_WIN_POS(dp->pipe),dp->pfa_pos);
	gtt_write(PF_CTL(dp->pipe),dp->pfa_ctl);
	gtt_write(PF_WIN_SZ(dp->pipe),dp->pfa_sz);

	gtt_write(TRANS_DDI_FUNC_CTL_EDP,dp->flags);
        gtt_write(PIPECONF(dp->transcoder),PIPECONF_ENABLE|PIPECONF_DITHER_EN);

	/* what is this doing? Not sure yet. But we don't seem to be
	 * able to live without it.*/
	intel_dp_i2c_write(dp, 0x0);
	intel_dp_i2c_read(dp, &read_val);
	intel_dp_i2c_write(dp, 0x04);
	intel_dp_i2c_read(dp, &read_val);
	intel_dp_i2c_write(dp, 0x7e);
	intel_dp_i2c_read(dp, &read_val);

	gtt_write(DDI_BUF_CTL_A,
		  DDI_BUF_IS_IDLE|
		  DDI_A_4_LANES|DDI_PORT_WIDTH_X1|DDI_INIT_DISPLAY_DETECTED
		  |0x00000091);

	gtt_write(TRANS_DDI_FUNC_CTL_EDP+0x10,0x00000001);
	gtt_write(DP_TP_CTL(dp->port),DP_TP_CTL_ENABLE |
		  DP_TP_CTL_ENHANCED_FRAME_ENABLE);

	gtt_write(DDI_BUF_CTL_A,
		  DDI_BUF_CTL_ENABLE|
		  /* another undocumented setting. Surprised? */ 0x40000 |
		  DDI_BUF_IS_IDLE|DDI_A_4_LANES|
		  DDI_PORT_WIDTH_X1|DDI_INIT_DISPLAY_DETECTED|
		  0x80040091);

	intel_dp_set_bw(dp);

	intel_dp_set_lane_count(dp);

	mainboard_train_link(dp);

	gtt_write(DP_TP_CTL(dp->port),
		  DP_TP_CTL_ENABLE | DP_TP_CTL_ENHANCED_FRAME_ENABLE |
		  DP_TP_CTL_LINK_TRAIN_IDLE);

	gtt_write(DP_TP_CTL(dp->port),
		  DP_TP_CTL_ENABLE | DP_TP_CTL_ENHANCED_FRAME_ENABLE |
		  DP_TP_CTL_LINK_TRAIN_NORMAL);

	gtt_write(BLC_PWM_CPU_CTL,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL2,0x03a903a9);
	gtt_write(BLC_PWM_PCH_CTL1,0x80000000);

	/* some of this is not needed. But with a total lack of docs, well ...*/
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

	gtt_write(PCH_PP_CONTROL,EDP_BLC_ENABLE | EDP_BLC_ENABLE | PANEL_POWER_ON);

	gtt_write(SDEIIR,0x00000000);
	gtt_write(SDEIIR,0x00000000);
	gtt_write(DEIIR,0x00000000);

}
