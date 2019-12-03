/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) Rockchip, Inc.
 * Copyright (C) Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Designware High-Definition Multimedia Interface (HDMI) driveG
 */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <gpio.h>
#include <stdlib.h>
#include <stdint.h>
#include <soc/addressmap.h>
#include <soc/hdmi.h>
#include <soc/grf.h>
#include <soc/vop.h>
#include <timer.h>

#include "chip.h"

#define AUDIO_SAMPLERATE_DEFAULT	(48*KHz)

#define hdmi_debug(x...)	do { if (0) printk(BIOS_DEBUG, x); } while (0)

struct rk3288_hdmi_regs * const hdmi_regs = (void *)HDMI_TX_BASE;

struct tmds_n_cts {
	u32 tmds;
	u32 cts;
	u32 n;
};

static const struct tmds_n_cts n_cts_table[] = {
	{
		.tmds = 25175, .n = 6144, .cts = 25175,
	}, {
		.tmds = 25200, .n = 6144, .cts = 25200,
	}, {
		.tmds = 27000, .n = 6144, .cts = 27000,
	}, {
		.tmds = 27027, .n = 6144, .cts = 27027,
	}, {
		.tmds = 40000, .n = 6144, .cts = 40000,
	}, {
		.tmds = 54000, .n = 6144, .cts = 54000,
	}, {
		.tmds = 54054, .n = 6144, .cts = 54054,
	}, {
		.tmds = 65000, .n = 6144, .cts = 65000,
	}, {
		.tmds = 74176, .n = 11648, .cts = 140625,
	}, {
		.tmds = 74250, .n = 6144, .cts = 74250,
	}, {
		.tmds = 83500, .n = 6144, .cts = 83500,
	}, {
		.tmds = 106500, .n = 6144, .cts = 106500,
	}, {
		.tmds = 108000, .n = 6144, .cts = 108000,
	}, {
		.tmds = 148352, .n = 5824, .cts = 140625,
	}, {
		.tmds = 148500, .n = 6144, .cts = 148500,
	}, {
		.tmds = 297000, .n = 5120, .cts = 247500,
	}
};

struct hdmi_mpll_config {
	u64 mpixelclock;
	/* Mode of Operation and PLL Dividers Control Register */
	u32 cpce;
	/* PLL Gmp Control Register */
	u32 gmp;
	/* PLL Current COntrol Register */
	u32 curr;
};

struct hdmi_phy_config {
	u64 mpixelclock;
	u32 sym_ctr;    /* clock symbol and transmitter control */
	u32 term;       /* transmission termination value */
	u32 vlev_ctr;   /* voltage level control */
};

static const struct hdmi_phy_config rockchip_phy_config[] = {
	{
		.mpixelclock = 74250,
		.sym_ctr = 0x8009, .term = 0x0004, .vlev_ctr = 0x0272,
	}, {
		.mpixelclock = 148500,
		.sym_ctr = 0x802b, .term = 0x0004, .vlev_ctr = 0x028d,
	}, {
		.mpixelclock = 297000,
		.sym_ctr = 0x8039, .term = 0x0005, .vlev_ctr = 0x028d,
	}, {
		.mpixelclock = ~0ul,
		.sym_ctr = 0x0000, .term = 0x0000, .vlev_ctr = 0x0000,
	}
};

static const struct hdmi_mpll_config rockchip_mpll_cfg[] = {
	{
		.mpixelclock = 40000,
		.cpce = 0x00b3, .gmp = 0x0000, .curr = 0x0018,
	}, {
		.mpixelclock = 65000,
		.cpce = 0x0072, .gmp = 0x0001, .curr = 0x0028,
	}, {
		.mpixelclock = 66000,
		.cpce = 0x013e, .gmp = 0x0003, .curr = 0x0038,
	}, {
		.mpixelclock = 83500,
		.cpce = 0x0072, .gmp = 0x0001, .curr = 0x0028,
	}, {
		.mpixelclock = 146250,
		.cpce = 0x0051, .gmp = 0x0002, .curr = 0x0038,
	}, {
		.mpixelclock = 148500,
		.cpce = 0x0051, .gmp = 0x0003, .curr = 0x0000,
	}, {
		.mpixelclock = ~0ul,
		.cpce = 0x0051, .gmp = 0x0003, .curr = 0x0000,
	}
};

static const u32 csc_coeff_default[3][4] = {
	{ 0x2000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x2000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x2000, 0x0000 }
};

static void hdmi_set_clock_regenerator(u32 n, u32 cts)
{
	u8 cts3;
	u8 n3;

	/* first set ncts_atomic_write (if present) */
	n3 = HDMI_AUD_N3_NCTS_ATOMIC_WRITE;
	write32(&hdmi_regs->aud_n3, n3);

	/* set cts_manual (if present) */
	cts3 = HDMI_AUD_CTS3_CTS_MANUAL;

	cts3 |= HDMI_AUD_CTS3_N_SHIFT_1 << HDMI_AUD_CTS3_N_SHIFT_OFFSET;
	cts3 |= (cts >> 16) & HDMI_AUD_CTS3_AUDCTS19_16_MASK;

	/* write cts values; cts3 must be written first */
	write32(&hdmi_regs->aud_cts3, cts3);
	write32(&hdmi_regs->aud_cts2, (cts >> 8) & 0xff);
	write32(&hdmi_regs->aud_cts1, cts & 0xff);

	/* write n values; n1 must be written last */
	n3 |= (n >> 16) & HDMI_AUD_N3_AUDN19_16_MASK;
	write32(&hdmi_regs->aud_n3, n3);
	write32(&hdmi_regs->aud_n2, (n >> 8) & 0xff);
	write32(&hdmi_regs->aud_n1, n & 0xff);

	write32(&hdmi_regs->aud_inputclkfs, HDMI_AUD_INPUTCLKFS_128);
}

static int hdmi_lookup_n_cts(u32 pixel_clk)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(n_cts_table); i++)
		if (pixel_clk <= n_cts_table[i].tmds)
			break;

	if (i >= ARRAY_SIZE(n_cts_table))
		return -1;

	return i;
}

static void hdmi_audio_set_samplerate(u32 pixel_clk)
{
	u32 clk_n, clk_cts;
	int index;

	index = hdmi_lookup_n_cts(pixel_clk);
	if (index == -1) {
		hdmi_debug("audio not supported for pixel clk %d\n", pixel_clk);
		return;
	}

	clk_n = n_cts_table[index].n;
	clk_cts = n_cts_table[index].cts;
	hdmi_set_clock_regenerator(clk_n, clk_cts);
}

/*
 * this submodule is responsible for the video data synchronization.
 * for example, for rgb 4:4:4 input, the data map is defined as
 *			pin{47~40} <==> r[7:0]
 *			pin{31~24} <==> g[7:0]
 *			pin{15~8}  <==> b[7:0]
 */
static void hdmi_video_sample(void)
{
	u32 color_format = 0x01;
	u8 val;

	val = HDMI_TX_INVID0_INTERNAL_DE_GENERATOR_DISABLE |
	      ((color_format << HDMI_TX_INVID0_VIDEO_MAPPING_OFFSET) &
	      HDMI_TX_INVID0_VIDEO_MAPPING_MASK);

	write32(&hdmi_regs->tx_invid0, val);

	/* enable tx stuffing: when de is inactive, fix the output data to 0 */
	val = HDMI_TX_INSTUFFING_BDBDATA_STUFFING_ENABLE |
	      HDMI_TX_INSTUFFING_RCRDATA_STUFFING_ENABLE |
	      HDMI_TX_INSTUFFING_GYDATA_STUFFING_ENABLE;
	write32(&hdmi_regs->tx_instuffing, val);
	write32(&hdmi_regs->tx_gydata0, 0x0);
	write32(&hdmi_regs->tx_gydata1, 0x0);
	write32(&hdmi_regs->tx_rcrdata0, 0x0);
	write32(&hdmi_regs->tx_rcrdata1, 0x0);
	write32(&hdmi_regs->tx_bcbdata0, 0x0);
	write32(&hdmi_regs->tx_bcbdata1, 0x0);
}

static void hdmi_update_csc_coeffs(void)
{
	u32 i, j;
	u32 csc_scale = 1;

	/* the csc registers are sequential, alternating msb then lsb */
	for (i = 0; i < ARRAY_SIZE(csc_coeff_default); i++) {
		for (j = 0; j < ARRAY_SIZE(csc_coeff_default[0]); j++) {
			u32 coeff = csc_coeff_default[i][j];
			write32(&hdmi_regs->csc_coef[i][j].msb, coeff >> 8);
			write32(&hdmi_regs->csc_coef[i][j].lsb, coeff & 0xff);
		}
	}

	clrsetbits32(&hdmi_regs->csc_scale, HDMI_CSC_SCALE_CSCSCALE_MASK,
		     csc_scale);
}

static void hdmi_video_csc(void)
{
	u32 color_depth = HDMI_CSC_SCALE_CSC_COLORDE_PTH_24BPP;
	u32 interpolation = HDMI_CSC_CFG_INTMODE_DISABLE;

	/* configure the csc registers */
	write32(&hdmi_regs->csc_cfg, interpolation);
	clrsetbits32(&hdmi_regs->csc_scale,
		     HDMI_CSC_SCALE_CSC_COLORDE_PTH_MASK, color_depth);

	hdmi_update_csc_coeffs();
}

static void hdmi_video_packetize(void)
{
	u32 output_select = HDMI_VP_CONF_OUTPUT_SELECTOR_BYPASS;
	u32 remap_size = HDMI_VP_REMAP_YCC422_16BIT;
	u32 color_depth = 0;
	u8 val, vp_conf;

	/* set the packetizer registers */
	val = ((color_depth << HDMI_VP_PR_CD_COLOR_DEPTH_OFFSET) &
		HDMI_VP_PR_CD_COLOR_DEPTH_MASK) |
		((0 << HDMI_VP_PR_CD_DESIRED_PR_FACTOR_OFFSET) &
		HDMI_VP_PR_CD_DESIRED_PR_FACTOR_MASK);
	write32(&hdmi_regs->vp_pr_cd, val);

	clrsetbits32(&hdmi_regs->vp_stuff, HDMI_VP_STUFF_PR_STUFFING_MASK,
		     HDMI_VP_STUFF_PR_STUFFING_STUFFING_MODE);

	/* data from pixel repeater block */
	vp_conf = HDMI_VP_CONF_PR_EN_DISABLE |
		  HDMI_VP_CONF_BYPASS_SELECT_VID_PACKETIZER;

	clrsetbits32(&hdmi_regs->vp_conf, HDMI_VP_CONF_PR_EN_MASK |
		     HDMI_VP_CONF_BYPASS_SELECT_MASK, vp_conf);

	clrsetbits32(&hdmi_regs->vp_stuff, HDMI_VP_STUFF_IDEFAULT_PHASE_MASK,
		     1 << HDMI_VP_STUFF_IDEFAULT_PHASE_OFFSET);

	write32(&hdmi_regs->vp_remap, remap_size);

	vp_conf = HDMI_VP_CONF_BYPASS_EN_ENABLE |
		  HDMI_VP_CONF_PP_EN_DISABLE |
		  HDMI_VP_CONF_YCC422_EN_DISABLE;

	clrsetbits32(&hdmi_regs->vp_conf, HDMI_VP_CONF_BYPASS_EN_MASK |
		     HDMI_VP_CONF_PP_EN_ENMASK | HDMI_VP_CONF_YCC422_EN_MASK,
		     vp_conf);

	clrsetbits32(&hdmi_regs->vp_stuff, HDMI_VP_STUFF_PP_STUFFING_MASK |
		     HDMI_VP_STUFF_YCC422_STUFFING_MASK,
		     HDMI_VP_STUFF_PP_STUFFING_STUFFING_MODE |
		     HDMI_VP_STUFF_YCC422_STUFFING_STUFFING_MODE);

	clrsetbits32(&hdmi_regs->vp_conf, HDMI_VP_CONF_OUTPUT_SELECTOR_MASK,
		     output_select);
}

static inline void hdmi_phy_test_clear(u8 bit)
{
	clrsetbits32(&hdmi_regs->phy_tst0, HDMI_PHY_TST0_TSTCLR_MASK,
		     bit << HDMI_PHY_TST0_TSTCLR_OFFSET);
}

static int hdmi_phy_wait_i2c_done(u32 msec)
{
	struct stopwatch phyi2c_done;
	u32 val;

	stopwatch_init_msecs_expire(&phyi2c_done, msec);
	do {
		val = read32(&hdmi_regs->ih_i2cmphy_stat0);
		if (val & 0x3) {
			write32(&hdmi_regs->ih_i2cmphy_stat0, val);
			return 0;
		}

		udelay(100);
	} while (!stopwatch_expired(&phyi2c_done));

	return 1;
}

static void hdmi_phy_i2c_write(u16 data, u8 addr)
{
	write32(&hdmi_regs->ih_i2cmphy_stat0, 0xff);
	write32(&hdmi_regs->phy_i2cm_address_addr, addr);
	write32(&hdmi_regs->phy_i2cm_datao_1_addr, (u8)(data >> 8));
	write32(&hdmi_regs->phy_i2cm_datao_0_addr, (u8)(data >> 0));
	write32(&hdmi_regs->phy_i2cm_operation_addr,
		HDMI_PHY_I2CM_OPERATION_ADDR_WRITE);

	hdmi_phy_wait_i2c_done(1000);
}

static void hdmi_phy_enable_power(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0, HDMI_PHY_CONF0_PDZ_MASK,
		     enable << HDMI_PHY_CONF0_PDZ_OFFSET);
}

static void hdmi_phy_enable_tmds(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0, HDMI_PHY_CONF0_ENTMDS_MASK,
		     enable << HDMI_PHY_CONF0_ENTMDS_OFFSET);
}

static void hdmi_phy_enable_spare(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0, HDMI_PHY_CONF0_SPARECTRL_MASK,
		     enable << HDMI_PHY_CONF0_SPARECTRL_OFFSET);
}

static void hdmi_phy_gen2_pddq(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0, HDMI_PHY_CONF0_GEN2_PDDQ_MASK,
		     enable << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
}

static void hdmi_phy_gen2_txpwron(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0,
		     HDMI_PHY_CONF0_GEN2_TXPWRON_MASK,
		     enable << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
}

static void hdmi_phy_sel_data_en_pol(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0,
		     HDMI_PHY_CONF0_SELDATAENPOL_MASK,
		     enable << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
}

static void hdmi_phy_sel_interface_control(u8 enable)
{
	clrsetbits32(&hdmi_regs->phy_conf0, HDMI_PHY_CONF0_SELDIPIF_MASK,
		     enable << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
}

static int hdmi_phy_configure(u32 mpixelclock)
{
	struct stopwatch pll_ready;
	u8 i, val;

	write32(&hdmi_regs->mc_flowctrl,
		HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS);

	/* gen2 tx power off */
	hdmi_phy_gen2_txpwron(0);

	/* gen2 pddq */
	hdmi_phy_gen2_pddq(1);

	/* phy reset */
	write32(&hdmi_regs->mc_phyrstz, HDMI_MC_PHYRSTZ_DEASSERT);
	write32(&hdmi_regs->mc_phyrstz, HDMI_MC_PHYRSTZ_ASSERT);
	write32(&hdmi_regs->mc_heacphy_rst, HDMI_MC_HEACPHY_RST_ASSERT);

	hdmi_phy_test_clear(1);
	write32(&hdmi_regs->phy_i2cm_slave_addr,
		HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2);
	hdmi_phy_test_clear(0);

	/* pll/mpll cfg - always match on final entry */
	for (i = 0; rockchip_mpll_cfg[i].mpixelclock != (~0ul); i++)
		if (mpixelclock <= rockchip_mpll_cfg[i].mpixelclock)
			break;

	hdmi_phy_i2c_write(rockchip_mpll_cfg[i].cpce, PHY_OPMODE_PLLCFG);
	hdmi_phy_i2c_write(rockchip_mpll_cfg[i].gmp, PHY_PLLGMPCTRL);
	hdmi_phy_i2c_write(rockchip_mpll_cfg[i].curr, PHY_PLLCURRCTRL);

	hdmi_phy_i2c_write(0x0000, PHY_PLLPHBYCTRL);
	hdmi_phy_i2c_write(0x0006, PHY_PLLCLKBISTPHASE);

	for (i = 0; rockchip_phy_config[i].mpixelclock != (~0ul); i++)
		if (mpixelclock <= rockchip_phy_config[i].mpixelclock)
			break;

	/*
	 * resistance term 133ohm cfg
	 * preemp cgf 0.00
	 * tx/ck lvl 10
	 */
	hdmi_phy_i2c_write(rockchip_phy_config[i].term, PHY_TXTERM);
	hdmi_phy_i2c_write(rockchip_phy_config[i].sym_ctr, PHY_CKSYMTXCTRL);
	hdmi_phy_i2c_write(rockchip_phy_config[i].vlev_ctr, PHY_VLEVCTRL);

	/* remove clk term */
	hdmi_phy_i2c_write(0x8000, PHY_CKCALCTRL);

	hdmi_phy_enable_power(1);

	/* toggle tmds enable */
	hdmi_phy_enable_tmds(0);
	hdmi_phy_enable_tmds(1);

	/* gen2 tx power on */
	hdmi_phy_gen2_txpwron(1);
	hdmi_phy_gen2_pddq(0);

	hdmi_phy_enable_spare(1);

	/* wait for phy pll lock */
	stopwatch_init_msecs_expire(&pll_ready, 5);
	do {
		val = read32(&hdmi_regs->phy_stat0);
		if (!(val & HDMI_PHY_TX_PHY_LOCK))
			return 0;

		udelay(100);
	} while (!stopwatch_expired(&pll_ready));

	return -1;
}

static int hdmi_phy_init(u32 mpixelclock)
{
	int i, ret;

	/* hdmi phy spec says to do the phy initialization sequence twice */
	for (i = 0; i < 2; i++) {
		hdmi_phy_sel_data_en_pol(1);
		hdmi_phy_sel_interface_control(0);
		hdmi_phy_enable_tmds(0);
		hdmi_phy_enable_power(0);

		/* enable csc */
		ret = hdmi_phy_configure(mpixelclock);
		if (ret) {
			hdmi_debug("hdmi phy config failure %d\n", ret);
			return ret;
		}
	}

	return 0;
}

static void hdmi_av_composer(const struct edid *edid)
{
	u8 mdataenablepolarity = 1;
	u8 inv_val;

	/* set up hdmi_fc_invidconf */
	inv_val = HDMI_FC_INVIDCONF_HDCP_KEEPOUT_INACTIVE;

	inv_val |= ((edid->mode.pvsync == '+') ?
		   HDMI_FC_INVIDCONF_VSYNC_IN_POLARITY_ACTIVE_HIGH :
		   HDMI_FC_INVIDCONF_VSYNC_IN_POLARITY_ACTIVE_LOW);

	inv_val |= ((edid->mode.phsync == '+') ?
		   HDMI_FC_INVIDCONF_HSYNC_IN_POLARITY_ACTIVE_HIGH :
		   HDMI_FC_INVIDCONF_HSYNC_IN_POLARITY_ACTIVE_LOW);

	inv_val |= (mdataenablepolarity ?
		   HDMI_FC_INVIDCONF_DE_IN_POLARITY_ACTIVE_HIGH :
		   HDMI_FC_INVIDCONF_DE_IN_POLARITY_ACTIVE_LOW);

	inv_val |= (edid->hdmi_monitor_detected ?
		   HDMI_FC_INVIDCONF_DVI_MODEZ_HDMI_MODE :
		   HDMI_FC_INVIDCONF_DVI_MODEZ_DVI_MODE);

	inv_val |= HDMI_FC_INVIDCONF_R_V_BLANK_IN_OSC_ACTIVE_LOW;

	inv_val |= HDMI_FC_INVIDCONF_IN_I_P_PROGRESSIVE;

	write32(&hdmi_regs->fc_invidconf, inv_val);

	/* set up horizontal active pixel width */
	write32(&hdmi_regs->fc_inhactv1, edid->mode.ha >> 8);
	write32(&hdmi_regs->fc_inhactv0, edid->mode.ha);

	/* set up vertical active lines */
	write32(&hdmi_regs->fc_invactv1, edid->mode.va >> 8);
	write32(&hdmi_regs->fc_invactv0, edid->mode.va);

	/* set up horizontal blanking pixel region width */
	write32(&hdmi_regs->fc_inhblank1, edid->mode.hbl >> 8);
	write32(&hdmi_regs->fc_inhblank0, edid->mode.hbl);

	/* set up vertical blanking pixel region width */
	write32(&hdmi_regs->fc_invblank, edid->mode.vbl);

	/* set up hsync active edge delay width (in pixel clks) */
	write32(&hdmi_regs->fc_hsyncindelay1, edid->mode.hso >> 8);
	write32(&hdmi_regs->fc_hsyncindelay0, edid->mode.hso);

	/* set up vsync active edge delay (in lines) */
	write32(&hdmi_regs->fc_vsyncindelay, edid->mode.vso);

	/* set up hsync active pulse width (in pixel clks) */
	write32(&hdmi_regs->fc_hsyncinwidth1, edid->mode.hspw >> 8);
	write32(&hdmi_regs->fc_hsyncinwidth0, edid->mode.hspw);

	/* set up vsync active edge delay (in lines) */
	write32(&hdmi_regs->fc_vsyncinwidth, edid->mode.vspw);
}

/* hdmi initialization step b.4 */
static void hdmi_enable_video_path(void)
{
	u8 clkdis;

	/* control period minimum duration */
	write32(&hdmi_regs->fc_ctrldur, 12);
	write32(&hdmi_regs->fc_exctrldur, 32);
	write32(&hdmi_regs->fc_exctrlspac, 1);

	/* set to fill tmds data channels */
	write32(&hdmi_regs->fc_ch0pream, 0x0b);
	write32(&hdmi_regs->fc_ch1pream, 0x16);
	write32(&hdmi_regs->fc_ch2pream, 0x21);

	/* enable pixel clock and tmds data path */
	clkdis = 0x7f;
	clkdis &= ~HDMI_MC_CLKDIS_PIXELCLK_DISABLE;
	write32(&hdmi_regs->mc_clkdis, clkdis);

	clkdis &= ~HDMI_MC_CLKDIS_TMDSCLK_DISABLE;
	write32(&hdmi_regs->mc_clkdis, clkdis);

	clkdis &= ~HDMI_MC_CLKDIS_AUDCLK_DISABLE;
	write32(&hdmi_regs->mc_clkdis, clkdis);
}

/* workaround to clear the overflow condition */
static void hdmi_clear_overflow(void)
{
	u8 val, count;

	/* tmds software reset */
	write32(&hdmi_regs->mc_swrstz, (u8)~HDMI_MC_SWRSTZ_TMDSSWRST_REQ);

	val = read32(&hdmi_regs->fc_invidconf);

	for (count = 0; count < 4; count++)
		write32(&hdmi_regs->fc_invidconf, val);
}

static void hdmi_audio_set_format(void)
{
	write32(&hdmi_regs->aud_conf0,
		HDMI_AUD_CONF0_I2S_SELECT | HDMI_AUD_CONF0_I2S_IN_EN_0);

	write32(&hdmi_regs->aud_conf1,
		HDMI_AUD_CONF1_I2S_MODE_STANDARD_MODE |
		HDMI_AUD_CONF1_I2S_WIDTH_16BIT);

	write32(&hdmi_regs->aud_conf2, 0x00);
}

static void hdmi_audio_fifo_reset(void)
{
	write32(&hdmi_regs->mc_swrstz, (u8)~HDMI_MC_SWRSTZ_II2SSWRST_REQ);
	write32(&hdmi_regs->aud_conf0, HDMI_AUD_CONF0_SW_AUDIO_FIFO_RST);

	write32(&hdmi_regs->aud_int, 0x00);
	write32(&hdmi_regs->aud_int1, 0x00);
}

static int hdmi_setup(const struct edid *edid)
{
	int ret;

	hdmi_debug("hdmi, mode info : clock %d hdis %d vdis %d\n",
		   edid->mode.pixel_clock, edid->mode.ha, edid->mode.va);

	hdmi_av_composer(edid);

	ret = hdmi_phy_init(edid->mode.pixel_clock);
	if (ret)
		return ret;

	hdmi_enable_video_path();

	hdmi_audio_fifo_reset();
	hdmi_audio_set_format();
	hdmi_audio_set_samplerate(edid->mode.pixel_clock);

	hdmi_video_packetize();
	hdmi_video_csc();
	hdmi_video_sample();

	hdmi_clear_overflow();

	return 0;
}

static void hdmi_init_interrupt(void)
{
	u8 ih_mute;

	/*
	 * boot up defaults are:
	 * hdmi_ih_mute   = 0x03 (disabled)
	 * hdmi_ih_mute_* = 0x00 (enabled)
	 *
	 * disable top level interrupt bits in hdmi block
	 */
	ih_mute = read32(&hdmi_regs->ih_mute) |
		  HDMI_IH_MUTE_MUTE_WAKEUP_INTERRUPT |
		  HDMI_IH_MUTE_MUTE_ALL_INTERRUPT;

	write32(&hdmi_regs->ih_mute, ih_mute);

	/* enable i2c master done irq */
	write32(&hdmi_regs->i2cm_int, ~0x04);

	/* enable i2c client nack % arbitration error irq */
	write32(&hdmi_regs->i2cm_ctlint, ~0x44);

	/* enable phy i2cm done irq */
	write32(&hdmi_regs->phy_i2cm_int_addr, HDMI_PHY_I2CM_INT_ADDR_DONE_POL);

	/* enable phy i2cm nack & arbitration error irq */
	write32(&hdmi_regs->phy_i2cm_ctlint_addr,
		HDMI_PHY_I2CM_CTLINT_ADDR_NAC_POL |
		HDMI_PHY_I2CM_CTLINT_ADDR_ARBITRATION_POL);

	/* enable cable hot plug irq */
	write32(&hdmi_regs->phy_mask0, (u8)~HDMI_PHY_HPD);

	/* clear hotplug interrupts */
	write32(&hdmi_regs->ih_phy_stat0, HDMI_IH_PHY_STAT0_HPD);
}

static u8 hdmi_get_plug_in_status(void)
{
	u8 val = read32(&hdmi_regs->phy_stat0) & HDMI_PHY_HPD;

	return !!(val);
}

static int hdmi_wait_for_hpd(void)
{
	struct stopwatch hpd;

	stopwatch_init_msecs_expire(&hpd, 30000);
	do {
		if (hdmi_get_plug_in_status())
			return 0;
		udelay(100);
	} while (!stopwatch_expired(&hpd));

	return -1;
}

static int hdmi_ddc_wait_i2c_done(int msec)
{
	struct stopwatch ddci2c_done;
	u32 val;

	stopwatch_init_msecs_expire(&ddci2c_done, msec);
	do {
		val = read32(&hdmi_regs->ih_i2cm_stat0);
		if (val & 0x2) {
			write32(&hdmi_regs->ih_i2cm_stat0, val);
			return 0;
		}

		udelay(100);
	} while (!stopwatch_expired(&ddci2c_done));

	return 1;
}

static void hdmi_ddc_reset(void)
{
	clrsetbits32(&hdmi_regs->i2cm_softrstz, HDMI_I2CM_SOFTRSTZ,
		     HDMI_I2CM_SOFTRSTZ);
}

static int hdmi_read_edid(int block, u8 *buff)
{
	int shift = (block % 2) * 0x80;
	int edid_read_err = 0;
	u32 trytime = 5;
	u32 n, j, val;

	/* set ddc i2c clk which devided from ddc_clk to 100khz */
	write32(&hdmi_regs->i2cm_ss_scl_hcnt_0_addr, 0x7a);
	write32(&hdmi_regs->i2cm_ss_scl_lcnt_0_addr, 0x8d);
	clrsetbits32(&hdmi_regs->i2cm_div, HDMI_I2CM_DIV_FAST_STD_MODE,
			HDMI_I2CM_DIV_STD_MODE);

	write32(&hdmi_regs->i2cm_slave, HDMI_I2CM_SLAVE_DDC_ADDR);
	write32(&hdmi_regs->i2cm_segaddr, HDMI_I2CM_SEGADDR_DDC);
	write32(&hdmi_regs->i2cm_segptr, block >> 1);

	while (trytime--) {
		edid_read_err = 0;

		for (n = 0; n < HDMI_EDID_BLOCK_SIZE/8; n++) {
			write32(&hdmi_regs->i2cmess, shift + 8 * n);

			if (block == 0)
				clrsetbits32(&hdmi_regs->i2cm_operation,
					     HDMI_I2CM_OPT_RD8,
					     HDMI_I2CM_OPT_RD8);
			else
				clrsetbits32(&hdmi_regs->i2cm_operation,
					     HDMI_I2CM_OPT_RD8_EXT,
					     HDMI_I2CM_OPT_RD8_EXT);

			if (hdmi_ddc_wait_i2c_done(10)) {
				hdmi_ddc_reset();
				edid_read_err = 1;
				break;
			}

			for (j = 0; j < 8; j++) {
				val = read32(&hdmi_regs->i2cm_buf0 + j);
				buff[8 * n + j] = val;
			}
		}

		if (!edid_read_err)
			break;
	}

	return edid_read_err;
}

int rk_hdmi_get_edid(struct edid *edid)
{
	u8 edid_buf[HDMI_EDID_BLOCK_SIZE * 2];
	u32 edid_size = HDMI_EDID_BLOCK_SIZE;
	gpio_t hdmi_i2c_sda = GPIO(7, C, 3);
	gpio_t hdmi_i2c_scl = GPIO(7, C, 4);
	int ret;

	/* If SDA is low, try to clock once to fix it */
	gpio_input_pullup(hdmi_i2c_sda);
	if (gpio_get(hdmi_i2c_sda) == 0) {
		gpio_output(hdmi_i2c_scl, 0);
		udelay(1000);
		gpio_input_pullup(hdmi_i2c_scl);
		udelay(1000);
	}

	/* HDMI I2C */
	write32(&rk3288_grf->iomux_i2c5sda, IOMUX_HDMI_EDP_I2C_SDA);
	write32(&rk3288_grf->iomux_i2c5scl, IOMUX_HDMI_EDP_I2C_SCL);

	ret = hdmi_read_edid(0, edid_buf);
	if (ret) {
		hdmi_debug("failed to read edid.\n");
		return -1;
	}

	if (edid_buf[0x7e] != 0) {
		hdmi_read_edid(1, edid_buf + HDMI_EDID_BLOCK_SIZE);
		edid_size += HDMI_EDID_BLOCK_SIZE;
	}

	/* Assume usage of HDMI implies an external display in which case
	 * we should be lenient about errors that the EDID decoder finds. */
	if (decode_edid(edid_buf, edid_size, edid) != EDID_CONFORMANT)
		hdmi_debug("failed to decode edid.\n");

	/* Try 480p for best compatibility. */
	if (set_display_mode(edid, EDID_MODE_640x480_60Hz))
		hdmi_debug("failed to set mode to 640x480@60Hz\n");

	return 0;
}

int rk_hdmi_enable(const struct edid *edid)
{
	hdmi_setup(edid);

	return 0;
}

int rk_hdmi_init(u32 vop_id)
{
	int ret;
	u32 val;

	/* hdmi source select hdmi controller */
	write32(&rk3288_grf->soc_con6, RK_SETBITS(1 << 15));

	/* hdmi data from vop id */
	val = (vop_id == 1) ? RK_SETBITS(1 << 4) : RK_CLRBITS(1 << 4);
	write32(&rk3288_grf->soc_con6, val);

	ret = hdmi_wait_for_hpd();
	if (ret < 0) {
		hdmi_debug("hdmi can not get hpd signal\n");
		return -1;
	}

	hdmi_init_interrupt();

	hdmi_debug("hdmi init success\n");

	return 0;
}
