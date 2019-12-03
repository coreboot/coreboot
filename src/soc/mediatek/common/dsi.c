/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <assert.h>
#include <device/mmio.h>
#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <edid.h>
#include <soc/dsi.h>
#include <string.h>
#include <timer.h>

static unsigned int mtk_dsi_get_bits_per_pixel(u32 format)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB565:
		return 16;
	case MIPI_DSI_FMT_RGB666:
	case MIPI_DSI_FMT_RGB666_PACKED:
		return 18;
	case MIPI_DSI_FMT_RGB888:
		return 24;
	}
	printk(BIOS_WARNING, "%s: WARN: Unknown format %d, assuming 24 bpp\n",
	       __func__, format);
	return 24;
}

static int mtk_dsi_get_data_rate(u32 bits_per_pixel, u32 lanes,
				 const struct edid *edid)
{
	/* data_rate = pixel_clock * bits_per_pixel * mipi_ratio / lanes
	 * Note pixel_clock comes in kHz and returned data_rate is in Mbps.
	 * mipi_ratio is the clk coefficient to balance the pixel clk in MIPI
	 * for older platforms which do not have complete implementation in HFP.
	 * Newer platforms should just set that to 1.0 (100 / 100).
	 */
	int data_rate = (u64)edid->mode.pixel_clock * bits_per_pixel *
			MTK_DSI_MIPI_RATIO_NUMERATOR /
			(1000 * lanes * MTK_DSI_MIPI_RATIO_DENOMINATOR);
	printk(BIOS_INFO, "DSI data_rate: %d Mbps\n", data_rate);

	if (data_rate < MTK_DSI_DATA_RATE_MIN_MHZ) {
		printk(BIOS_ERR, "data rate (%dMbps) must be >=%dMbps. "
		       "Please check the pixel clock (%u), bits per pixel(%u), "
		       "mipi_ratio (%d%%) and number of lanes (%d)\n",
		       data_rate, MTK_DSI_DATA_RATE_MIN_MHZ,
		       edid->mode.pixel_clock, bits_per_pixel,
		       (100 * MTK_DSI_MIPI_RATIO_NUMERATOR /
			MTK_DSI_MIPI_RATIO_DENOMINATOR), lanes);
		return -1;
	}
	return data_rate;
}

__weak void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing)
{
	/* Do nothing. */
}

static void mtk_dsi_phy_timing(int data_rate, struct mtk_phy_timing *phy_timing)
{
	u32 cycle_time, ui;

	ui = 1000 / data_rate + 0x01;
	cycle_time = 8000 / data_rate + 0x01;

	memset(phy_timing, 0, sizeof(*phy_timing));

	phy_timing->lpx = DIV_ROUND_UP(60, cycle_time);
	phy_timing->da_hs_prepare = DIV_ROUND_UP((50 + 5 * ui), cycle_time);
	phy_timing->da_hs_zero = DIV_ROUND_UP((110 + 6 * ui), cycle_time);
	phy_timing->da_hs_trail = DIV_ROUND_UP(((4 * ui) + 77), cycle_time);

	phy_timing->ta_go = 4U * phy_timing->lpx;
	phy_timing->ta_sure = 3U * phy_timing->lpx / 2U;
	phy_timing->ta_get = 5U * phy_timing->lpx;
	phy_timing->da_hs_exit = 2U * phy_timing->lpx;

	phy_timing->da_hs_sync = 1;
	phy_timing->clk_hs_zero = DIV_ROUND_UP(0x150U, cycle_time);
	phy_timing->clk_hs_trail = DIV_ROUND_UP(0x64U, cycle_time) + 0xaU;

	phy_timing->clk_hs_prepare = DIV_ROUND_UP(0x40U, cycle_time);
	phy_timing->clk_hs_post = DIV_ROUND_UP(80U + 52U * ui, cycle_time);
	phy_timing->clk_hs_exit = 2U * phy_timing->lpx;

	/* Allow board-specific tuning. */
	mtk_dsi_override_phy_timing(phy_timing);

	u32 timcon0, timcon1, timcon2, timcon3;

	timcon0 = phy_timing->lpx | phy_timing->da_hs_prepare << 8 |
		  phy_timing->da_hs_zero << 16 | phy_timing->da_hs_trail << 24;
	timcon1 = phy_timing->ta_go | phy_timing->ta_sure << 8 |
		  phy_timing->ta_get << 16 | phy_timing->da_hs_exit << 24;
	timcon2 = phy_timing->da_hs_sync << 8 | phy_timing->clk_hs_zero << 16 |
		  phy_timing->clk_hs_trail << 24;
	timcon3 = phy_timing->clk_hs_prepare | phy_timing->clk_hs_post << 8 |
		  phy_timing->clk_hs_exit << 16;

	write32(&dsi0->dsi_phy_timecon0, timcon0);
	write32(&dsi0->dsi_phy_timecon1, timcon1);
	write32(&dsi0->dsi_phy_timecon2, timcon2);
	write32(&dsi0->dsi_phy_timecon3, timcon3);
}

static void mtk_dsi_clk_hs_mode_enable(void)
{
	setbits32(&dsi0->dsi_phy_lccon, LC_HS_TX_EN);
}

static void mtk_dsi_clk_hs_mode_disable(void)
{
	clrbits32(&dsi0->dsi_phy_lccon, LC_HS_TX_EN);
}

static void mtk_dsi_set_mode(u32 mode_flags)
{
	u32 tmp_reg1 = 0;

	if (mode_flags & MIPI_DSI_MODE_VIDEO) {
		tmp_reg1 = SYNC_PULSE_MODE;

		if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
			tmp_reg1 = BURST_MODE;

		if (mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
			tmp_reg1 = SYNC_PULSE_MODE;
	}

	write32(&dsi0->dsi_mode_ctrl, tmp_reg1);
}

static void mtk_dsi_rxtx_control(u32 mode_flags, u32 lanes)
{
	u32 tmp_reg = 0;

	switch (lanes) {
	case 1:
		tmp_reg = 1 << 2;
		break;
	case 2:
		tmp_reg = 3 << 2;
		break;
	case 3:
		tmp_reg = 7 << 2;
		break;
	case 4:
	default:
		tmp_reg = 0xf << 2;
		break;
	}

	tmp_reg |= (mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS) << 6;
	tmp_reg |= (mode_flags & MIPI_DSI_MODE_EOT_PACKET) >> 3;

	write32(&dsi0->dsi_txrx_ctrl, tmp_reg);
}

static void mtk_dsi_config_vdo_timing(u32 mode_flags, u32 format, u32 lanes,
				      const struct edid *edid,
				      const struct mtk_phy_timing *phy_timing)
{
	u32 hsync_active_byte;
	u32 hbp_byte;
	u32 hfp_byte;
	u32 vbp_byte;
	u32 vfp_byte;
	u32 bytes_per_pixel;
	u32 packet_fmt;
	u32 hactive;
	u32 data_phy_cycles;

	bytes_per_pixel = DIV_ROUND_UP(mtk_dsi_get_bits_per_pixel(format), 8);
	vbp_byte = edid->mode.vbl - edid->mode.vso - edid->mode.vspw -
		   edid->mode.vborder;
	vfp_byte = edid->mode.vso - edid->mode.vborder;

	write32(&dsi0->dsi_vsa_nl, edid->mode.vspw);
	write32(&dsi0->dsi_vbp_nl, vbp_byte);
	write32(&dsi0->dsi_vfp_nl, vfp_byte);
	write32(&dsi0->dsi_vact_nl, edid->mode.va);

	unsigned int hspw = 0;
	if (mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
		hspw = edid->mode.hspw;

	hbp_byte = (edid->mode.hbl - edid->mode.hso - hspw - edid->mode.hborder)
			* bytes_per_pixel - 10;
	hsync_active_byte = edid->mode.hspw * bytes_per_pixel - 10;
	hfp_byte = (edid->mode.hso - edid->mode.hborder) * bytes_per_pixel;

	data_phy_cycles = phy_timing->lpx + phy_timing->da_hs_prepare +
		phy_timing->da_hs_zero + phy_timing->da_hs_exit + 2;

	u32 delta = 12;
	if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
		delta += 6;

	u32 d_phy = phy_timing->d_phy;
	if (d_phy == 0)
		d_phy = data_phy_cycles * lanes + delta;
	if (hfp_byte > d_phy)
		hfp_byte -= d_phy;
	else
		printk(BIOS_ERR, "HFP is not greater than d-phy, FPS < 60Hz "
		       "and the panel may not work properly.\n");

	write32(&dsi0->dsi_hsa_wc, hsync_active_byte);
	write32(&dsi0->dsi_hbp_wc, hbp_byte);
	write32(&dsi0->dsi_hfp_wc, hfp_byte);

	switch (format) {
	case MIPI_DSI_FMT_RGB888:
		packet_fmt = PACKED_PS_24BIT_RGB888;
		break;
	case MIPI_DSI_FMT_RGB666:
		packet_fmt = LOOSELY_PS_18BIT_RGB666;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		packet_fmt = PACKED_PS_18BIT_RGB666;
		break;
	case MIPI_DSI_FMT_RGB565:
		packet_fmt = PACKED_PS_16BIT_RGB565;
		break;
	default:
		packet_fmt = PACKED_PS_24BIT_RGB888;
		break;
	}

	hactive = edid->mode.ha;
	packet_fmt |= (hactive * bytes_per_pixel) & DSI_PS_WC;

	write32(&dsi0->dsi_psctrl,
		PIXEL_STREAM_CUSTOM_HEADER << DSI_PSCON_CUSTOM_HEADER_SHIFT |
		packet_fmt);

	/* Older systems like MT8173 do not support size_con. */
	if (MTK_DSI_HAVE_SIZE_CON)
		write32(&dsi0->dsi_size_con,
			edid->mode.va << DSI_SIZE_CON_HEIGHT_SHIFT |
			hactive << DSI_SIZE_CON_WIDTH_SHIFT);
}

static void mtk_dsi_start(void)
{
	write32(&dsi0->dsi_start, 0);
	/* Only start master DSI */
	write32(&dsi0->dsi_start, 1);
}

static bool mtk_dsi_is_read_command(u32 type)
{
	switch (type) {
	case MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM:
	case MIPI_DSI_DCS_READ:
		return true;
	}
	return false;
}

static void mtk_dsi_cmdq(const u8 *data, u8 len, u32 type)
{
	const u8 *tx_buf = data;
	u32 config;
	int i, j;

	if (!wait_ms(20, !(read32(&dsi0->dsi_intsta) & DSI_BUSY))) {
		printk(BIOS_ERR, "%s: cannot get DSI ready for sending commands"
		       " after 20ms and the panel may not work properly.\n",
		       __func__);
		return;
	}
	write32(&dsi0->dsi_intsta, 0);

	if (mtk_dsi_is_read_command(type))
		config = BTA;
	else
		config = (len > 2) ? LONG_PACKET : SHORT_PACKET;

	if (len <= 2) {
		uint32_t val = (type << 8) | config;
		for (i = 0; i < len; i++)
			val |= tx_buf[i] << (i + 2) * 8;
		write32(&dsi0->dsi_cmdq[0], val);
		write32(&dsi0->dsi_cmdq_size, 1);
	} else {
		/* TODO(hungte) Replace by buffer_to_fifo32_prefix */
		write32(&dsi0->dsi_cmdq[0], (len << 16) | (type << 8) | config);
		for (i = 0; i < len; i += 4) {
			uint32_t val = 0;
			for (j = 0; j < MIN(len - i, 4); j++)
				val |= tx_buf[i + j] << j * 8;
			write32(&dsi0->dsi_cmdq[i / 4 + 1], val);
		}
		write32(&dsi0->dsi_cmdq_size, 1 + DIV_ROUND_UP(len, 4));
	}

	mtk_dsi_start();

	if (!wait_us(400, read32(&dsi0->dsi_intsta) & CMD_DONE_INT_FLAG)) {
		printk(BIOS_ERR, "%s: failed sending DSI command, "
		       "panel may not work.\n", __func__);
		return;
	}
}

static void mtk_dsi_send_init_commands(const u8 *buf)
{
	if (!buf)
		return;
	const struct lcm_init_command *init = (const void *)buf;

	/*
	 * The given commands should be in a buffer containing a packed array of
	 * lcm_init_command and each element may be in variable size so we have
	 * to parse and scan.
	 */

	for (; init->cmd != LCM_END_CMD; init = (const void *)buf) {
		/*
		 * For some commands like DELAY, the init->len should not be
		 * counted for buf.
		 */
		buf += sizeof(*init);

		u32 cmd = init->cmd, len = init->len;
		u32 type;

		switch (cmd) {
		case LCM_DELAY_CMD:
			mdelay(len);
			continue;

		case LCM_DCS_CMD:
			switch (len) {
			case 0:
				return;
			case 1:
				type = MIPI_DSI_DCS_SHORT_WRITE;
				break;
			case 2:
				type = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
				break;
			default:
				type = MIPI_DSI_DCS_LONG_WRITE;
				break;
			}
			break;

		case LCM_GENERIC_CMD:
			switch (len) {
			case 0:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM;
				break;
			case 1:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM;
				break;
			case 2:
				type = MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM;
				break;
			default:
				type = MIPI_DSI_GENERIC_LONG_WRITE;
				break;
			}
			break;

		default:
			printk(BIOS_ERR, "%s: Unknown cmd: %d, "
			       "abort panel initialization.\n", __func__, cmd);
			return;

		}
		buf += len;
		mtk_dsi_cmdq(init->data, len, type);
	}
}

static void mtk_dsi_reset_dphy(void)
{
	setbits32(&dsi0->dsi_con_ctrl, DPHY_RESET);
	clrbits32(&dsi0->dsi_con_ctrl, DPHY_RESET);
}

int mtk_dsi_init(u32 mode_flags, u32 format, u32 lanes, const struct edid *edid,
		 const u8 *init_commands)
{
	int data_rate;
	u32 bits_per_pixel = mtk_dsi_get_bits_per_pixel(format);

	data_rate = mtk_dsi_get_data_rate(bits_per_pixel, lanes, edid);
	if (data_rate < 0)
		return -1;

	mtk_dsi_configure_mipi_tx(data_rate, lanes);
	mtk_dsi_reset();
	struct mtk_phy_timing phy_timing;
	mtk_dsi_phy_timing(data_rate, &phy_timing);
	mtk_dsi_rxtx_control(mode_flags, lanes);
	mdelay(1);
	mtk_dsi_reset_dphy();
	mtk_dsi_clk_hs_mode_disable();
	mtk_dsi_config_vdo_timing(mode_flags, format, lanes, edid, &phy_timing);
	mtk_dsi_clk_hs_mode_enable();
	mtk_dsi_send_init_commands(init_commands);
	mtk_dsi_set_mode(mode_flags);
	mtk_dsi_start();

	return 0;
}
