/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>
#include <delay.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <device/device.h>
#include <edid.h>
#include <soc/nvidia/tegra/types.h>
#include <soc/nvidia/tegra/dc.h>
#include "chip.h"
#include <soc/display.h>
#include <soc/mipi_dsi.h>
#include <soc/mipi_display.h>
#include <soc/tegra_dsi.h>
#include <soc/mipi-phy.h>
#include "jdi_25x18_display/panel-jdi-lpm102a188a.h"
#include <commonlib/helpers.h>

struct tegra_mipi_device mipi_device_data[NUM_DSI];

struct tegra_dsi dsi_data[NUM_DSI] = {
	{
		.regs = (void *)TEGRA_DSIA_BASE,
		.channel = 0,
		.slave = &dsi_data[DSI_B],
		.master = NULL,
		.video_fifo_depth = MAX_DSI_VIDEO_FIFO_DEPTH,
		.host_fifo_depth = MAX_DSI_HOST_FIFO_DEPTH,
	},
	{
		.regs = (void *)TEGRA_DSIB_BASE,
		.channel = 0,
		.slave = NULL,
		.master = &dsi_data[DSI_A],
		.video_fifo_depth = MAX_DSI_VIDEO_FIFO_DEPTH,
		.host_fifo_depth = MAX_DSI_HOST_FIFO_DEPTH,
	},
};

static const u32 init_reg[] = {
	DSI_INT_ENABLE,
	DSI_INT_STATUS,
	DSI_INT_MASK,
	DSI_INIT_SEQ_DATA_0,
	DSI_INIT_SEQ_DATA_1,
	DSI_INIT_SEQ_DATA_2,
	DSI_INIT_SEQ_DATA_3,
	DSI_INIT_SEQ_DATA_4,
	DSI_INIT_SEQ_DATA_5,
	DSI_INIT_SEQ_DATA_6,
	DSI_INIT_SEQ_DATA_7,
	DSI_INIT_SEQ_DATA_15,
	DSI_DCS_CMDS,
	DSI_PKT_SEQ_0_LO,
	DSI_PKT_SEQ_1_LO,
	DSI_PKT_SEQ_2_LO,
	DSI_PKT_SEQ_3_LO,
	DSI_PKT_SEQ_4_LO,
	DSI_PKT_SEQ_5_LO,
	DSI_PKT_SEQ_0_HI,
	DSI_PKT_SEQ_1_HI,
	DSI_PKT_SEQ_2_HI,
	DSI_PKT_SEQ_3_HI,
	DSI_PKT_SEQ_4_HI,
	DSI_PKT_SEQ_5_HI,
	DSI_CONTROL,
	DSI_HOST_CONTROL,
	DSI_PAD_CONTROL_0,
	DSI_PAD_CONTROL_CD,
	DSI_SOL_DELAY,
	DSI_MAX_THRESHOLD,
	DSI_TRIGGER,
	DSI_TX_CRC,
	DSI_INIT_SEQ_CONTROL,
	DSI_PKT_LEN_0_1,
	DSI_PKT_LEN_2_3,
	DSI_PKT_LEN_4_5,
	DSI_PKT_LEN_6_7,
};

static inline struct tegra_dsi *host_to_tegra(struct mipi_dsi_host *host)
{
	return container_of(host, struct tegra_dsi, host);
}

/*
 * non-burst mode with sync pulses
 */
static const u32 pkt_seq_video_non_burst_sync_pulses[NUM_PKT_SEQ] = {
	[ 0] = PKT_ID0(MIPI_DSI_V_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0) |
	       PKT_LP,
	[ 1] = 0,
	[ 2] = PKT_ID0(MIPI_DSI_V_SYNC_END) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0) |
	       PKT_LP,
	[ 3] = 0,
	[ 4] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0) |
	       PKT_LP,
	[ 5] = 0,
	[ 6] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0),
	[ 7] = PKT_ID0(MIPI_DSI_BLANKING_PACKET) | PKT_LEN0(2) |
	       PKT_ID1(MIPI_DSI_PACKED_PIXEL_STREAM_24) | PKT_LEN1(3) |
	       PKT_ID2(MIPI_DSI_BLANKING_PACKET) | PKT_LEN2(4),
	[ 8] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0) |
	       PKT_LP,
	[ 9] = 0,
	[10] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(1) |
	       PKT_ID2(MIPI_DSI_H_SYNC_END) | PKT_LEN2(0),
	[11] = PKT_ID0(MIPI_DSI_BLANKING_PACKET) | PKT_LEN0(2) |
	       PKT_ID1(MIPI_DSI_PACKED_PIXEL_STREAM_24) | PKT_LEN1(3) |
	       PKT_ID2(MIPI_DSI_BLANKING_PACKET) | PKT_LEN2(4),
};

/*
 * non-burst mode with sync events
 */
static const u32 pkt_seq_video_non_burst_sync_events[NUM_PKT_SEQ] = {
	[ 0] = PKT_ID0(MIPI_DSI_V_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_END_OF_TRANSMISSION) | PKT_LEN1(7) |
	       PKT_LP,
	[ 1] = 0,
	[ 2] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_END_OF_TRANSMISSION) | PKT_LEN1(7) |
	       PKT_LP,
	[ 3] = 0,
	[ 4] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_END_OF_TRANSMISSION) | PKT_LEN1(7) |
	       PKT_LP,
	[ 5] = 0,

	[ 6] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(2) |
	       PKT_ID2(MIPI_DSI_PACKED_PIXEL_STREAM_24) | PKT_LEN2(3),

	[ 7] = PKT_ID0(MIPI_DSI_BLANKING_PACKET) | PKT_LEN0(4),
	[ 8] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_END_OF_TRANSMISSION) | PKT_LEN1(7) |
	       PKT_LP,
	[ 9] = 0,

	[10] = PKT_ID0(MIPI_DSI_H_SYNC_START) | PKT_LEN0(0) |
	       PKT_ID1(MIPI_DSI_BLANKING_PACKET) | PKT_LEN1(2) |
	       PKT_ID2(MIPI_DSI_PACKED_PIXEL_STREAM_24) | PKT_LEN2(3),

	[11] = PKT_ID0(MIPI_DSI_BLANKING_PACKET) | PKT_LEN0(4),
};

static const u32 pkt_seq_command_mode[NUM_PKT_SEQ] = {
	[ 0] = 0,
	[ 1] = 0,
	[ 2] = 0,
	[ 3] = 0,
	[ 4] = 0,
	[ 5] = 0,
	[ 6] = PKT_ID0(MIPI_DSI_DCS_LONG_WRITE) | PKT_LEN0(3) | PKT_LP,
	[ 7] = 0,
	[ 8] = 0,
	[ 9] = 0,
	[10] = PKT_ID0(MIPI_DSI_DCS_LONG_WRITE) | PKT_LEN0(5) | PKT_LP,
	[11] = 0,
};

static int tegra_dsi_set_phy_timing(struct tegra_dsi *dsi)
{
	int err;

	err = mipi_dphy_set_timing(dsi);
	if (err < 0) {
		printk(BIOS_ERR, "failed to set D-PHY timing: %d\n", err);
		return err;
	}

	if (dsi->slave)
		tegra_dsi_set_phy_timing(dsi->slave);
	return 0;
}

static int tegra_dsi_get_muldiv(enum mipi_dsi_pixel_format format,
				unsigned int *mulp, unsigned int *divp)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB666_PACKED:
	case MIPI_DSI_FMT_RGB888:
		*mulp = 3;
		*divp = 1;
		break;

	case MIPI_DSI_FMT_RGB565:
		*mulp = 2;
		*divp = 1;
		break;

	case MIPI_DSI_FMT_RGB666:
		*mulp = 9;
		*divp = 4;
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

static int tegra_dsi_get_format(enum mipi_dsi_pixel_format format,
				enum tegra_dsi_format *fmt)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB888:
		*fmt = TEGRA_DSI_FORMAT_24P;
		break;

	case MIPI_DSI_FMT_RGB666:
		*fmt = TEGRA_DSI_FORMAT_18NP;
		break;

	case MIPI_DSI_FMT_RGB666_PACKED:
		*fmt = TEGRA_DSI_FORMAT_18P;
		break;

	case MIPI_DSI_FMT_RGB565:
		*fmt = TEGRA_DSI_FORMAT_16P;
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

static void tegra_dsi_ganged_enable(struct tegra_dsi *dsi, unsigned int start,
				    unsigned int size)
{
	u32 value;

	tegra_dsi_writel(dsi, start, DSI_GANGED_MODE_START);
	tegra_dsi_writel(dsi, size << 16 | size, DSI_GANGED_MODE_SIZE);

	value = DSI_GANGED_MODE_CONTROL_ENABLE;
	tegra_dsi_writel(dsi, value, DSI_GANGED_MODE_CONTROL);
}

static void tegra_dsi_enable(struct tegra_dsi *dsi)
{
	u32 value;

	value = tegra_dsi_readl(dsi, DSI_POWER_CONTROL);
	value |= DSI_POWER_CONTROL_ENABLE;
	tegra_dsi_writel(dsi, value, DSI_POWER_CONTROL);

	if (dsi->slave)
		tegra_dsi_enable(dsi->slave);
}

static int tegra_dsi_configure(struct tegra_dsi *dsi, unsigned int pipe,
			const struct soc_nvidia_tegra210_config *mode)
{
	unsigned int hact, hsw, hbp, hfp, i, mul, div;
	enum tegra_dsi_format format;
	const u32 *pkt_seq;
	u32 value;
	int err;

	if (dsi->flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE) {
		printk(BIOS_SPEW, "Non-burst video mode with sync pulses\n");
		pkt_seq = pkt_seq_video_non_burst_sync_pulses;
	} else if (dsi->flags & MIPI_DSI_MODE_VIDEO) {
		printk(BIOS_SPEW, "Non-burst video mode with sync events\n");
		pkt_seq = pkt_seq_video_non_burst_sync_events;
	} else {
		printk(BIOS_SPEW, "Command mode\n");
		pkt_seq = pkt_seq_command_mode;
	}

	err = tegra_dsi_get_muldiv(dsi->format, &mul, &div);
	if (err < 0)
		return err;

	err = tegra_dsi_get_format(dsi->format, &format);
	if (err < 0)
		return err;

	value = DSI_CONTROL_CHANNEL(0) | DSI_CONTROL_FORMAT(format) |
		DSI_CONTROL_LANES(dsi->lanes - 1) |
		DSI_CONTROL_SOURCE(pipe);
	tegra_dsi_writel(dsi, value, DSI_CONTROL);

	tegra_dsi_writel(dsi, dsi->video_fifo_depth, DSI_MAX_THRESHOLD);

	value = DSI_HOST_CONTROL_HS;
	tegra_dsi_writel(dsi, value, DSI_HOST_CONTROL);

	value = tegra_dsi_readl(dsi, DSI_CONTROL);

	if (dsi->flags & MIPI_DSI_CLOCK_NON_CONTINUOUS)
		value |= DSI_CONTROL_HS_CLK_CTRL;

	value &= ~DSI_CONTROL_TX_TRIG(3);

	/* enable DCS commands for command mode */
	if (dsi->flags & MIPI_DSI_MODE_VIDEO)
		value &= ~DSI_CONTROL_DCS_ENABLE;
	else
		value |= DSI_CONTROL_DCS_ENABLE;

	value |= DSI_CONTROL_VIDEO_ENABLE;
	value &= ~DSI_CONTROL_HOST_ENABLE;
	tegra_dsi_writel(dsi, value, DSI_CONTROL);

	for (i = 0; i < NUM_PKT_SEQ; i++)
		tegra_dsi_writel(dsi, pkt_seq[i], DSI_PKT_SEQ_0_LO + i);

	if (dsi->flags & MIPI_DSI_MODE_VIDEO) {
		/* horizontal active pixels */
		hact = mode->xres * mul / div;

		/* horizontal sync width */
		hsw = (hsync_end(mode) - hsync_start(mode)) * mul / div;

		/* horizontal back porch */
		hbp = (htotal(mode) - hsync_end(mode)) * mul / div;
		if ((dsi->flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE) == 0)
			hbp += hsw;

		/* horizontal front porch */
		hfp = (hsync_start(mode) - mode->xres) * mul / div;

		/* subtract packet overhead */
		hsw -= 10;
		hbp -= 14;
		hfp -= 8;

		tegra_dsi_writel(dsi, hsw << 16 | 0, DSI_PKT_LEN_0_1);
		tegra_dsi_writel(dsi, hact << 16 | hbp, DSI_PKT_LEN_2_3);
		tegra_dsi_writel(dsi, hfp, DSI_PKT_LEN_4_5);
		tegra_dsi_writel(dsi, 0x0f0f << 16, DSI_PKT_LEN_6_7);

		/* set SOL delay (for non-burst mode only) */
		tegra_dsi_writel(dsi, 8 * mul / div, DSI_SOL_DELAY);

		/* TODO: implement ganged mode */
	} else {
		u16 bytes;
		if (dsi->ganged_mode) {
			/*
			 * For ganged mode, assume symmetric left-right mode.
			 */
			bytes = 1 + (mode->xres / 2) * mul / div;
		} else {
			/* 1 byte (DCS command) + pixel data */
			bytes = 1 + mode->xres * mul / div;
		}

		tegra_dsi_writel(dsi, 0, DSI_PKT_LEN_0_1);
		tegra_dsi_writel(dsi, bytes << 16, DSI_PKT_LEN_2_3);
		tegra_dsi_writel(dsi, bytes << 16, DSI_PKT_LEN_4_5);
		tegra_dsi_writel(dsi, 0, DSI_PKT_LEN_6_7);

		value = MIPI_DCS_WRITE_MEMORY_START << 8 |
			MIPI_DCS_WRITE_MEMORY_CONTINUE;
		tegra_dsi_writel(dsi, value, DSI_DCS_CMDS);

		/* set SOL delay */
		if (dsi->ganged_mode) {
			unsigned long delay, bclk, bclk_ganged;
			unsigned int lanes = dsi->ganged_lanes;

			/* SOL to valid, valid to FIFO and FIFO write delay */
			delay = 4 + 4 + 2;
			delay = DIV_ROUND_UP(delay * mul, div * lanes);
			/* FIFO read delay */
			delay = delay + 6;

			bclk = DIV_ROUND_UP(htotal(mode) * mul, div * lanes);
			bclk_ganged = DIV_ROUND_UP(bclk * lanes / 2, lanes);
			value = bclk - bclk_ganged + delay + 20;
		} else {
			/* TODO: revisit for non-ganged mode */
			value = 8 * mul / div;
		}

		tegra_dsi_writel(dsi, value, DSI_SOL_DELAY);
	}

	if (dsi->slave) {
		err = tegra_dsi_configure(dsi->slave, pipe, mode);
		if (err < 0)
			return err;

		/*
		 * enable ganged mode
		 */
		if (dsi->ganged_mode) {
			tegra_dsi_ganged_enable(dsi, mode->xres / 2,
					mode->xres / 2);
			tegra_dsi_ganged_enable(dsi->slave, 0, mode->xres / 2);
		}
	}
	return 0;
}

static int tegra_output_dsi_enable(struct tegra_dsi *dsi,
			const struct soc_nvidia_tegra210_config *config)
{
	int err;

	if (dsi->enabled)
		return 0;

	err = tegra_dsi_configure(dsi, 0, config);
	if (err < 0) {
		printk(BIOS_ERR, "DSI configuration failed\n");
		return err;
	}

	/* enable DSI controller */
	tegra_dsi_enable(dsi);

	dsi->enabled = true;
	return 0;
}


static void tegra_dsi_set_timeout(struct tegra_dsi *dsi, unsigned long bclk,
				  unsigned int vrefresh)
{
	unsigned int timeout;
	u32 value;

	/* one frame high-speed transmission timeout */
	timeout = (bclk / vrefresh) / 512;
	value = DSI_TIMEOUT_LRX(0x2000) | DSI_TIMEOUT_HTX(timeout);
	tegra_dsi_writel(dsi, value, DSI_TIMEOUT_0);

	/* 2 ms peripheral timeout for panel */
	timeout = 2 * bclk / 512 * 1000;
	value = DSI_TIMEOUT_PR(timeout) | DSI_TIMEOUT_TA(0x2000);
	tegra_dsi_writel(dsi, value, DSI_TIMEOUT_1);

	value = DSI_TALLY_TA(0) | DSI_TALLY_LRX(0) | DSI_TALLY_HTX(0);
	tegra_dsi_writel(dsi, value, DSI_TO_TALLY);

	if (dsi->slave)
		tegra_dsi_set_timeout(dsi->slave, bclk, vrefresh);
}

static int tegra_output_dsi_setup_clock(struct tegra_dsi *dsi,
			const struct soc_nvidia_tegra210_config *config)
{
	unsigned int mul, div, num_lanes;
	unsigned long bclk;
	unsigned long pclk = config->pixel_clock;
	int plld;
	int err;
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;
	unsigned int shift_clk_div;

	err = tegra_dsi_get_muldiv(dsi->format, &mul, &div);
	if (err < 0)
		return err;

	/*
	 * In ganged mode, account for the total number of lanes across both
	 * DSI channels so that the bit clock is properly computed.
	 */
	if (dsi->ganged_mode)
		num_lanes = dsi->ganged_lanes;
	else
		num_lanes = dsi->lanes;

	/* compute byte clock */
	bclk = (pclk * mul) / (div * num_lanes);

	/*
	 * Compute bit clock and round up to the next MHz.
	 */
	plld = DIV_ROUND_UP(bclk * 8, USECS_PER_SEC) * USECS_PER_SEC;

	/*
	 * the actual rate on PLLD_OUT0 is 1/2 plld
	 */
	dsi->clk_rate = plld / 2;
	if (dsi->slave)
		dsi->slave->clk_rate = dsi->clk_rate;

	/* set up plld */
	plld = clock_configure_plld(plld);
	if (plld == 0) {
		printk(BIOS_ERR, "%s: clock init failed\n", __func__);
		return -1;
	} else
		printk(BIOS_INFO, "%s:  plld is configured to: %u\n",
			 __func__, plld);

	/*
	 * Derive pixel clock from bit clock using the shift clock divider.
	 * Note that this is only half of what we would expect, but we need
	 * that to make up for the fact that we divided the bit clock by a
	 * factor of two above.
	 */
	shift_clk_div = ((8 * mul) / (div * num_lanes)) - 2;
	update_display_shift_clock_divider(disp_ctrl, shift_clk_div);

	tegra_dsi_set_timeout(dsi, bclk, config->refresh);
	return plld/1000000;
}



static int tegra_dsi_pad_enable(struct tegra_dsi *dsi)
{
	unsigned long value;

	value = DSI_PAD_CONTROL_VS1_PULLDN(0) | DSI_PAD_CONTROL_VS1_PDIO(0);
	tegra_dsi_writel(dsi, value, DSI_PAD_CONTROL_0);
	return 0;
}

static int tegra_dsi_pad_calibrate(struct tegra_dsi *dsi)
{
	u32 value;

	tegra_dsi_writel(dsi, 0, DSI_PAD_CONTROL_0);
	tegra_dsi_writel(dsi, 0, DSI_PAD_CONTROL_1);
	tegra_dsi_writel(dsi, 0, DSI_PAD_CONTROL_2);
	tegra_dsi_writel(dsi, 0, DSI_PAD_CONTROL_3);
	tegra_dsi_writel(dsi, 0, DSI_PAD_CONTROL_4);

	/* start calibration */
	tegra_dsi_pad_enable(dsi);

	value = DSI_PAD_SLEW_UP(0x7) | DSI_PAD_SLEW_DN(0x7) |
		DSI_PAD_LP_UP(0x1) | DSI_PAD_LP_DN(0x1) |
		DSI_PAD_OUT_CLK(0x0);
	tegra_dsi_writel(dsi, value, DSI_PAD_CONTROL_2);

	value = DSI_PAD_PREEMP_PD_CLK(0x3) | DSI_PAD_PREEMP_PU_CLK(0x3) |
		DSI_PAD_PREEMP_PD(0x03) | DSI_PAD_PREEMP_PU(0x3);
	tegra_dsi_writel(dsi, value, DSI_PAD_CONTROL_3);

	return tegra_mipi_calibrate(dsi->mipi);
}

static const char *const error_report[16] = {
	"SoT Error",
	"SoT Sync Error",
	"EoT Sync Error",
	"Escape Mode Entry Command Error",
	"Low-Power Transmit Sync Error",
	"Peripheral Timeout Error",
	"False Control Error",
	"Contention Detected",
	"ECC Error, single-bit",
	"ECC Error, multi-bit",
	"Checksum Error",
	"DSI Data Type Not Recognized",
	"DSI VC ID Invalid",
	"Invalid Transmission Length",
	"Reserved",
	"DSI Protocol Violation",
};

static int tegra_dsi_read_response(struct tegra_dsi *dsi,
				   const struct mipi_dsi_msg *msg,
				   unsigned int count)
{
	u8 *rx = msg->rx_buf;
	unsigned int i, j, k;
	size_t size = 0;
	u16 errors;
	u32 value;

	/* read and parse packet header */
	value = tegra_dsi_readl(dsi, DSI_RD_DATA);

	switch (value & 0x3f) {
	case MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT:
		errors = (value >> 8) & 0xffff;
		printk(BIOS_ERR, "Acknowledge and error report: %04x\n",
			errors);
		for (i = 0; i < ARRAY_SIZE(error_report); i++)
			if (errors & BIT(i))
				printk(BIOS_INFO, "  %2u: %s\n", i,
					error_report[i]);
		break;

	case MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_1BYTE:
		rx[0] = (value >> 8) & 0xff;
		break;

	case MIPI_DSI_RX_DCS_SHORT_READ_RESPONSE_2BYTE:
		rx[0] = (value >>  8) & 0xff;
		rx[1] = (value >> 16) & 0xff;
		break;

	case MIPI_DSI_RX_DCS_LONG_READ_RESPONSE:
		size = ((value >> 8) & 0xff00) | ((value >> 8) & 0xff);
		break;

	case MIPI_DSI_RX_GENERIC_LONG_READ_RESPONSE:
		size = ((value >> 8) & 0xff00) | ((value >> 8) & 0xff);
		break;

	default:
		printk(BIOS_ERR, "unhandled response type: %02x\n",
			value & 0x3f);
		break;
	}

	size = MIN(size, msg->rx_len);

	if (msg->rx_buf && size > 0) {
		for (i = 0, j = 0; i < count - 1; i++, j += 4) {
			value = tegra_dsi_readl(dsi, DSI_RD_DATA);

			for (k = 0; k < 4 && (j + k) < msg->rx_len; k++)
				rx[j + k] = (value >> (k << 3)) & 0xff;
		}
	}
	return 0;
}

static int tegra_dsi_transmit(struct tegra_dsi *dsi, unsigned long timeout_ms)
{
	u32 poll_interval_us = 2000;
	u32 timeout_us = timeout_ms * 1000;

	tegra_dsi_writel(dsi, DSI_TRIGGER_HOST, DSI_TRIGGER);
	udelay(poll_interval_us);

	do {
		u32 value = tegra_dsi_readl(dsi, DSI_TRIGGER);
		if ((value & DSI_TRIGGER_HOST) == 0)
			return 0;

		//usleep_range(1000, 2000);
		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;

		udelay(poll_interval_us);
	} while (1);

	printk(BIOS_ERR, "%s: ERROR: timeout waiting for transmission"
			" to complete\n", __func__);
	return -ETIMEDOUT;
}

static int tegra_dsi_wait_for_response(struct tegra_dsi *dsi,
				       unsigned long timeout_ms)
{
	u32 poll_interval_us = 2000;
	u32 timeout_us = timeout_ms * 1000;

	do {
		u32 value = tegra_dsi_readl(dsi, DSI_STATUS);
		u8 count = value & 0x1f;

		if (count > 0)
			return count;

		if (timeout_us > poll_interval_us)
			timeout_us -= poll_interval_us;
		else
			break;

		udelay(poll_interval_us);
	} while (1);

	printk(BIOS_ERR, "%s: ERROR: timeout\n", __func__);

	return -ETIMEDOUT;
}

static ssize_t tegra_dsi_host_transfer(struct mipi_dsi_host *host,
				const struct mipi_dsi_msg *msg)
{
	struct tegra_dsi *dsi = host_to_tegra(host);
	const u8 *tx = msg->tx_buf;
	unsigned int count, i, j;
	u32 value;
	int err;

	if (msg->tx_len > dsi->video_fifo_depth * 4)
		return -ENOSPC;

	/* reset underflow/overflow flags */
	value = tegra_dsi_readl(dsi, DSI_STATUS);
	if (value & (DSI_STATUS_UNDERFLOW | DSI_STATUS_OVERFLOW)) {
		value = DSI_HOST_CONTROL_FIFO_RESET;
		tegra_dsi_writel(dsi, value, DSI_HOST_CONTROL);
		udelay(20);	// usleep_range(10, 20);
	}

	value = tegra_dsi_readl(dsi, DSI_POWER_CONTROL);
	value |= DSI_POWER_CONTROL_ENABLE;
	tegra_dsi_writel(dsi, value, DSI_POWER_CONTROL);

	udelay(7000);	//usleep_range(5000, 10000);

	value = DSI_HOST_CONTROL_CRC_RESET | DSI_HOST_CONTROL_TX_TRIG_HOST |
		DSI_HOST_CONTROL_CS | DSI_HOST_CONTROL_ECC;

	if ((msg->flags & MIPI_DSI_MSG_USE_LPM) == 0)
		value |= DSI_HOST_CONTROL_HS;

	/*
	 * The host FIFO has a maximum of 64 words, so larger transmissions
	 * need to use the video FIFO.
	 */
	if (msg->tx_len > dsi->host_fifo_depth * 4)
		value |= DSI_HOST_CONTROL_FIFO_SEL;

	tegra_dsi_writel(dsi, value, DSI_HOST_CONTROL);

	/*
	 * For reads and messages with explicitly requested ACK, generate a
	 * BTA sequence after the transmission of the packet.
	 */
	if ((msg->flags & MIPI_DSI_MSG_REQ_ACK) ||
	    (msg->rx_buf && msg->rx_len > 0)) {
		value = tegra_dsi_readl(dsi, DSI_HOST_CONTROL);
		value |= DSI_HOST_CONTROL_PKT_BTA;
		tegra_dsi_writel(dsi, value, DSI_HOST_CONTROL);
	}

	value = DSI_CONTROL_LANES(0) | DSI_CONTROL_HOST_ENABLE;
	tegra_dsi_writel(dsi, value, DSI_CONTROL);

	/* write packet header */
	value = ((msg->channel & 0x3) << 6) | (msg->type & 0x3f);

	if (tx && msg->tx_len > 0)
		value |= tx[0] <<  8;

	if (tx && msg->tx_len > 1)
		value |= tx[1] << 16;

	tegra_dsi_writel(dsi, value, DSI_WR_DATA);

	/* write payload (if any) */
	if (msg->tx_len > 2) {
		for (j = 2; j < msg->tx_len; j += 4) {
			value = 0;

			for (i = 0; i < 4 && j + i < msg->tx_len; i++)
				value |= tx[j + i] << (i << 3);

			tegra_dsi_writel(dsi, value, DSI_WR_DATA);
		}
	}

	err = tegra_dsi_transmit(dsi, 250);
	if (err < 0) {
		printk(BIOS_INFO, "Failed to transmit. %d\n", err);
		return err;
	}

	if ((msg->flags & MIPI_DSI_MSG_REQ_ACK) ||
	    (msg->rx_buf && msg->rx_len > 0)) {
		err = tegra_dsi_wait_for_response(dsi, 250);
		if (err < 0) {
			printk(BIOS_INFO, "Failed to read response. %d\n", err);
			return err;
		}
		count = err;

		value = tegra_dsi_readl(dsi, DSI_RD_DATA);
		switch (value) {
		case 0x84:
			printk(BIOS_INFO, "ACK\n");
			break;

		case 0x87:
			printk(BIOS_INFO, "ESCAPE\n");
			break;

		default:
			printk(BIOS_INFO, "unknown status: %08x\n", value);
			break;
		}

		if (count > 1) {
			err = tegra_dsi_read_response(dsi, msg, count);
			if (err < 0)
				printk(BIOS_INFO,
					"failed to parse response: %d\n",
					err);
			else {
				/*
				 * For read commands, return the number of
				 * bytes returned by the peripheral.
				 */
				count = err;
			}
		}
	} else {
		/*
		 * For write commands, we have transmitted the 4-byte header
		 * plus the variable-length payload.
		 */
		count = 4 + msg->tx_len;
	}

	return count;
}

static int tegra_dsi_ganged_setup(struct tegra_dsi *dsi,
			  struct tegra_dsi *slave)
{
	/*
	 * The number of ganged lanes is the sum of lanes of all peripherals
	 * in the gang.
	 */
	dsi->slave->ganged_lanes = dsi->lanes + dsi->slave->lanes;
	dsi->slave->ganged_mode = 1;

	dsi->ganged_lanes = dsi->lanes + dsi->slave->lanes;
	dsi->ganged_mode = 1;
	return 0;
}

static int tegra_dsi_host_attach(struct mipi_dsi_host *host,
				struct mipi_dsi_device *device)
{
	struct tegra_dsi *dsi = host_to_tegra(host);
	int err;

	dsi->flags = device->mode_flags;
	dsi->format = device->format;
	dsi->lanes = device->lanes;

	if (dsi->master) {
		err = tegra_dsi_ganged_setup(dsi->master, dsi);
		if (err < 0) {
			printk(BIOS_ERR, "failed to set up ganged mode: %d\n",
				err);
			return err;
		}
	}
	return 0;
}

static const struct mipi_dsi_host_ops tegra_dsi_host_ops = {
	.attach = tegra_dsi_host_attach,
	.transfer = tegra_dsi_host_transfer,
};

static int dsi_probe_if(int dsi_index,
			struct soc_nvidia_tegra210_config *config)
{
	struct tegra_dsi *dsi = &dsi_data[dsi_index];
	int err;

	tegra_dsi_writel(dsi, 0, DSI_INIT_SEQ_CONTROL);

	/*
	 * Set default value. Will be taken from attached device once detected
	 */
        dsi->flags = 0;
        dsi->format = MIPI_DSI_FMT_RGB888;
        dsi->lanes = 4;

	/* get tegra_mipi_device */
        dsi->mipi = tegra_mipi_request(&mipi_device_data[dsi_index], dsi_index);

	/* calibrate */
	err = tegra_dsi_pad_calibrate(dsi);
	if (err < 0) {
		printk(BIOS_ERR, "MIPI calibration failed: %d\n", err);
		return err;
	}

	dsi->host.ops = &tegra_dsi_host_ops;
	err = mipi_dsi_host_register(&dsi->host);
	if (err < 0) {
		printk(BIOS_ERR, "failed to register DSI host: %d\n", err);
		return err;
	}

	/* get panel */
	dsi->panel = panel_jdi_dsi_probe((struct mipi_dsi_device *)dsi->host.dev);
	if (IS_ERR_PTR(dsi->panel)) {
		printk(BIOS_ERR, "failed to get dsi panel\n");
		return -EPTR;
	}
	dsi->panel->mode = config;
	return 0;
}

static int dsi_probe(struct soc_nvidia_tegra210_config *config)
{
	dsi_probe_if(DSI_A, config);
	dsi_probe_if(DSI_B, config);
	return 0;
}

static void tegra_dsi_init_regs(struct tegra_dsi *dsi)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(init_reg); i++)
		tegra_dsi_writel(dsi, 0, init_reg[i]);

	if (dsi->slave)
		tegra_dsi_init_regs(dsi->slave);
}

static int dsi_enable(struct soc_nvidia_tegra210_config *config)
{
	struct tegra_dsi *dsi_a = &dsi_data[DSI_A];

	dsi_probe(config);

	/* set up clock and TimeOutRegisters */
	tegra_output_dsi_setup_clock(dsi_a, config);

	/* configure APB_MISC_GP_MIPI_PAD_CTRL_0 */
	write32((unsigned int *)APB_MISC_GP_MIPI_PAD_CTRL_0, DSIB_MODE_DSI);

	/* configure phy interface timing registers */
	tegra_dsi_set_phy_timing(dsi_a);

	/* Initialize DSI registers */
	tegra_dsi_init_regs(dsi_a);

	/* prepare panel */
	panel_jdi_prepare(dsi_a->panel);

	/* enable dsi */
	if (tegra_output_dsi_enable(dsi_a, config)) {
		printk(BIOS_ERR,"%s: Error: failed to enable dsi output.\n",
			__func__);
		return -1;
	}

	return 0;
}

void dsi_display_startup(struct device *dev)
{
	struct soc_nvidia_tegra210_config *config = dev->chip_info;
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;
	u32 plld_rate;

	u32 framebuffer_size_mb = config->framebuffer_size / MiB;
	u32 framebuffer_base_mb= config->framebuffer_base / MiB;

	printk(BIOS_INFO, "%s: entry: disp_ctrl: %p.\n",
		 __func__, disp_ctrl);

	if (disp_ctrl == NULL) {
		printk(BIOS_ERR, "Error: No dc is assigned by dt.\n");
		return;
	}

	if (framebuffer_size_mb == 0){
		framebuffer_size_mb = ALIGN_UP(config->display_xres *
			config->display_yres *
			(config->framebuffer_bits_per_pixel / 8), MiB)/MiB;
	}

	config->framebuffer_size = framebuffer_size_mb * MiB;
	config->framebuffer_base = framebuffer_base_mb * MiB;

	/*
	 * The plld is programmed with the assumption of the SHIFT_CLK_DIVIDER
	 * and PIXEL_CLK_DIVIDER are zero (divide by 1). See the
	 * update_display_mode() for detail.
	 */
	/* set default plld */
	plld_rate = clock_configure_plld(config->pixel_clock * 2);
	if (plld_rate == 0) {
		printk(BIOS_ERR, "dc: clock init failed\n");
		return;
	}

	/* set disp1's clock source to PLLD_OUT0 */
	clock_configure_source(disp1, PLLD_OUT0, (plld_rate/KHz));

	/* Init dc */
	if (tegra_dc_init(disp_ctrl)) {
		printk(BIOS_ERR, "dc: init failed\n");
		return;
	}

	/* Configure dc mode */
	if (update_display_mode(disp_ctrl, config)) {
		printk(BIOS_ERR, "dc: failed to configure display mode.\n");
		return;
	}

	/* Configure and enable dsi controller and panel */
	if (dsi_enable(config)) {
		printk(BIOS_ERR, "%s: failed to enable dsi controllers.\n",
			__func__);
		return;
	}

	/* Set up window */
	update_window(config);
	printk(BIOS_INFO, "%s: display init done.\n", __func__);

	/* Save panel information to cb tables */
	pass_mode_info_to_payload(config);

	/*
	 * After this point, it is payload's responsibility to allocate
	 * framebuffer and sets the base address to dc's
	 * WINBUF_START_ADDR register and enables window by setting dc's
	 * DISP_DISP_WIN_OPTIONS register.
	 */
}
