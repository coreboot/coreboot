/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/dp_intf.h>
#include <soc/mcucfg.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/spm.h>

static void mtk_dpintf_mask(struct mtk_dpintf *dpintf, u32 offset, u32 val, u32 mask)
{
	clrsetbits32(dpintf->regs + offset, mask, val);
}

static void mtk_dpintf_sw_reset(struct mtk_dpintf *dpintf, bool reset)
{
	mtk_dpintf_mask(dpintf, DPINTF_RET, reset ? RST : 0, RST);
}

static void mtk_dpintf_enable(struct mtk_dpintf *dpintf)
{
	mtk_dpintf_mask(dpintf, DPINTF_EN, EN, EN);
}

static void mtk_dpintf_config_hsync(struct mtk_dpintf *dpintf,
				    struct mtk_dpintf_sync_param *sync)
{
	mtk_dpintf_mask(dpintf, DPINTF_TGEN_HWIDTH,
			sync->sync_width << HPW, HPW_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_TGEN_HPORCH,
			sync->back_porch << HBP, HBP_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_TGEN_HPORCH,
			sync->front_porch << HFP, HFP_MASK);
}

static void mtk_dpintf_config_vsync(struct mtk_dpintf *dpintf,
				    struct mtk_dpintf_sync_param *sync,
				    u32 width_addr, u32 porch_addr)
{
	mtk_dpintf_mask(dpintf, width_addr,
			sync->sync_width << VSYNC_WIDTH_SHIFT,
			VSYNC_WIDTH_MASK);
	mtk_dpintf_mask(dpintf, width_addr,
			sync->shift_half_line << VSYNC_HALF_LINE_SHIFT,
			VSYNC_HALF_LINE_MASK);
	mtk_dpintf_mask(dpintf, porch_addr,
			sync->back_porch << VSYNC_BACK_PORCH_SHIFT,
			VSYNC_BACK_PORCH_MASK);
	mtk_dpintf_mask(dpintf, porch_addr,
			sync->front_porch << VSYNC_FRONT_PORCH_SHIFT,
			VSYNC_FRONT_PORCH_MASK);
}

static void mtk_dpintf_config_vsync_lodd(struct mtk_dpintf *dpintf,
					 struct mtk_dpintf_sync_param *sync)
{
	mtk_dpintf_config_vsync(dpintf, sync, DPINTF_TGEN_VWIDTH,
				DPINTF_TGEN_VPORCH);
}

static void mtk_dpintf_config_vsync_leven(struct mtk_dpintf *dpintf,
					  struct mtk_dpintf_sync_param *sync)
{
	mtk_dpintf_config_vsync(dpintf, sync, DPINTF_TGEN_VWIDTH_LEVEN,
				DPINTF_TGEN_VPORCH_LEVEN);
}

static void mtk_dpintf_config_vsync_rodd(struct mtk_dpintf *dpintf,
					 struct mtk_dpintf_sync_param *sync)
{
	mtk_dpintf_config_vsync(dpintf, sync, DPINTF_TGEN_VWIDTH_RODD,
				DPINTF_TGEN_VPORCH_RODD);
}

static void mtk_dpintf_config_vsync_reven(struct mtk_dpintf *dpintf,
					  struct mtk_dpintf_sync_param *sync)
{
	mtk_dpintf_config_vsync(dpintf, sync, DPINTF_TGEN_VWIDTH_REVEN,
				DPINTF_TGEN_VPORCH_REVEN);
}

static void mtk_dpintf_config_pol(struct mtk_dpintf *dpintf,
				  struct mtk_dpintf_polarities *dpintf_pol)
{
	u32 pol;

	pol = (dpintf_pol->hsync_pol == MTK_DPINTF_POLARITY_RISING ? 0 : HSYNC_POL) |
	      (dpintf_pol->vsync_pol == MTK_DPINTF_POLARITY_RISING ? 0 : VSYNC_POL);
	mtk_dpintf_mask(dpintf, DPINTF_OUTPUT_SETTING, pol, HSYNC_POL | VSYNC_POL);
}

static void mtk_dpintf_config_3d(struct mtk_dpintf *dpintf, bool en_3d)
{
	mtk_dpintf_mask(dpintf, DPINTF_CON, en_3d ? TDFP_EN : 0, TDFP_EN);
}

static void mtk_dpintf_config_interface(struct mtk_dpintf *dpintf, bool inter)
{
	mtk_dpintf_mask(dpintf, DPINTF_CON, inter ? INTL_EN : 0, INTL_EN);
}

static void mtk_dpintf_config_fb_size(struct mtk_dpintf *dpintf,
				      u32 width, u32 height)
{
	mtk_dpintf_mask(dpintf, DPINTF_SIZE, width << HSIZE, HSIZE_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_SIZE, height << VSIZE, VSIZE_MASK);
}

static void mtk_dpintf_config_channel_limit(struct mtk_dpintf *dpintf,
					    struct mtk_dpintf_yc_limit *limit)
{
	mtk_dpintf_mask(dpintf, DPINTF_Y_LIMIT,
			limit->y_bottom << Y_LIMINT_BOT, Y_LIMINT_BOT_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_Y_LIMIT,
			limit->y_top << Y_LIMINT_TOP, Y_LIMINT_TOP_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_C_LIMIT,
			limit->c_bottom << C_LIMIT_BOT, C_LIMIT_BOT_MASK);
	mtk_dpintf_mask(dpintf, DPINTF_C_LIMIT,
			limit->c_top << C_LIMIT_TOP, C_LIMIT_TOP_MASK);
}

static void mtk_dpintf_config_bit_num(struct mtk_dpintf *dpintf,
				      enum mtk_dpintf_out_bit_num num)
{
	u32 val;

	switch (num) {
	case MTK_DPINTF_OUT_BIT_NUM_8BITS:
		val = OUT_BIT_8;
		break;
	case MTK_DPINTF_OUT_BIT_NUM_10BITS:
		val = OUT_BIT_10;
		break;
	case MTK_DPINTF_OUT_BIT_NUM_12BITS:
		val = OUT_BIT_12;
		break;
	case MTK_DPINTF_OUT_BIT_NUM_16BITS:
		val = OUT_BIT_16;
		break;
	default:
		val = OUT_BIT_8;
		break;
	}
	mtk_dpintf_mask(dpintf, DPINTF_OUTPUT_SETTING, val, OUT_BIT_MASK);
}

static void mtk_dpintf_config_channel_swap(struct mtk_dpintf *dpintf,
					   enum mtk_dpintf_out_channel_swap swap)
{
	u32 val;

	switch (swap) {
	case MTK_DPINTF_OUT_CHANNEL_SWAP_RGB:
		val = SWAP_RGB;
		break;
	case MTK_DPINTF_OUT_CHANNEL_SWAP_GBR:
		val = SWAP_GBR;
		break;
	case MTK_DPINTF_OUT_CHANNEL_SWAP_BRG:
		val = SWAP_BRG;
		break;
	case MTK_DPINTF_OUT_CHANNEL_SWAP_RBG:
		val = SWAP_RBG;
		break;
	case MTK_DPINTF_OUT_CHANNEL_SWAP_GRB:
		val = SWAP_GRB;
		break;
	case MTK_DPINTF_OUT_CHANNEL_SWAP_BGR:
		val = SWAP_BGR;
		break;
	default:
		val = SWAP_RGB;
		break;
	}

	mtk_dpintf_mask(dpintf, DPINTF_OUTPUT_SETTING, val, CH_SWAP_MASK);
}

static void mtk_dpintf_config_yuv422_enable(struct mtk_dpintf *dpintf, bool enable)
{
	mtk_dpintf_mask(dpintf, DPINTF_CON, enable ? YUV422_EN : 0, YUV422_EN);
}

static void mtk_dpintf_config_color_format(struct mtk_dpintf *dpintf,
					   enum mtk_dpintf_out_color_format format)
{
	bool enable;
	int channel_swap;

	if (format == MTK_DPINTF_COLOR_FORMAT_YCBCR_444 ||
	    format == MTK_DPINTF_COLOR_FORMAT_YCBCR_444_FULL) {
		enable = false;
		channel_swap = MTK_DPINTF_OUT_CHANNEL_SWAP_BGR;
	} else if (format == MTK_DPINTF_COLOR_FORMAT_YCBCR_422 ||
		   format == MTK_DPINTF_COLOR_FORMAT_YCBCR_422_FULL) {
		enable = true;
		channel_swap = MTK_DPINTF_OUT_CHANNEL_SWAP_RGB;
	} else {
		enable = false;
		channel_swap = MTK_DPINTF_OUT_CHANNEL_SWAP_RGB;
	}

	mtk_dpintf_config_yuv422_enable(dpintf, enable);
	mtk_dpintf_config_channel_swap(dpintf, channel_swap);
}

static int mtk_dpintf_power_on(struct mtk_dpintf *dpintf, const struct edid *edid)
{
	u32 clksrc;
	u32 pll_rate;

	if (edid->mode.pixel_clock < 70000)
		clksrc = TVDPLL_D16;
	else if (edid->mode.pixel_clock < 200000)
		clksrc = TVDPLL_D8;
	else
		clksrc = TVDPLL_D4;

	pll_rate = edid->mode.pixel_clock * 1000 * (1 << ((clksrc + 1) / 2));

	mt_pll_set_tvd_pll1_freq(pll_rate / 4);
	mt_pll_edp_mux_set_sel(clksrc);

	mtk_dpintf_enable(dpintf);

	return 0;
}

static int mtk_dpintf_set_display_mode(struct mtk_dpintf *dpintf,
				       const struct edid *edid)
{
	struct mtk_dpintf_yc_limit limit;
	struct mtk_dpintf_polarities dpintf_pol;
	struct mtk_dpintf_sync_param hsync;
	struct mtk_dpintf_sync_param vsync_lodd = { 0 };
	struct mtk_dpintf_sync_param vsync_leven = { 0 };
	struct mtk_dpintf_sync_param vsync_rodd = { 0 };
	struct mtk_dpintf_sync_param vsync_reven = { 0 };

	vsync_lodd.back_porch = edid->mode.vbl - edid->mode.vso -
				edid->mode.vspw - edid->mode.vborder;
	vsync_lodd.front_porch = edid->mode.vso - edid->mode.vborder;
	vsync_lodd.sync_width = edid->mode.vspw;
	vsync_lodd.shift_half_line = false;

	hsync.sync_width = edid->mode.hspw / 4;
	hsync.back_porch = (edid->mode.hbl - edid->mode.hso -
			    edid->mode.hspw - edid->mode.hborder) / 4;
	hsync.front_porch = (edid->mode.hso - edid->mode.hborder) / 4;
	hsync.shift_half_line = false;

	/* Let pll_rate be able to fix the valid range of tvdpll (1G~2GHz) */
	limit.c_bottom = 0x0000;
	limit.c_top = 0xfff;
	limit.y_bottom = 0x0000;
	limit.y_top = 0xfff;

	dpintf_pol.ck_pol = MTK_DPINTF_POLARITY_FALLING;
	dpintf_pol.de_pol = MTK_DPINTF_POLARITY_RISING;
	dpintf_pol.hsync_pol = (edid->mode.phsync == '+') ?
			       MTK_DPINTF_POLARITY_FALLING :
			       MTK_DPINTF_POLARITY_RISING;
	dpintf_pol.vsync_pol = (edid->mode.pvsync == '+') ?
			       MTK_DPINTF_POLARITY_FALLING :
			       MTK_DPINTF_POLARITY_RISING;

	mtk_dpintf_sw_reset(dpintf, true);
	mtk_dpintf_config_pol(dpintf, &dpintf_pol);

	mtk_dpintf_config_hsync(dpintf, &hsync);
	mtk_dpintf_config_vsync_lodd(dpintf, &vsync_lodd);
	mtk_dpintf_config_vsync_rodd(dpintf, &vsync_rodd);
	mtk_dpintf_config_vsync_leven(dpintf, &vsync_leven);
	mtk_dpintf_config_vsync_reven(dpintf, &vsync_reven);

	mtk_dpintf_config_3d(dpintf, false);
	mtk_dpintf_config_interface(dpintf, false);
	mtk_dpintf_config_fb_size(dpintf, edid->mode.ha, edid->mode.va);

	mtk_dpintf_config_channel_limit(dpintf, &limit);
	mtk_dpintf_config_bit_num(dpintf, dpintf->bit_num);
	mtk_dpintf_config_channel_swap(dpintf, dpintf->channel_swap);
	mtk_dpintf_config_color_format(dpintf, dpintf->color_format);

	mtk_dpintf_mask(dpintf, DPINTF_CON, INPUT_2P_EN, INPUT_2P_EN);
	mtk_dpintf_sw_reset(dpintf, false);

	return 0;
}

void dp_intf_config(const struct edid *edid)
{
	struct mtk_dpintf dpintf = {
		.regs = (void *)(DP_INTF0_BASE),
		.color_format = MTK_DPINTF_COLOR_FORMAT_RGB,
		.yc_map = MTK_DPINTF_OUT_YC_MAP_RGB,
		.bit_num = MTK_DPINTF_OUT_BIT_NUM_8BITS,
		.channel_swap = MTK_DPINTF_OUT_CHANNEL_SWAP_RGB,
	};

	mtk_dpintf_power_on(&dpintf, edid);
	mtk_dpintf_set_display_mode(&dpintf, edid);
}
