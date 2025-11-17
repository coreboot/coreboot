/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <mipi/panel.h>
#include <device/mmio.h>
#include <delay.h>
#include <edid.h>
#include <soc/display_dsi.h>
#include <soc/dsi.h>
#include <string.h>
#include <timer.h>

#define MIN_HFP_BYTE	2
#define MIN_HBP_BYTE	2

#define CPHY_SYMBOL_RATE		7
#define CPHY_SYMBOL_RATE_DIVISOR	16

#define COMPRESSION_RATIO	3
#define UNCOMPRESSED_RATIO	1

static const struct {
	struct dsi_regs *const dsi_reg;
	struct mipi_tx_regs *const mipi_reg;
} dsi_mipi_regs[] = {
	{
		.dsi_reg = dsi0,
		.mipi_reg = mipi_tx0,
	},
#if CONFIG(MEDIATEK_DSI_DUAL_CHANNEL)
	{
		.dsi_reg = dsi1,
		.mipi_reg = mipi_tx1,
	},
#endif
};

static unsigned int mtk_dsi_get_bits_per_pixel(u32 format)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB565:
		return 16;
	case MIPI_DSI_FMT_RGB666_PACKED:
		return 18;
	case MIPI_DSI_FMT_RGB666:
	case MIPI_DSI_FMT_RGB888:
		return 24;
	}
	printk(BIOS_WARNING, "%s: WARN: Unknown format %d, assuming 24 bpp\n",
	       __func__, format);
	return 24;
}

static u32 mtk_dsi_get_data_rate(u32 bits_per_pixel, u32 lanes,
				 const struct edid *edid, u32 mode_flags)
{
	/* data_rate = pixel_clock * bits_per_pixel * mipi_ratio / lanes
	 * Note pixel_clock comes in kHz and returned data_rate is in bps.
	 * mipi_ratio is the clk coefficient to balance the pixel clk in MIPI
	 * for older platforms which do not have complete implementation in HFP.
	 * Newer platforms should just set that to 1.0 (100 / 100).
	 */
	u64 pixel_clock = edid->mode.pixel_clock;
	u32 data_rate;
	bool is_cphy = !!(mode_flags & MIPI_DSI_MODE_CPHY);
	bool is_dsi_dual_channel = !!(mode_flags & MIPI_DSI_DUAL_CHANNEL);
	bool is_dsc_enabled = !!(mode_flags & MIPI_DSI_DSC_MODE);

	if (is_dsc_enabled)
		pixel_clock = pixel_clock *
			      (DIV_ROUND_UP(edid->mode.ha, 3) + edid->mode.hbl) /
			      (edid->mode.ha + edid->mode.hbl);

	u64 dividend = pixel_clock * bits_per_pixel * 1000 * MTK_DSI_MIPI_RATIO_NUMERATOR;
	u64 divisor = (u64)lanes * MTK_DSI_MIPI_RATIO_DENOMINATOR;

	if (is_cphy) {
		dividend *= CPHY_SYMBOL_RATE;
		divisor *= CPHY_SYMBOL_RATE_DIVISOR;
	}
	data_rate = DIV_ROUND_UP(dividend, divisor);
	printk(BIOS_INFO, "pixel_clock: %lld\n", pixel_clock);
	printk(BIOS_INFO, "bits_per_pixel: %d\n", bits_per_pixel);
	if (is_dsi_dual_channel)
		data_rate = data_rate / 2;

	printk(BIOS_INFO, "DSI final data_rate: %u bps\n", data_rate);

	if (data_rate < MTK_DSI_DATA_RATE_MIN_MHZ * MHz) {
		printk(BIOS_ERR, "data rate (%ubps) must be >= %ubps. "
		       "Please check the pixel clock (%llu), "
		       "bits per pixel (%u), "
		       "mipi_ratio (%d%%) and number of lanes (%d)\n",
		       data_rate, MTK_DSI_DATA_RATE_MIN_MHZ * MHz,
		       pixel_clock, bits_per_pixel,
		       (100 * MTK_DSI_MIPI_RATIO_NUMERATOR /
			MTK_DSI_MIPI_RATIO_DENOMINATOR), lanes);
		return 0;
	}
	return data_rate;
}

__weak void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing)
{
	/* Do nothing. */
}

static void mtk_dsi_dphy_timing(struct dsi_regs *dsi_reg, u32 data_rate,
				struct mtk_phy_timing *timing)
{
	u32 timcon0, timcon1, timcon2, timcon3;
	u32 data_rate_mhz = DIV_ROUND_UP(data_rate, MHz);

	mtk_dsi_dphy_timing_calculation(data_rate_mhz, timing);

	/* Allow board-specific tuning. */
	mtk_dsi_override_phy_timing(timing);

	timcon0 = timing->lpx | timing->da_hs_prepare << 8 |
		  timing->da_hs_zero << 16 | timing->da_hs_trail << 24;
	timcon1 = timing->ta_go | timing->ta_sure << 8 |
		  timing->ta_get << 16 | timing->da_hs_exit << 24;
	timcon2 = timing->da_hs_sync << 8 | timing->clk_hs_zero << 16 |
		  timing->clk_hs_trail << 24;
	timcon3 = timing->clk_hs_prepare | timing->clk_hs_post << 8 |
		  timing->clk_hs_exit << 16;

	write32(&dsi_reg->dsi_phy_timecon0, timcon0);
	write32(&dsi_reg->dsi_phy_timecon1, timcon1);
	write32(&dsi_reg->dsi_phy_timecon2, timcon2);
	write32(&dsi_reg->dsi_phy_timecon3, timcon3);
}

static void mtk_dsi_clk_hs_mode_enable(struct dsi_regs *dsi_reg)
{
	setbits32(&dsi_reg->dsi_phy_lccon, LC_HS_TX_EN);
}

static void mtk_dsi_clk_hs_mode_disable(struct dsi_regs *dsi_reg)
{
	clrbits32(&dsi_reg->dsi_phy_lccon, LC_HS_TX_EN);
}

static void mtk_dsi_set_mode(struct dsi_regs *dsi_reg, u32 mode_flags)
{
	u32 tmp_reg1 = 0;

	if (mode_flags & MIPI_DSI_MODE_VIDEO) {
		tmp_reg1 = SYNC_PULSE_MODE;

		if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
			tmp_reg1 = BURST_MODE;

		if (mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
			tmp_reg1 = SYNC_PULSE_MODE;
	}

	write32(&dsi_reg->dsi_mode_ctrl, tmp_reg1);
}

static void mtk_dsi_rxtx_control(struct dsi_regs *dsi_reg, u32 mode_flags, u32 lanes)
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

	if (mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS)
		tmp_reg |= NON_CONTINUOUS_CLK;

	if (!(mode_flags & MIPI_DSI_MODE_EOT_PACKET))
		tmp_reg |= EOTP_DISABLE;

	write32(&dsi_reg->dsi_txrx_ctrl, tmp_reg);
}

static void mtk_dsi_dphy_vdo_timing(const u32 mode_flags,
				    const u32 lanes,
				    const struct edid *edid,
				    const struct mtk_phy_timing *phy_timing,
				    const u32 bytes_per_pixel,
				    const u32 hbp,
				    const u32 hfp,
				    s32 *hbp_byte,
				    s32 *hfp_byte,
				    u32 *hsync_active_byte)
{
	u32 data_phy_cycles = phy_timing->lpx + phy_timing->da_hs_prepare +
				phy_timing->da_hs_zero + phy_timing->da_hs_exit + 3;

	u32 delta = 10;
	int channels = (mode_flags & MIPI_DSI_DUAL_CHANNEL) ? 2 : 1;
	int dsc_ratio = (mode_flags & MIPI_DSI_DSC_MODE) ? COMPRESSION_RATIO :
							   UNCOMPRESSED_RATIO;

	if (mode_flags & MIPI_DSI_MODE_EOT_PACKET)
		delta += 2;

	if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
		delta += 6;

	u32 d_phy = phy_timing->d_phy;
	if (d_phy == 0)
		d_phy = data_phy_cycles * lanes + delta;

	if ((hfp + hbp) * bytes_per_pixel > d_phy) {
		*hfp_byte -= d_phy * hfp / (hfp + hbp);
		*hbp_byte -= d_phy * hbp / (hfp + hbp);
	} else {
		printk(BIOS_ERR, "HFP %u plus HBP %u is not greater than d_phy %u, "
			"the panel may not work properly.\n", hfp * bytes_per_pixel,
			hbp * bytes_per_pixel, d_phy);
	}

	*hsync_active_byte = edid->mode.hspw / channels * bytes_per_pixel - 10;

	if (mode_flags & MIPI_DSI_MODE_LINE_END) {
		*hsync_active_byte = DIV_ROUND_UP(*hsync_active_byte, lanes) * lanes - 2;
		*hbp_byte = DIV_ROUND_UP(*hbp_byte, lanes) * lanes - 2;
		*hfp_byte = DIV_ROUND_UP(*hfp_byte, lanes) * lanes - 2;
		*hbp_byte -= (edid->mode.ha / dsc_ratio / channels * bytes_per_pixel + 2) %
			     lanes;
	}
}

static u32 mtk_dsi_get_packet_fmt(u32 format)
{
	u32 packet_fmt;

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

	return packet_fmt;
}

static void mtk_dsi_config_vdo_timing(struct dsi_regs *const dsi_reg, u32 mode_flags,
				      u32 format, u32 lanes, const struct edid *edid,
				      const struct mtk_phy_timing *phy_timing)
{
	u32 hsync_active_byte;
	u32 hbp;
	u32 hfp;
	s32 hbp_byte;
	s32 hfp_byte;
	u32 vbp_byte;
	u32 vfp_byte;
	u32 bytes_per_pixel;
	u32 packet_fmt;
	u32 hactive;
	u32 hbp_offset;
	bool is_cphy = !!(mode_flags & MIPI_DSI_MODE_CPHY);
	bool is_dsc_enabled = !!(mode_flags & MIPI_DSI_DSC_MODE);
	int channels = !!(mode_flags & MIPI_DSI_DUAL_CHANNEL) ? 2 : 1;
	int dsc_ratio = is_dsc_enabled ? COMPRESSION_RATIO : UNCOMPRESSED_RATIO;

	bytes_per_pixel = DIV_ROUND_UP(mtk_dsi_get_bits_per_pixel(format), 8);
	vbp_byte = edid->mode.vbl - edid->mode.vso - edid->mode.vspw -
		   edid->mode.vborder;
	vfp_byte = edid->mode.vso - edid->mode.vborder;

	write32(&dsi_reg->dsi_vsa_nl, edid->mode.vspw);
	write32(&dsi_reg->dsi_vbp_nl, vbp_byte);
	write32(&dsi_reg->dsi_vfp_nl, vfp_byte);
	write32(&dsi_reg->dsi_vact_nl, edid->mode.va);

	hbp = (edid->mode.hbl - edid->mode.hso - edid->mode.hspw -
	       edid->mode.hborder) / channels;
	hfp = (edid->mode.hso - edid->mode.hborder) / channels;

	hbp_offset = (mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE) ?
		     0 : (edid->mode.hspw / channels);
	hbp_byte = (hbp + hbp_offset) * bytes_per_pixel - 10;

	hfp_byte = hfp * bytes_per_pixel;

	if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
		mtk_dsi_cphy_vdo_timing(lanes, edid, phy_timing, bytes_per_pixel, hbp, hfp,
					&hbp_byte, &hfp_byte, &hsync_active_byte);
	else
		mtk_dsi_dphy_vdo_timing(mode_flags, lanes, edid, phy_timing, bytes_per_pixel,
					hbp, hfp, &hbp_byte, &hfp_byte, &hsync_active_byte);

	if (hfp_byte + hbp_byte < MIN_HFP_BYTE + MIN_HBP_BYTE) {
		printk(BIOS_ERR, "Calculated hfp_byte %d and hbp_byte %d are too small, "
		       "the panel may not work properly.\n", hfp_byte, hbp_byte);
	} else if (hfp_byte < MIN_HFP_BYTE) {
		printk(BIOS_NOTICE, "Calculated hfp_byte %d is too small, "
		       "adjust it to the minimum value %d.\n", hfp_byte, MIN_HFP_BYTE);
		hbp_byte -= MIN_HFP_BYTE - hfp_byte;
		hfp_byte = MIN_HFP_BYTE;
	} else if (hbp_byte < MIN_HBP_BYTE) {
		printk(BIOS_NOTICE, "Calculated hbp_byte %d is too small, "
		       "adjust it to the minimum value %d.\n", hbp_byte, MIN_HBP_BYTE);
		hfp_byte -= MIN_HBP_BYTE - hbp_byte;
		hbp_byte = MIN_HBP_BYTE;
	}

	write32(&dsi_reg->dsi_hsa_wc, hsync_active_byte);
	write32(&dsi_reg->dsi_hbp_wc, hbp_byte);
	write32(&dsi_reg->dsi_hfp_wc, hfp_byte);

	if (is_dsc_enabled)
		packet_fmt = COMPRESSED_PIXEL_STREAM_V2;
	else
		packet_fmt = mtk_dsi_get_packet_fmt(format);

	hactive = edid->mode.ha / dsc_ratio / channels;
	packet_fmt |= (hactive * bytes_per_pixel) & DSI_PS_WC;

	write32(&dsi_reg->dsi_psctrl,
		PIXEL_STREAM_CUSTOM_HEADER << DSI_PSCON_CUSTOM_HEADER_SHIFT |
		packet_fmt);

	/* Older systems like MT8173 do not support size_con. */
	if (MTK_DSI_HAVE_SIZE_CON)
		write32(&dsi_reg->dsi_size_con,
			edid->mode.va << DSI_SIZE_CON_HEIGHT_SHIFT |
			hactive << DSI_SIZE_CON_WIDTH_SHIFT);
	if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
		mtk_dsi_cphy_enable_cmdq_6byte(dsi_reg);
}

static void mtk_dsi_dual_enable(struct dsi_regs *dsi_reg, bool enable)
{
	clrsetbits32(&dsi_reg->dsi_con_ctrl, DSI_DUAL, (enable ? DSI_DUAL : 0));
}

static void mtk_dsi_start(struct dsi_regs *dsi_reg)
{
	write32(&dsi_reg->dsi_start, 0);
	write32(&dsi_reg->dsi_start, 1);
}

static bool mtk_dsi_is_read_command(enum mipi_dsi_transaction type)
{
	switch (type) {
	case MIPI_DSI_GENERIC_READ_REQUEST_0_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM:
	case MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM:
	case MIPI_DSI_DCS_READ:
		return true;
	default:
		return false;
	}
}

static void mtk_dsi_enable_and_start(bool is_dsi_dual_channel)
{
	/*
	 * For dual channel synchronization,
	 * the secondary dsi1 must be dual-enabled before starting the primary dsi0.
	 */
	if (is_dsi_dual_channel)
		mtk_dsi_dual_enable(dsi_mipi_regs[ARRAY_SIZE(dsi_mipi_regs) - 1].dsi_reg, true);
	/*
	 * Only start primary dsi0 for single or dual channel mode.
	 * The secondary dsi1 is dual-enabled in mtk_dsi_dual_enable()
	 * and does not require a separate start.
	 * Starting only the primary dsi0 ensures correct synchronization
	 * with secondary dsi1 if there is.
	 */
	mtk_dsi_start(dsi_mipi_regs[0].dsi_reg);
}

static enum cb_err mtk_dsi_cmdq(enum mipi_dsi_transaction type, const u8 *data, u8 len,
				void *user_data)
{
	const u8 *tx_buf = data;
	u32 config;
	uint32_t *mode_flags = (uint32_t *)user_data;
	bool is_dsi_dual_channel = (*mode_flags & MIPI_DSI_DUAL_CHANNEL);

	for (unsigned int k = 0; k < ARRAY_SIZE(dsi_mipi_regs); k++) {
		struct dsi_regs *dsi = dsi_mipi_regs[k].dsi_reg;
		if (!wait_ms(20, !(read32(&dsi->dsi_intsta) & DSI_BUSY))) {
			printk(BIOS_ERR, "%s: cannot get DSI-%d ready for sending commands"
			       " after 20ms and the panel may not work properly.\n",
			       __func__, k);
			return CB_ERR;
		}
		write32(&dsi->dsi_intsta, 0);

		if (mtk_dsi_is_read_command(type))
			config = BTA;
		else
			config = (len > 2) ? LONG_PACKET : SHORT_PACKET;

		u32 prefix = config | type << 8;
		int prefsz = 2;
		if (len > 2) {
			prefix |= len << 16;
			prefsz += 2;
		}
		buffer_to_fifo32_prefix(tx_buf, prefix, prefsz, prefsz + len, &dsi->dsi_cmdq[0],
					4, 4);
		write32(&dsi->dsi_cmdq_size, DIV_ROUND_UP(prefsz + len, 4));
		setbits32(&dsi->dsi_cmdq_size, CMDQ_SIZE_SEL);
	}

	mtk_dsi_enable_and_start(is_dsi_dual_channel);

	for (unsigned int k = 0; k < ARRAY_SIZE(dsi_mipi_regs); k++) {
		struct dsi_regs *dsi = dsi_mipi_regs[k].dsi_reg;
		if (!wait_us(400, read32(&dsi->dsi_intsta) & CMD_DONE_INT_FLAG)) {
			printk(BIOS_ERR, "%s: failed sending DSI-%d command, "
			       "panel may not work.\n", __func__, k);
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}

static void mtk_dsi_reset_phy(struct dsi_regs *const dsi_reg)
{
	setbits32(&dsi_reg->dsi_con_ctrl, DPHY_RESET);
	clrbits32(&dsi_reg->dsi_con_ctrl, DPHY_RESET);
}

int mtk_dsi_init(u32 mode_flags, u32 format, u32 lanes, const struct edid *edid,
		 const u8 *init_commands)
{
	u32 data_rate;
	u32 bits_per_pixel = mtk_dsi_get_bits_per_pixel(format);
	bool is_cphy = !!(mode_flags & MIPI_DSI_MODE_CPHY);
	bool is_dsi_dual_channel = !!(mode_flags & MIPI_DSI_DUAL_CHANNEL);
	unsigned int num_dsi = is_dsi_dual_channel ? 2 : 1;

	if (!CONFIG(MEDIATEK_DSI_CPHY) && is_cphy) {
		printk(BIOS_ERR, "%s: Board is built without C-PHY interface support. "
		       "Please check Kconfig MEDIATEK_DSI_CPHY.\n", __func__);
		return -1;
	}

	if (num_dsi > ARRAY_SIZE(dsi_mipi_regs)) {
		printk(BIOS_ERR, "%s: num_dsi %d > %lu\n", __func__,
		       num_dsi, ARRAY_SIZE(dsi_mipi_regs));
		return -1;
	}

	data_rate = mtk_dsi_get_data_rate(bits_per_pixel, lanes, edid, mode_flags);
	if (!data_rate)
		return -1;

	for (unsigned int i = 0; i < num_dsi; i++) {
		struct dsi_regs *dsi = dsi_mipi_regs[i].dsi_reg;
		struct mipi_tx_regs *mipi = dsi_mipi_regs[i].mipi_reg;
		if (!dsi || !mipi) {
			printk(BIOS_ERR, "%s: Null dsi/mipi reg for DSI-%d\n", __func__, i);
			return -1;
		}
		mtk_dsi_configure_mipi_tx(mipi, data_rate, lanes, is_cphy);
		mtk_dsi_reset(dsi);
		struct mtk_phy_timing phy_timing = {};
		if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
			/* Dual channel is not implemented for CPHY. */
			mtk_dsi_cphy_timing(data_rate, &phy_timing);
		else
			mtk_dsi_dphy_timing(dsi, data_rate, &phy_timing);

		mtk_dsi_rxtx_control(dsi, mode_flags, lanes);
		mdelay(1);
		mtk_dsi_reset_phy(dsi);
		mtk_dsi_clk_hs_mode_disable(dsi);
		mtk_dsi_config_vdo_timing(dsi, mode_flags, format, lanes, edid, &phy_timing);
		mtk_dsi_clk_hs_mode_enable(dsi);
	}

	if (init_commands)
		mipi_panel_parse_init_commands(init_commands, mtk_dsi_cmdq, &mode_flags);

	for (unsigned int i = 0; i < num_dsi; i++)
		mtk_dsi_set_mode(dsi_mipi_regs[i].dsi_reg, mode_flags);

	mtk_dsi_enable_and_start(is_dsi_dual_channel);

	return 0;
}
