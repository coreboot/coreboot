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

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <gpio.h>
#include <string.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/mipi.h>
#include <soc/soc.h>
#include <types.h>
#include <timer.h>

static struct rk_mipi_dsi rk_mipi[2] = {
	{ .mipi_regs = (void *)MIPI0_BASE},
	{ .mipi_regs = (void *)MIPI1_BASE}
};

/*
 * The controller should generate 2 frames before
 * preparing the peripheral.
 */
static void rk_mipi_dsi_wait_for_two_frames(struct rk_mipi_dsi *dsi,
					    const struct edid *edid)
{
	int two_frames;
	unsigned int refresh = edid->mode.refresh;

	two_frames = DIV_ROUND_UP(MSECS_PER_SEC * 2, refresh);
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
	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0,
		PHY_TESTCLK | PHY_UNTESTCLR);

	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl1,
		PHY_TESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(test_code));

	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0,
		PHY_UNTESTCLK | PHY_UNTESTCLR);

	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl1,
		PHY_UNTESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(test_data));

	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0,
		PHY_TESTCLK | PHY_UNTESTCLR);
}

/* bytes_per_ns - Nanoseconds to byte clock cycles */
static inline unsigned int bytes_per_ns(struct rk_mipi_dsi *dsi, int ns)
{
	return DIV_ROUND_UP((u64)ns * dsi->lane_bps, (u64)8 * NSECS_PER_SEC);
}

 /* bits_per_ns - Nanoseconds to bit time periods */
static inline unsigned int bits_per_ns(struct rk_mipi_dsi *dsi, int ns)
{
	return DIV_ROUND_UP((u64)ns * dsi->lane_bps, NSECS_PER_SEC);
}

static int rk_mipi_dsi_wait_phy_lock(struct rk_mipi_dsi *dsi)
{
	struct stopwatch sw;
	int val;

	stopwatch_init_msecs_expire(&sw, 20);
	do {
		val = read32(&dsi->mipi_regs->dsi_phy_status);
		if (val & LOCK)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
}

static int rk_mipi_dsi_phy_init(struct rk_mipi_dsi *dsi)
{
	int i, vco, val;
	int lane_mbps = DIV_ROUND_UP(dsi->lane_bps, USECS_PER_SEC);
	struct stopwatch sw;

	vco = (lane_mbps < 200) ? 0 : (lane_mbps + 100) / 200;

	i = max_mbps_to_parameter(lane_mbps);
	if (i < 0) {
		printk(BIOS_DEBUG,
		       "failed to get parameter for %dmbps clock\n", lane_mbps);
		return i;
	}

	/* Start by clearing PHY state */
	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0, PHY_UNTESTCLR);
	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0, PHY_TESTCLR);
	write32(&dsi->mipi_regs->dsi_phy_tst_ctrl0, PHY_UNTESTCLR);

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

	/*
	 * we need set divider control register immediately to make
	 * the configured LSB effective according to IP simulation
	 * and lab test results. Only in this way can we get correct
	 * mipi phy pll frequency.
	 */
	rk_mipi_dsi_phy_write(dsi, PLL_INPUT_AND_LOOP_DIVIDER_RATIOS_CONTROL,
			      PLL_LOOP_DIV_EN | PLL_INPUT_DIV_EN);
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
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_REQUEST_STATE_TIME_CONTROL,
			      TLP_PROGRAM_EN | bytes_per_ns(dsi, 500));
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_PREPARE_STATE_TIME_CONTROL,
			      THS_PRE_PROGRAM_EN | bits_per_ns(dsi, 40));
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_HS_ZERO_STATE_TIME_CONTROL,
			      THS_ZERO_PROGRAM_EN | bytes_per_ns(dsi, 300));
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_TRAIL_STATE_TIME_CONTROL,
			      THS_PRE_PROGRAM_EN | bits_per_ns(dsi, 100));
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_EXIT_STATE_TIME_CONTROL,
			      BIT(5) | bytes_per_ns(dsi, 100));
	rk_mipi_dsi_phy_write(dsi, HS_TX_CLOCK_LANE_POST_TIME_CONTROL,
			      BIT(5) | (bytes_per_ns(dsi, 60) + 7));
	rk_mipi_dsi_phy_write(dsi, HS_TX_DATA_LANE_REQUEST_STATE_TIME_CONTROL,
			      TLP_PROGRAM_EN | bytes_per_ns(dsi, 500));
	rk_mipi_dsi_phy_write(dsi, HS_TX_DATA_LANE_PREPARE_STATE_TIME_CONTROL,
			      THS_PRE_PROGRAM_EN | (bits_per_ns(dsi, 50) + 5));
	rk_mipi_dsi_phy_write(dsi, HS_TX_DATA_LANE_HS_ZERO_STATE_TIME_CONTROL,
				   THS_ZERO_PROGRAM_EN |
				   (bytes_per_ns(dsi, 140) + 2));
	rk_mipi_dsi_phy_write(dsi, HS_TX_DATA_LANE_TRAIL_STATE_TIME_CONTROL,
			      THS_PRE_PROGRAM_EN | (bits_per_ns(dsi, 60) + 8));
	rk_mipi_dsi_phy_write(dsi, HS_TX_DATA_LANE_EXIT_STATE_TIME_CONTROL,
			      BIT(5) | bytes_per_ns(dsi, 100));

	write32(&dsi->mipi_regs->dsi_phy_rstz,
				PHY_ENFORCEPLL | PHY_ENABLECLK |
				PHY_UNRSTZ | PHY_UNSHUTDOWNZ);

	if (rk_mipi_dsi_wait_phy_lock(dsi)) {
		printk(BIOS_ERR, "failed to wait for phy lock state\n");
		return -1;
	}

	stopwatch_init_msecs_expire(&sw, 20);
	do {
		val = read32(&dsi->mipi_regs->dsi_phy_status);
		if (val & STOP_STATE_CLK_LANE)
			return 0;
	} while (!stopwatch_expired(&sw));

	printk(BIOS_ERR, "failed to wait for phy clk lane stop state");
	return -1;
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
				    const struct edid *edid,
				    const struct mipi_panel_data *panel_data)
{
	u64 pclk, target_bps;
	u32 max_bps = dppa_map[ARRAY_SIZE(dppa_map) - 1].max_mbps * MHz;
	int bpp;
	u64 best_freq = 0;
	u64 fvco_min, fvco_max, fref;
	u32 min_prediv, max_prediv;
	u32 prediv, best_prediv;
	u64 fbdiv, best_fbdiv;
	u32 min_delta;

	bpp = mipi_dsi_pixel_format_to_bpp(dsi->format);
	if (bpp < 0) {
		printk(BIOS_DEBUG, "failed to get bpp for pixel format %d\n",
		       dsi->format);
		return bpp;
	}
	pclk = (u64)edid->mode.pixel_clock * MSECS_PER_SEC;

	/* take 1 / 0.8, since mbps must bigger than bandwidth of RGB */
	target_bps = pclk / panel_data->lanes * bpp / 8 * 10;
	if (target_bps >= max_bps) {
		printk(BIOS_DEBUG, "DPHY clock frequency is out of range\n");
		return -1;
	}

	fref = OSC_HZ;

	/* constraint: 5Mhz <= Fref / N <= 40MHz */
	min_prediv = DIV_ROUND_UP(fref, 40 * MHz);
	max_prediv = fref / (5 * MHz);

	/* constraint: 80MHz <= Fvco <= 1500Mhz */
	fvco_min = 80 * MHz;
	fvco_max = 1500 * MHz;
	min_delta = 1500 * MHz;

	for (prediv = min_prediv; prediv <= max_prediv; prediv++) {
		u64 freq;
		int delta;

		/* Fvco = Fref * M / N */
		fbdiv = target_bps * prediv / fref;

		/*
		 * Due to the use of a "by 2 pre-scaler", the range of the
		 * feedback multiplication value M is limited to even division
		 * numbers, and m must be in 6 <= m <= 512.
		 */
		fbdiv += fbdiv % 2;
		if (fbdiv < 6 || fbdiv > 512)
			continue;

		freq = (u64)fbdiv * fref / prediv;
		if (freq < fvco_min || freq > fvco_max)
			continue;

		delta = target_bps - freq;
		delta = ABS(delta);
		if (delta >= min_delta)
			continue;

		best_prediv = prediv;
		best_fbdiv = fbdiv;
		min_delta = delta;
		best_freq = freq;
	}

	if (best_freq) {
		dsi->lane_bps = best_freq;
		dsi->input_div = best_prediv;
		dsi->feedback_div = best_fbdiv;
	} else {
		printk(BIOS_ERR, "Can not find best_freq for DPHY\n");
		return -1;
	}

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

	write32(&dsi->mipi_regs->dsi_dpi_vcid, 0);
	write32(&dsi->mipi_regs->dsi_dpi_color_coding, color);

	write32(&dsi->mipi_regs->dsi_dpi_cfg_pol, 0);

	write32(&dsi->mipi_regs->dsi_dpi_lp_cmd_tim,
		OUTVACT_LPCMD_TIME(4) | INVACT_LPCMD_TIME(4));
}

static void rk_mipi_dsi_packet_handler_config(struct rk_mipi_dsi *dsi)
{
	write32(&dsi->mipi_regs->dsi_pckhdl_cfg,
		EN_CRC_RX | EN_ECC_RX | EN_BTA);
}

static void rk_mipi_dsi_video_mode_config(struct rk_mipi_dsi *dsi)
{
	write32(&dsi->mipi_regs->dsi_vid_mode_cfg,
		VID_MODE_TYPE_BURST_SYNC_PULSES | ENABLE_LOW_POWER);
}

static void rk_mipi_dsi_video_packet_config(struct rk_mipi_dsi *dsi,
			const struct edid *edid,
			const struct mipi_panel_data *panel_data)
{
	int pkt_size;

	if (panel_data->mipi_num > 1)
		pkt_size = VID_PKT_SIZE(edid->mode.ha / 2 + 4);
	else
		pkt_size = VID_PKT_SIZE(edid->mode.ha);

	write32(&dsi->mipi_regs->dsi_vid_pkt_size, pkt_size);
}

static void rk_mipi_dsi_command_mode_config(struct rk_mipi_dsi *dsi)
{
	write32(&dsi->mipi_regs->dsi_to_cnt_cfg,
		HSTX_TO_CNT(1000) | LPRX_TO_CNT(1000));
	write32(&dsi->mipi_regs->dsi_bta_to_cnt, 0xd00);
	write32(&dsi->mipi_regs->dsi_cmd_mode_cfg, CMD_MODE_ALL_LP);
	write32(&dsi->mipi_regs->dsi_mode_cfg, ENABLE_CMD_MODE);
}

/* Get lane byte clock cycles. */
static u32 rk_mipi_dsi_get_hcomponent_lbcc(struct rk_mipi_dsi *dsi,
					   u32 hcomponent,
					   const struct edid *edid)
{
	u32 lbcc;
	u64 lbcc_tmp;

	lbcc_tmp = hcomponent * dsi->lane_bps / (8 * MSECS_PER_SEC);
	lbcc = DIV_ROUND_UP(lbcc_tmp, edid->mode.pixel_clock);

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
	write32(&dsi->mipi_regs->dsi_vid_hline_time, lbcc);

	lbcc = rk_mipi_dsi_get_hcomponent_lbcc(dsi, hsa, edid);
	write32(&dsi->mipi_regs->dsi_vid_hsa_time, lbcc);
	lbcc = rk_mipi_dsi_get_hcomponent_lbcc(dsi, hbp, edid);
	write32(&dsi->mipi_regs->dsi_vid_hbp_time, lbcc);
}

static void rk_mipi_dsi_vertical_timing_config(struct rk_mipi_dsi *dsi,
					       const struct edid *edid)
{
	u32 vactive, vsa, vfp, vbp;

	vactive = edid->mode.va;
	vsa = edid->mode.vspw;
	vfp = edid->mode.vso;
	vbp = edid->mode.vbl - edid->mode.vso - edid->mode.vspw;

	write32(&dsi->mipi_regs->dsi_vid_vactive_lines, vactive);
	write32(&dsi->mipi_regs->dsi_vid_vsa_lines, vsa);
	write32(&dsi->mipi_regs->dsi_vid_vfp_lines, vfp);
	write32(&dsi->mipi_regs->dsi_vid_vbp_lines, vbp);
}

static void rk_mipi_dsi_dphy_timing_config(struct rk_mipi_dsi *dsi)
{
	/*
	 * HS-PREPARE: 40ns + 4 * UI ~ 85ns + 6 * UI
	 * HS-EXIT: 100ns
	 */
	write32(&dsi->mipi_regs->dsi_phy_tmr_cfg, PHY_HS2LP_TIME(0x40) |
					     PHY_LP2HS_TIME(0x40) |
					     MAX_RD_TIME(10000));

	write32(&dsi->mipi_regs->dsi_phy_tmr_lpclk_cfg,
		PHY_CLKHS2LP_TIME(0x40) | PHY_CLKLP2HS_TIME(0x40));
}

static void rk_mipi_dsi_clear_err(struct rk_mipi_dsi *dsi)
{
	read32(&dsi->mipi_regs->dsi_int_st0);
	read32(&dsi->mipi_regs->dsi_int_st1);
	write32(&dsi->mipi_regs->dsi_int_msk0, 0);
	write32(&dsi->mipi_regs->dsi_int_msk1, 0);
}

static void rk_mipi_dsi_dphy_interface_config(struct rk_mipi_dsi *dsi)
{
	write32(&dsi->mipi_regs->dsi_phy_if_cfg, PHY_STOP_WAIT_TIME(0x20) |
					    N_LANES(dsi->lanes));
}

static void rk_mipi_dsi_set_mode(struct rk_mipi_dsi *dsi,
				 enum rk_mipi_dsi_mode mode)
{
	write32(&dsi->mipi_regs->dsi_pwr_up, RESET);
	if (mode == MIPI_DSI_CMD_MODE) {
		write32(&dsi->mipi_regs->dsi_mode_cfg, ENABLE_CMD_MODE);
	} else {
		write32(&dsi->mipi_regs->dsi_mode_cfg, ENABLE_VIDEO_MODE);
		rk_mipi_dsi_video_mode_config(dsi);
		write32(&dsi->mipi_regs->dsi_lpclk_ctrl, PHY_TXREQUESTCLKHS);
	}
	write32(&dsi->mipi_regs->dsi_pwr_up, POWERUP);
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
	u32 esc_clk_division = DIV_ROUND_UP(dsi->lane_bps,
					    8 * 20 * USECS_PER_SEC);

	write32(&dsi->mipi_regs->dsi_pwr_up, RESET);
	write32(&dsi->mipi_regs->dsi_phy_rstz,
		PHY_DISFORCEPLL | PHY_DISABLECLK | PHY_RSTZ | PHY_SHUTDOWNZ);
	write32(&dsi->mipi_regs->dsi_clk_cfg,
		TO_CLK_DIVIDSION(10) |
		TX_ESC_CLK_DIVIDSION(esc_clk_division));
}

static void rk_mipi_message_config(struct rk_mipi_dsi *dsi)
{
	write32(&dsi->mipi_regs->dsi_lpclk_ctrl, 0);
	write32(&dsi->mipi_regs->dsi_cmd_mode_cfg, CMD_MODE_ALL_LP);
}

static int rk_mipi_dsi_check_fifo(struct rk_mipi_dsi *dsi, u32 flag)
{
	struct stopwatch sw;
	int val;

	stopwatch_init_msecs_expire(&sw, 20);
	do {
		val = read32(&dsi->mipi_regs->dsi_cmd_pkt_status);
		if (!(val & flag))
			return 0 ;
	} while (!stopwatch_expired(&sw));

	return -1;
}

static int rk_mipi_dsi_gen_pkt_hdr_write(struct rk_mipi_dsi *dsi, u32 hdr_val)
{
	int val;
	struct stopwatch sw;
	u32 mask;

	if (rk_mipi_dsi_check_fifo(dsi, GEN_CMD_FULL)) {
		printk(BIOS_ERR, "failed to get available command FIFO\n");
		return -1;
	}

	write32(&dsi->mipi_regs->dsi_gen_hdr, hdr_val);

	mask = GEN_CMD_EMPTY | GEN_PLD_W_EMPTY;
	stopwatch_init_msecs_expire(&sw, 20);
	do {
		val = read32(&dsi->mipi_regs->dsi_cmd_pkt_status);
		if ((val & mask) == mask)
			return 0 ;
	} while (!stopwatch_expired(&sw));
	printk(BIOS_ERR, "failed to write command FIFO\n");

	return -1;
}

static int rk_mipi_dsi_dcs_cmd(struct rk_mipi_dsi *dsi, u8 cmd)
{
	u32 val;

	rk_mipi_message_config(dsi);

	val = GEN_HDATA(cmd) | GEN_HTYPE(MIPI_DSI_DCS_SHORT_WRITE);

	return rk_mipi_dsi_gen_pkt_hdr_write(dsi, val);
}

static int rk_mipi_dsi_dci_long_write(struct rk_mipi_dsi *dsi,
				      char *data, u32 len)
{
	u32 remainder;
	int ret = 0;

	while (len) {
		if (len < 4) {
			remainder = 0;
			memcpy(&remainder, data, len);
			write32(&dsi->mipi_regs->dsi_gen_pld_data, remainder);
			len = 0;
		} else {
			remainder = *(u32 *)data;
			write32(&dsi->mipi_regs->dsi_gen_pld_data, remainder);
			data += 4;
			len -= 4;
		}

		ret = rk_mipi_dsi_check_fifo(dsi, GEN_PLD_W_FULL);
		if (ret) {
			printk(BIOS_ERR, "Failed to write fifo\n");
			return ret;
		}
	}

	return ret;
}

static int rk_mipi_dsi_write(struct rk_mipi_dsi *dsi, char *data, int len)
{
	u16 buf = 0;
	u32 val;
	int ret = 0;

	rk_mipi_message_config(dsi);

	switch (len) {
	case 0:
		die("not data!");
	case 1:
		val = GEN_HDATA(*data) |
		      GEN_HTYPE(MIPI_DSI_DCS_SHORT_WRITE);
		break;
	case 2:
		buf = *data++;
		buf |= *data << 8;
		val = GEN_HDATA(buf) |
		      GEN_HTYPE(MIPI_DSI_DCS_SHORT_WRITE_PARAM);
		break;
	default:
		ret = rk_mipi_dsi_dci_long_write(dsi, data, len);
		if (ret) {
			printk(BIOS_ERR, "error happened during long write\n");
			return ret;
		}
		val = GEN_HDATA(len) | GEN_HTYPE(MIPI_DSI_DCS_LONG_WRITE);
		break;
	}

	return rk_mipi_dsi_gen_pkt_hdr_write(dsi, val);
}

static void rk_mipi_enable(struct rk_mipi_dsi *dsi,
			   const struct edid *edid,
			   const struct mipi_panel_data *panel_data)
{
	if (rk_mipi_dsi_get_lane_bps(dsi, edid, panel_data) < 0)
		return;

	rk_mipi_dsi_init(dsi);
	rk_mipi_dsi_dpi_config(dsi);
	rk_mipi_dsi_packet_handler_config(dsi);
	rk_mipi_dsi_video_mode_config(dsi);
	rk_mipi_dsi_video_packet_config(dsi, edid, panel_data);
	rk_mipi_dsi_command_mode_config(dsi);
	rk_mipi_dsi_line_timer_config(dsi, edid);
	rk_mipi_dsi_vertical_timing_config(dsi, edid);
	rk_mipi_dsi_dphy_timing_config(dsi);
	rk_mipi_dsi_dphy_interface_config(dsi);
	rk_mipi_dsi_clear_err(dsi);
	if (rk_mipi_dsi_phy_init(dsi) < 0)
		return;
	rk_mipi_dsi_wait_for_two_frames(dsi, edid);

	rk_mipi_dsi_set_mode(dsi, MIPI_DSI_CMD_MODE);
}

void rk_mipi_prepare(const struct edid *edid,
		     const struct mipi_panel_data *panel_data)
{
	int i, num;
	struct panel_init_command *cmds;

	for (i = 0; i < panel_data->mipi_num; i++) {
		rk_mipi[i].lanes = panel_data->lanes / panel_data->mipi_num;
		rk_mipi[i].format = panel_data->format;
		rk_mipi_enable(&rk_mipi[i], edid, panel_data);
	}

	if (panel_data->init_cmd) {
		cmds = panel_data->init_cmd;
		for (num = 0; cmds[num].len != 0; num++) {
			struct panel_init_command *cmd = &cmds[num];
			for (i = 0; i < panel_data->mipi_num; i++) {
				if (rk_mipi_dsi_write(&rk_mipi[i], cmd->data,
						      cmd->len))
					return;

				/* make sure panel picks up the command */
				if (rk_mipi_dsi_dcs_cmd(&rk_mipi[i],
							MIPI_DCS_NOP))
					return;
			}
		}
	}

	for (i = 0; i < panel_data->mipi_num; i++) {
		if (rk_mipi_dsi_dcs_cmd(&rk_mipi[i],
					MIPI_DCS_EXIT_SLEEP_MODE) < 0)
			return;
	}
	udelay(panel_data->display_on_udelay);
	for (i = 0; i < panel_data->mipi_num; i++) {
		if (rk_mipi_dsi_dcs_cmd(&rk_mipi[i],
					MIPI_DCS_SET_DISPLAY_ON) < 0)
			return;
	}
	udelay(panel_data->video_mode_udelay);
	for (i = 0; i < panel_data->mipi_num; i++)
		rk_mipi_dsi_set_mode(&rk_mipi[i], MIPI_DSI_VID_MODE);
}
