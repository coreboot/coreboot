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
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

/* This code was originally generated using an i915tool program. It has been
 * improved by hand.
 */

#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include <drivers/intel/gma/i915.h>
#include <arch/io.h>

/* these variables will be removed when the proper support is finished in src/drivers/intel/gma/intel_dp.c */
int index;
u32 auxout;
u8 auxin[20];
u8 msg[32];

/* this function will either be renamed or subsumed into ./gma.c:i915_lightup */
void runio(struct intel_dp *dp);

void runio(struct intel_dp *dp)
{
	intel_dp_wait_panel_power_control(0xabcd0008);

	/* vbios spins at this point. Some haswell weirdness? */
	intel_dp_wait_panel_power_control(0xabcd0008);

	/* This is stuff we don't totally understand yet. */
	io_i915_write32(0x03a903a9,BLC_PWM_CPU_CTL);
	io_i915_write32(0x03a903a9,BLC_PWM_PCH_CTL2);
	io_i915_write32(0x80000000,BLC_PWM_PCH_CTL1);
	io_i915_write32(0x00ffffff,0x64ea8);
	io_i915_write32(0x00040006,0x64eac);
	io_i915_write32( PORTD_HOTPLUG_ENABLE | PORTB_HOTPLUG_ENABLE |0x10100010,SDEISR+0x30);
	io_i915_write32(0x0000020c,0x4f054);
	intel_dp_wait_reg(0x0004f054, 0x0000020c);
	io_i915_write32(0x00000000,0x4f008);
	io_i915_write32(0x0000020c,0x4f054);
	intel_dp_wait_reg(0x0004f054, 0x0000020c);
	io_i915_write32(0x00000000,0x4f044);
	intel_dp_wait_reg(0x0004f044, 0x00000000);
	io_i915_write32(0x00000400,0x4f044);
	intel_dp_wait_reg(0x0004f044, 0x00000400);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x01000008,0x4f040);
	io_i915_write32(0x00000008,0x4f05c);
	io_i915_write32(0x00000008,0x4f060);
	io_i915_write32(0x80000000,0x45400);
	intel_dp_wait_reg(0x00045400, 0xc0000000);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x45450000,0x4f04c);
	io_i915_write32(0x45450000,0x4f04c);
	io_i915_write32(0x03000400,0x4f000);
	io_i915_write32(0x8000298e,CPU_VGACNTRL);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,_CURACNTR);
	io_i915_write32(0x00000000,_CURABASE);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x00000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,_CURBCNTR_IVB);
	io_i915_write32(0x00000000,_CURBBASE_IVB);
	io_i915_write32(0x00000000,_DSPBCNTR);
	io_i915_write32(0x00000000,_DSPBSURF);
	io_i915_write32(0x00000000,0x72080);
	io_i915_write32(0x00000000,0x72084);
	io_i915_write32(0x00000000,_DVSACNTR);
	io_i915_write32(0x00000000,_DVSASURF);
	io_i915_write32(0x00008000,DEIIR);
	intel_dp_wait_reg(0x00044008, 0x00000000);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000600,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x01000008,0x4f040);
	io_i915_write32(0x00000008,0x4f05c);
	io_i915_write32(0x00000008,0x4f060);
	io_i915_write32(0x8020298e,CPU_VGACNTRL);
	io_i915_write32(0x00000000,0x4f044);
	intel_dp_wait_reg(0x0004f044, 0x00000000);
	io_i915_write32(/*0x00000800*/dp->stride,_DSPASTRIDE);
	io_i915_write32(0x00000000,_DSPAADDR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000000,0x4f044);

	intel_dp_sink_dpms(dp, 0);

	io_i915_write32(0x00000001,0x4f008);
	io_i915_write32(0x00000012,0x4f014);
	/* replace with a function: unsigned int intel_dp_get_max_downspread(dp); */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_MAX_DOWNSPREAD<<8|0x0|0x90000300;
printk(BIOS_SPEW, "DP_MAX_DOWNSPREAD");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* undocumented. */
	io_i915_write32(0x7e4a0000,0x6f030);
	/* io_i915_write32(0x00800000,0x6f034); */
	/* Write to 0x6f030 has to be 0x7e4ayyyy -- First four hex digits are important.
	   However, with our formula we always see values 0x7e43yyyy (1366 panel) and
	   0x7e42yyy (1280 panel) */
	/* io_i915_write32(TU_SIZE(dp->m_n.tu) | dp->m_n.gmch_m,0x6f030); */
	io_i915_write32(dp->m_n.gmch_n,0x6f034);
	io_i915_write32(dp->m_n.link_m,0x6f040);
	io_i915_write32(dp->m_n.link_n,0x6f044);

	/* leave as is for now. */
	io_i915_write32(dp->htotal,0x6f000);
	io_i915_write32(dp->hblank,0x6f004);
	io_i915_write32(dp->hsync,0x6f008);
	io_i915_write32(dp->vtotal,0x6f00c);
	io_i915_write32(dp->vblank,0x6f010);
	io_i915_write32(dp->vsync,0x6f014);
	io_i915_write32(dp->pipesrc,_PIPEASRC);
	io_i915_write32(0x00000000,0x7f008);
	io_i915_write32(0x00000000,_TRANSACONF);

	io_i915_write32(0x20000000,PORT_CLK_SEL_A);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x14000000,_DSPACNTR);
	io_i915_write32(dp->stride,_DSPASTRIDE);

	io_i915_write32(0x00000000,_DSPAADDR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x94000000,_DSPACNTR);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x94000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000080,DEIIR);
	intel_dp_wait_reg(0x00044008, 0x00000000);

	io_i915_write32(0x00230000,TRANS_DDI_FUNC_CTL_EDP);
	io_i915_write32(0x00000010,0x7f008);
	io_i915_write32(dp->flags,TRANS_DDI_FUNC_CTL_EDP);
	io_i915_write32(0x80000010,0x7f008);

	intel_dp_wait_panel_power_control(0xabcd000a);

	/* what is this doing? Not sure yet. */
	/* each block here needs to be a call to a function */
	auxout = 0<<31 /* i2c */|1<<30|0x0<<28/*W*/|0x50<<8|0x0|0x40005000;
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x00000000;
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	auxout = 0<<31 /* i2c */|0<<30|0x1<<28/*R*/|0x50<<8|0x3|0x10005003;
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 3);

	auxout = 0<<31 /* i2c */|1<<30|0x0<<28/*W*/|0x50<<8|0x0|0x40005000;
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x04000000;
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	auxout = 0<<31 /* i2c */|0<<30|0x1<<28/*R*/|0x50<<8|0x3|0x10005003;
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 3);


	auxout = 0<<31 /* i2c */|1<<30|0x0<<28/*W*/|0x50<<8|0x0|0x40005000;
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x7e000000;
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);


	auxout = 0<<31 /* i2c */|0<<30|0x1<<28/*R*/|0x50<<8|0x0|0x10005000;
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* this needs to be a call to a function */
	io_i915_write32( DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE | DP_SYNC_VS_HIGH |0x00000091,DP_A);
	io_i915_write32(0x00000001,TRANS_DDI_FUNC_CTL_EDP+0x10);
	io_i915_write32(0x80040011,DP_TP_CTL_A);
	io_i915_write32( DP_PORT_EN | DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_ENHANCED_FRAMING | DP_PLL_FREQ_270MHZ | DP_SCRAMBLING_DISABLE_IRONLAKE | DP_SYNC_VS_HIGH |0x80040091,DP_A);

	/* we may need to move these *after* power well power up and *before* PCH_PP_CONTROL in gma.c */
	io_i915_write32( PANEL_PORT_SELECT_LVDS |(/* PANEL_POWER_UP_DELAY_MASK */0x1<<16)|(/* PANEL_LIGHT_ON_DELAY_MASK */0xa<<0)|0x0001000a,PCH_PP_ON_DELAYS);
	io_i915_write32( PANEL_PORT_SELECT_LVDS |(/* PANEL_POWER_UP_DELAY_MASK */0x7d0<<16)|(/* PANEL_LIGHT_ON_DELAY_MASK */0xa<<0)|0x07d0000a,PCH_PP_ON_DELAYS);


	/* create function: intel_dp_set_bw(dp, u8 bw); */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_LINK_BW_SET<<8|0x0|0x80010000;
	printk(BIOS_SPEW, "DP_LINK_BW_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x0a000480;
	/*( DP_LINK_BW_2_7 &0xa)|0xffffffff8004000a*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	/* this info whould have been goten in intel_dp_get_dpcd. So that function should
	 * set dp->lane_count but does not yet.
	 */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_MAX_LANE_COUNT<<8|0x0|0x90000200;
printk(BIOS_SPEW, "DP_MAX_LANE_COUNT");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* create a function: intel_dp_set_lane_count(dp); gets lane count from dp->lane_count */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_LANE_COUNT_SET<<8|0x0|0x80010100;
printk(BIOS_SPEW, "DP_LANE_COUNT_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x81000000;
	/*0x00000081*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	io_i915_write32(0x80040000,DP_TP_CTL_A);
	io_i915_write32( DP_PORT_EN | DP_LINK_TRAIN_PAT_1 | DP_LINK_TRAIN_PAT_1_CPT | DP_VOLTAGE_0_4 | DP_PRE_EMPHASIS_0 | DP_PORT_WIDTH_1 | DP_PLL_FREQ_270MHZ | DP_SYNC_VS_HIGH |0x80000011,DP_A);

	/* find or create: intel_dp_set_training_pattern(dp, pattern); */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x90010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* why did they do it twice? */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x80010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x01000000;
	/* DP_TRAINING_PATTERN_1 | DP_LINK_QUAL_PATTERN_DISABLE | DP_SYMBOL_ERROR_COUNT_BOTH |0x00000001*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_LANE_COUNT_SET<<8|0x0|0x90010100;
printk(BIOS_SPEW, "DP_LANE_COUNT_SET");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_TRAINING_LANE0_SET<<8|0x0|0x80010300;
printk(BIOS_SPEW, "DP_TRAINING_LANE0_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x00000000;
	/* DP_TRAIN_VOLTAGE_SWING_400 | DP_TRAIN_PRE_EMPHASIS_0 |0x00000000*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_LANE0_1_STATUS<<8|0x1|0x90020201;
printk(BIOS_SPEW, "DP_LANE0_1_STATUS");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 1);

	io_i915_write32(0x80040100,DP_TP_CTL_A);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x90010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x80010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x02000000;
	/* DP_TRAINING_PATTERN_2 | DP_LINK_QUAL_PATTERN_DISABLE | DP_SYMBOL_ERROR_COUNT_BOTH |0x00000002*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_LANE0_1_STATUS<<8|0x1|0x90020201;
printk(BIOS_SPEW, "DP_LANE0_1_STATUS");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 1);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_LANE_ALIGN_STATUS_UPDATED<<8|0x0|0x90020400;
printk(BIOS_SPEW, "DP_LANE_ALIGN_STATUS_UPDATED");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x1<<28/*R*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x90010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	intel_dp_aux_ch(dp, msg, 4, auxin, 0);

	/* create a function */
	auxout = 1<<31 /* dp */|0x0<<28/*W*/|DP_TRAINING_PATTERN_SET<<8|0x0|0x80010200;
printk(BIOS_SPEW, "DP_TRAINING_PATTERN_SET");
	unpack_aux(auxout, &msg[0], 4);
	auxout = 0x00000000;
	/* DP_TRAINING_PATTERN_DISABLE | DP_LINK_QUAL_PATTERN_DISABLE | DP_SYMBOL_ERROR_COUNT_BOTH |0x00000000*/
	unpack_aux(auxout, &msg[4], 4);
	intel_dp_aux_ch(dp, msg, 5, auxin, 0);

	/* need a function: intel_ddi_set_tp or similar */
	io_i915_write32(0x80040200,DP_TP_CTL_A);
	io_i915_write32(0x80040300,DP_TP_CTL_A);
	io_i915_write32(0x03a903a9,BLC_PWM_CPU_CTL);
	io_i915_write32(0x03a903a9,BLC_PWM_PCH_CTL2);
	io_i915_write32(0x80000000,BLC_PWM_PCH_CTL1);


	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,0x4f044);


	/* some of this is not needed. */
	io_i915_write32( PORTD_HOTPLUG_ENABLE | PORTB_HOTPLUG_ENABLE |0x10100010,SDEISR+0x30);
	io_i915_write32( DIGITAL_PORTA_HOTPLUG_ENABLE |0x00000010,DIGITAL_PORT_HOTPLUG_CNTRL);
	io_i915_write32(0x00000000,SDEIIR);
	io_i915_write32(0x00000000,SDEIIR);
	io_i915_write32(0x00000000,DEIIR);
	io_i915_write32(0x80000000,0x45400);
	intel_dp_wait_reg(0x00045400, 0xc0000000);
	io_i915_write32(0x03200500,0x4f000);
	/* io_i915_write32(0x03000556,0x4f000); */
	io_i915_write32(0x03000400,0x4f000);
	io_i915_write32(0x80000000,0x45400);
	intel_dp_wait_reg(0x00045400, 0xc0000000);
	printk(BIOS_SPEW, "pci dev(0x0,0x2,0x0,0x6)");
	io_i915_write32(0x03000400,0x4f000);
	io_i915_write32(0x80000000,0x45400);
	intel_dp_wait_reg(0x00045400, 0xc0000000);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x45430000,0x4f04c);
	io_i915_write32(0x43430000,0x4f04c);
	io_i915_write32(0x02580320,0x4f000);
	io_i915_write32(0x8000298e,CPU_VGACNTRL);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,_CURACNTR);
	io_i915_write32(0x00000000,_CURABASE);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x00000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,_CURBCNTR_IVB);
	io_i915_write32(0x00000000,_CURBBASE_IVB);
	io_i915_write32(0x00000000,_DSPBCNTR);
	io_i915_write32(0x00000000,_DSPBSURF);
	io_i915_write32(0x00000000,0x72080);
	io_i915_write32(0x00000000,0x72084);
	io_i915_write32(0x00000000,_DVSACNTR);
	io_i915_write32(0x00000000,_DVSASURF);
	io_i915_write32(0x00008000,DEIIR);
	intel_dp_wait_reg(0x00044008, 0x00000000);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x00000000,0x4f044);

	/* we just turned vdd off. We're not going to wait. The panel is up. */
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000600,0x4f044);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32(0x01000008,0x4f040);
	io_i915_write32(0x00000008,0x4f05c);
	io_i915_write32(0x00000008,0x4f060);
	io_i915_write32(0x8020298e,CPU_VGACNTRL);
	io_i915_write32(0x00000000,0x4f044);
	intel_dp_wait_reg(0x0004f044, 0x00000000);
	io_i915_write32(/*0x00000640*/dp->stride,_DSPASTRIDE);
	io_i915_write32(0x00000000,_DSPAADDR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000000,0x4f044);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x00000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	/* io_i915_write32(dp->pfa_pos,_PFA_WIN_POS); */
	/* io_i915_write32(0x00000000,_PFA_WIN_SZ); */
	io_i915_write32(dp->pipesrc,_PIPEASRC);
	/* io_i915_write32(dp->pfa_pos,_PFA_WIN_POS); */
	/* io_i915_write32(dp->pfa_ctl,_PFA_CTL_1); */
	/* io_i915_write32(dp->pfa_sz,_PFA_WIN_SZ); */
	io_i915_write32(0x00000080,DEIIR);
	intel_dp_wait_reg(0x00044008, 0x00000000);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x14000000,_DSPACNTR);
	io_i915_write32(/*0x00000640*/dp->stride,_DSPASTRIDE);
	io_i915_write32(0x00000000,_DSPAADDR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x94000000,_DSPACNTR);
	io_i915_write32((/* DISPPLANE_SEL_PIPE(0=A,1=B) */0x0<<24)|0x98000000,_DSPACNTR);
	io_i915_write32(0x00000000,_DSPASIZE+0xc);
	io_i915_write32(0x00000400,0x4f044);
	io_i915_write32(0x00000000,0x4f044);

	io_i915_write32( EDP_BLC_ENABLE | PANEL_POWER_RESET | PANEL_POWER_ON |0x00000007,PCH_PP_CONTROL);

	io_i915_write32( PORTD_HOTPLUG_ENABLE | PORTB_HOTPLUG_ENABLE |0x10100010,SDEISR+0x30);
	io_i915_write32( DIGITAL_PORTA_HOTPLUG_ENABLE |0x00000010,DIGITAL_PORT_HOTPLUG_CNTRL);
	io_i915_write32(0x00000000,SDEIIR);
	io_i915_write32(0x00000000,SDEIIR);
	io_i915_write32(0x00000000,DEIIR);
	io_i915_write32(0x00001800,0x4f044);
}

