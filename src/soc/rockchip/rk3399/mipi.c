/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Rockchip Inc.
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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <gpio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/mipi.h>
#include <soc/soc.h>
#include <timer.h>

static struct rk_mipi_dsi rk_mipi;
static struct rk_mipi_regs *mipi_regs = (void *)MIPI_BASE;

/*
 * The controller should generate 2 frames before
 * preparing the peripheral.
 */
static void rk_mipi_dsi_wait_for_two_frames(struct rk_mipi_dsi *dsi,
					    const struct edid *edid)
{
	int two_frames;
	unsigned int refresh = edid->mode.refresh;

	two_frames = div_round_up(MSECS_PER_SEC * 2, refresh);
	mdelay(two_frames);
}

static const struct dphy_pll_parameter_map dppa_map[] = {
	{  89, 0x00, CP_CURRENT_3UA, LPF_RESISTORS_13KOHM},
	{  99, 0x10, CP_CURRENT_3UA, LPF_RESISTORS_13KOHM},
	{ 109, 0x20, CP_CURRENT_3UA, LPF_RESISTORS_13KOHM},
	{ 129, 0x01, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 139, 0x11, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 149, 0x21, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 169, 0x02, CP_CURRENT_6UA, LPF_RESISTORS_13KOHM},
	{ 179, 0x12, CP_CURRENT_6UA, LPF_RESISTORS_13KOHM},
	{ 199, 0x22, CP_CURRENT_6UA, LPF_RESISTORS_13KOHM},
	{ 219, 0x03, CP_CURRENT_4_5UA, LPF_RESISTORS_13KOHM},
	{ 239, 0x13, CP_CURRENT_4_5UA, LPF_RESISTORS_13KOHM},
	{ 249, 0x23, CP_CURRENT_4_5UA, LPF_RESISTORS_13KOHM},
	{ 269, 0x04, CP_CURRENT_6UA, LPF_RESISTORS_11_5KOHM},
	{ 299, 0x14, CP_CURRENT_6UA, LPF_RESISTORS_11_5KOHM},
	{ 329, 0x05, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 359, 0x15, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 399, 0x25, CP_CURRENT_3UA, LPF_RESISTORS_15_5KOHM},
	{ 449, 0x06, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 499, 0x16, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 549, 0x07, CP_CURRENT_7_5UA, LPF_RESISTORS_10_5KOHM},
	{ 599, 0x17, CP_CURRENT_7_5UA, LPF_RESISTORS_10_5KOHM},
	{ 649, 0x08, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 699, 0x18, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 749, 0x09, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 799, 0x19, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 849, 0x29, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 899, 0x39, CP_CURRENT_7_5UA, LPF_RESISTORS_11_5KOHM},
	{ 949, 0x0a, CP_CURRENT_12UA, LPF_RESISTORS_8KOHM},
	{ 999, 0x1a, CP_CURRENT_12UA, LPF_RESISTORS_8KOHM},
	{1049, 0x2a, CP_CURRENT_12UA, LPF_RESISTORS_8KOHM},
	{1099, 0x3a, CP_CURRENT_12UA, LPF_RESISTORS_8KOHM},
	{1149, 0x0b, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1199, 0x1b, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1249, 0x2b, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1299, 0x3b, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1349, 0x0c, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1399, 0x1c, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1449, 0x2c, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM},
	{1500, 0x3c, CP_CURRENT_12UA, LPF_RESISTORS_10_5KOHM}
};

static int max_mbps_to_parameter(unsigned int max_mbps)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(dppa_map); i++) {
		if (dppa_map[i].max_mbps >= max_mbps)
			return i;
	}

	return -1;
}

static void rk_mipi_dsi_phy_write(struct rk_mipi_dsi *dsi,
				  u8 test_code,
				  u8 test_data)
{
	/*
	 * With the falling edge on TESTCLK, the TESTDIN[7:0] signal content
	 * is latched internally as the current test code. Test data is
	 * programmed internally by rising edge on TESTCLK.
	 */
	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_TESTCLK | PHY_UNTESTCLR);

	write32(&mipi_regs->dsi_phy_tst_ctrl1, PHY_TESTEN | PHY_TESTDOUT(0) |
					       PHY_TESTDIN(test_code));

	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_UNTESTCLK | PHY_UNTESTCLR);

	write32(&mipi_regs->dsi_phy_tst_ctrl1, PHY_UNTESTEN | PHY_TESTDOUT(0) |
					       PHY_TESTDIN(test_data));

	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_TESTCLK | PHY_UNTESTCLR);
}

static int rk_mipi_dsi_phy_init(struct rk_mipi_dsi *dsi)
{
	int i, vco;
	int lane_mbps = div_round_up(dsi->lane_bps, USECS_PER_SEC);

	vco = (lane_mbps < 200) ? 0 : (lane_mbps + 100) / 200;

	i = max_mbps_to_parameter(lane_mbps);
	if (i < 0) {
		printk(BIOS_DEBUG,
		       "failed to get parameter for %dmbps clock\n", lane_mbps);
		return i;
	}

	/* Start by clearing PHY state */
	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_UNTESTCLR);
	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_TESTCLR);
	write32(&mipi_regs->dsi_phy_tst_ctrl0, PHY_UNTESTCLR);

	rk_mipi_dsi_phy_write(dsi, PLL_BIAS_CUR_SEL_CAP_VCO_CONTROL,
			      BYPASS_VCO_RANGE |
			      VCO_RANGE_CON_SEL(vco) |
			      VCO_IN_CAP_CON_LOW |
			      REF_BIAS_CUR_SEL);

	rk_mipi_dsi_phy_write(dsi, PLL_CP_CONTROL_PLL_LOCK_BYPASS,
			      CP_CURRENT_SEL(dppa_map[i].icpctrl));
	rk_mipi_dsi_phy_write(dsi, PLL_LPF_AND_CP_CONTROL,
			      CP_PROGRAM_EN |
			      LPF_PROGRAM_EN |
			      LPF_RESISTORS_SEL(dppa_map[i].lpfctrl));
	rk_mipi_dsi_phy_write(dsi, HS_RX_CONTROL_OF_LANE_0,
			      HSFREQRANGE_SEL(dppa_map[i].hsfreqrange));

	rk_mipi_dsi_phy_write(dsi, PLL_INPUT_DIVIDER_RATIO,
			      INPUT_DIVIDER(dsi->input_div));
	rk_mipi_dsi_phy_write(dsi, PLL_LOOP_DIVIDER_RATIO,
			      LOOP_DIV_LOW_SEL(dsi->feedback_div) |
			      LOW_PROGRAM_EN);
	rk_mipi_dsi_phy_write(dsi, PLL_LOOP_DIVIDER_RATIO,
			      LOOP_DIV_HIGH_SEL(dsi->feedback_div) |
			      HIGH_PROGRAM_EN);
	rk_mipi_dsi_phy_write(dsi, PLL_INPUT_AND_LOOP_DIVIDER_RATIOS_CONTROL,
			      PLL_LOOP_DIV_EN | PLL_INPUT_DIV_EN);
	rk_mipi_dsi_phy_write(dsi, AFE_BIAS_BANDGAP_ANALOG_PROGRAMMABILITY,
			      LOW_PROGRAM_EN |
			      BIASEXTR_SEL(BIASEXTR_127_7));
	rk_mipi_dsi_phy_write(dsi, AFE_BIAS_BANDGAP_ANALOG_PROGRAMMABILITY,
			      HIGH_PROGRAM_EN |
			      BANDGAP_SEL(BANDGAP_96_10));
	rk_mipi_dsi_phy_write(dsi, BANDGAP_AND_BIAS_CONTROL,
			      POWER_CONTROL | INTERNAL_REG_CURRENT |
			      BIAS_BLOCK_ON | BANDGAP_ON);
	rk_mipi_dsi_phy_write(dsi, TERMINATION_RESISTER_CONTROL,
			      TER_RESISTOR_LOW | TER_CAL_DONE |
			      SETRD_MAX | TER_RESISTORS_ON);
	rk_mipi_dsi_phy_write(dsi, TERMINATION_RESISTER_CONTROL,
			      TER_RESISTOR_HIGH | LEVEL_SHIFTERS_ON |
			      SETRD_MAX | POWER_MANAGE |
			      TER_RESISTORS_ON);

	write32(&mipi_regs->dsi_phy_rstz, PHY_ENFORCEPLL | PHY_ENABLECLK |
					  PHY_UNRSTZ | PHY_UNSHUTDOWNZ);
	return 0;
}

static inline int mipi_dsi_pixel_format_to_bpp(enum mipi_dsi_pixel_format fmt)
{
	switch (fmt) {
	case MIPI_DSI_FMT_RGB888:
	case MIPI_DSI_FMT_RGB666:
		return 24;

	case MIPI_DSI_FMT_RGB666_PACKED:
		return 18;

	case MIPI_DSI_FMT_RGB565:
		return 16;
	}

	return -1;
}

static int rk_mipi_dsi_get_lane_bps(struct rk_mipi_dsi *dsi,
				    const struct edid *edid)
{
	u32 i, pre;
	u64 pclk, pllref, tmp, target_bps;
	u32 m = 1, n = 1;
	u32 max_bps = dppa_map[ARRAY_SIZE(dppa_map) - 1].max_mbps * MHz;
	int bpp;

	bpp = mipi_dsi_pixel_format_to_bpp(dsi->format);
	if (bpp < 0) {
		printk(BIOS_DEBUG, "failed to get bpp for pixel format %d\n",
		       dsi->format);
		return bpp;
	}
	pclk = edid->mode.pixel_clock * MSECS_PER_SEC;
	/* take 1 / 0.8, since mbps must bigger than bandwidth of RGB */
	target_bps = pclk / dsi->lanes * bpp / 8 * 10;
	if (target_bps >= max_bps) {
		printk(BIOS_DEBUG, "DPHY clock frequency is out of range\n");
		return -1;
	}
	pllref = OSC_HZ;
	tmp = pllref;
	/*
	 * The limits on the PLL divisor are:
	 *
	 *	5MHz <= (pllref / n) <= 40MHz
	 */
	for (i = pllref / (5 * MHz); i > div_round_up(pllref, 40 * MHz); i--) {
		pre = pllref / i;
		if ((tmp > (target_bps % pre)) && (target_bps / pre < 512)) {
			tmp = target_bps % pre;
			n = i;
			m = target_bps / pre;
		}
		if (tmp == 0)
			break;
	}
	dsi->lane_bps = pllref / n * m;
	dsi->input_div = n;
	dsi->feedback_div = m;

	return 0;
}

static void rk_mipi_dsi_dpi_config(struct rk_mipi_dsi *dsi)
{
	u32 color = 0;

	switch (dsi->format) {
	case MIPI_DSI_FMT_RGB888:
		color = DPI_COLOR_CODING_24BIT;
		break;
	case MIPI_DSI_FMT_RGB666:
		color = DPI_COLOR_CODING_18BIT_2 | EN18_LOOSELY;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		color = DPI_COLOR_CODING_18BIT_1;
		break;
	case MIPI_DSI_FMT_RGB565:
		color = DPI_COLOR_CODING_16BIT_1;
		break;
	}

	write32(&mipi_regs->dsi_dpi_vcid, 0);
	write32(&mipi_regs->dsi_dpi_color_coding, color);

	write32(&mipi_regs->dsi_dpi_cfg_pol, 0);

	write32(&mipi_regs->dsi_dpi_lp_cmd_tim, OUTVACT_LPCMD_TIME(4) |
						INVACT_LPCMD_TIME(4));
}

static void rk_mipi_dsi_packet_handler_config(struct rk_mipi_dsi *dsi)
{
	write32(&mipi_regs->dsi_pckhdl_cfg, EN_CRC_RX | EN_ECC_RX | EN_BTA);
}

static void rk_mipi_dsi_video_mode_config(struct rk_mipi_dsi *dsi)
{
	write32(&mipi_regs->dsi_vid_mode_cfg,
		VID_MODE_TYPE_BURST_SYNC_PULSES | ENABLE_LOW_POWER);
}

static void rk_mipi_dsi_video_packet_config(struct rk_mipi_dsi *dsi)
{
	write32(&mipi_regs->dsi_vid_pkt_size, VID_PKT_SIZE(0x300));
}

static void rk_mipi_dsi_command_mode_config(struct rk_mipi_dsi *dsi)
{
	write32(&mipi_regs->dsi_to_cnt_cfg,
		HSTX_TO_CNT(1000) | LPRX_TO_CNT(1000));
	write32(&mipi_regs->dsi_bta_to_cnt, 0xd00);
	write32(&mipi_regs->dsi_cmd_mode_cfg, CMD_MODE_ALL_LP);
	write32(&mipi_regs->dsi_mode_cfg, ENABLE_CMD_MODE);
}

/* Get lane byte clock cycles. */
static u32 rk_mipi_dsi_get_hcomponent_lbcc(struct rk_mipi_dsi *dsi,
					   u32 hcomponent,
					   const struct edid *edid)
{
	u32 lbcc;
	u64 lbcc_tmp;

	lbcc_tmp = hcomponent * dsi->lane_bps / (8 * MSECS_PER_SEC);
	lbcc = div_round_up(lbcc_tmp, edid->mode.pixel_clock);

	return lbcc;
}

static void rk_mipi_dsi_line_timer_config(struct rk_mipi_dsi *dsi,
					  const struct edid *edid)
{
	u32 htotal, hsa, hbp, lbcc;

	htotal = edid->mode.ha + edid->mode.hbl;
	hsa = edid->mode.hspw;
	hbp = edid->mode.hbl - edid->mode.hso - edid->mode.hspw;

	lbcc = rk_mipi_dsi_get_hcomponent_lbcc(dsi, htotal, edid);
	write32(&mipi_regs->dsi_vid_hline_time, lbcc);

	lbcc = rk_mipi_dsi_get_hcomponent_lbcc(dsi, hsa, edid);
	write32(&mipi_regs->dsi_vid_hsa_time, lbcc);
	lbcc = rk_mipi_dsi_get_hcomponent_lbcc(dsi, hbp, edid);
	write32(&mipi_regs->dsi_vid_hbp_time, lbcc);
}

static void rk_mipi_dsi_vertical_timing_config(struct rk_mipi_dsi *dsi,
					       const struct edid *edid)
{
	u32 vactive, vsa, vfp, vbp;

	vactive = edid->mode.va;
	vsa = edid->mode.vspw;
	vfp = edid->mode.vso;
	vbp = edid->mode.vbl - edid->mode.vso - edid->mode.vspw;

	write32(&mipi_regs->dsi_vid_vactive_lines, vactive);
	write32(&mipi_regs->dsi_vid_vsa_lines, vsa);
	write32(&mipi_regs->dsi_vid_vfp_lines, vfp);
	write32(&mipi_regs->dsi_vid_vbp_lines, vbp);
}

static void rk_mipi_dsi_dphy_timing_config(struct rk_mipi_dsi *dsi)
{
	/*
	 * HS-PREPARE: 40ns + 4 * UI ~ 85ns + 6 * UI
	 * HS-EXIT: 100ns
	 */
	write32(&mipi_regs->dsi_phy_tmr_cfg, PHY_HS2LP_TIME(0x40) |
					     PHY_LP2HS_TIME(0x40) |
					     MAX_RD_TIME(10000));

	write32(&mipi_regs->dsi_phy_tmr_lpclk_cfg, PHY_CLKHS2LP_TIME(0x40) |
						   PHY_CLKLP2HS_TIME(0x40));
}

static void rk_mipi_dsi_clear_err(struct rk_mipi_dsi *dsi)
{
	read32(&mipi_regs->dsi_int_st0);
	read32(&mipi_regs->dsi_int_st1);
	write32(&mipi_regs->dsi_int_msk0, 0);
	write32(&mipi_regs->dsi_int_msk1, 0);
}

static void rk_mipi_dsi_dphy_interface_config(struct rk_mipi_dsi *dsi)
{
	write32(&mipi_regs->dsi_phy_if_cfg, PHY_STOP_WAIT_TIME(0x20) |
					    N_LANES(dsi->lanes));
}

static void rk_mipi_dsi_set_mode(struct rk_mipi_dsi *dsi,
				 enum rk_mipi_dsi_mode mode)
{
	write32(&mipi_regs->dsi_pwr_up, RESET);
	if (mode == MIPI_DSI_CMD_MODE) {
		write32(&mipi_regs->dsi_mode_cfg, ENABLE_CMD_MODE);
	} else {
		write32(&mipi_regs->dsi_mode_cfg, ENABLE_VIDEO_MODE);
		rk_mipi_dsi_video_mode_config(dsi);
		write32(&mipi_regs->dsi_lpclk_ctrl, PHY_TXREQUESTCLKHS);
	}
	write32(&mipi_regs->dsi_pwr_up, POWERUP);
}

static void rk_mipi_dsi_init(struct rk_mipi_dsi *dsi)
{
	/*
	 * The maximum permitted escape clock is 20MHz and it is derived from
	 * lanebyteclk, which is running at "lane_mbps / 8".  Thus we want:
	 *
	 *     (lane_mbps >> 3) / esc_clk_division < 20
	 * which is:
	 *     (lane_mbps >> 3) / 20 > esc_clk_division
	 */
	u32 esc_clk_division = div_round_up(dsi->lane_bps, 8 * 20 * USECS_PER_SEC);

	write32(&mipi_regs->dsi_pwr_up, RESET);
	write32(&mipi_regs->dsi_phy_rstz, PHY_DISFORCEPLL | PHY_DISABLECLK |
					  PHY_RSTZ | PHY_SHUTDOWNZ);
	write32(&mipi_regs->dsi_clk_cfg,
		TO_CLK_DIVIDSION(10) |
		TX_ESC_CLK_DIVIDSION(esc_clk_division));
}

static int rk_mipi_dsi_dcs_transfer(struct rk_mipi_dsi *dsi, u32 hdr_val)
{
	int ret;

	hdr_val = GEN_HDATA(hdr_val) | GEN_HTYPE(0x05);
	ret = read32(&mipi_regs->dsi_cmd_pkt_status);
	if (ret < 0) {
		printk(BIOS_DEBUG, "failed to get available command FIFO\n");
		return ret;
	}

	write32(&mipi_regs->dsi_lpclk_ctrl, 0);
	write32(&mipi_regs->dsi_cmd_mode_cfg, CMD_MODE_ALL_LP);
	write32(&mipi_regs->dsi_gen_hdr, hdr_val);

	return 0;
}

void rk_mipi_prepare(const struct edid *edid, u32 display_on_mdelay, u32 video_mode_mdelay)
{
	rk_mipi.lanes = 4;
	rk_mipi.format = MIPI_DSI_FMT_RGB888;
	if (rk_mipi_dsi_get_lane_bps(&rk_mipi, edid) < 0)
		return;

	rk_mipi_dsi_init(&rk_mipi);
	rk_mipi_dsi_dpi_config(&rk_mipi);
	rk_mipi_dsi_packet_handler_config(&rk_mipi);
	rk_mipi_dsi_video_mode_config(&rk_mipi);
	rk_mipi_dsi_video_packet_config(&rk_mipi);
	rk_mipi_dsi_command_mode_config(&rk_mipi);
	rk_mipi_dsi_line_timer_config(&rk_mipi, edid);
	rk_mipi_dsi_vertical_timing_config(&rk_mipi, edid);
	rk_mipi_dsi_dphy_timing_config(&rk_mipi);
	rk_mipi_dsi_dphy_interface_config(&rk_mipi);
	rk_mipi_dsi_clear_err(&rk_mipi);
	if (rk_mipi_dsi_phy_init(&rk_mipi) < 0)
		return;
	rk_mipi_dsi_wait_for_two_frames(&rk_mipi, edid);

	rk_mipi_dsi_set_mode(&rk_mipi, MIPI_DSI_CMD_MODE);
	if (rk_mipi_dsi_dcs_transfer(&rk_mipi, MIPI_DCS_EXIT_SLEEP_MODE) < 0)
		return;
	mdelay(display_on_mdelay);
	if (rk_mipi_dsi_dcs_transfer(&rk_mipi, MIPI_DCS_SET_DISPLAY_ON) < 0)
		return;
	mdelay(video_mode_mdelay);

	rk_mipi_dsi_set_mode(&rk_mipi, MIPI_DSI_VID_MODE);
}
