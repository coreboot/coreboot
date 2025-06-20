/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/dp_intf.h>
#include <soc/pll.h>
#include <soc/spm.h>
#include <types.h>

static const struct mtk_dvo_gs_info mtk_dvo_gs[MTK_DVO_GSL_MAX] = {
	[MTK_DVO_FHD_60FPS_1920] = {6880, 511},
	[MTK_DVO_8K_30FPS] = {5255, 3899},
};

static void mtk_dvo_mask(const struct mtk_dvo *dvo, u32 offset, u32 val, u32 mask)
{
	clrsetbits32(dvo->regs + offset, mask, val);
}

static void mtk_dvo_sw_reset(const struct mtk_dvo *dvo, bool reset)
{
	mtk_dvo_mask(dvo, DVO_RET, reset ? SWRST : 0, SWRST);
}

static void mtk_dvo_enable(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_EN, EN, EN);
}

static void mtk_dvo_config_hsync(const struct mtk_dvo *dvo,
				 const struct mtk_dvo_sync_param *sync)
{
	mtk_dvo_mask(dvo, DVO_TGEN_H0, sync->sync_width << HSYNC_SHIFT, HSYNC_MASK);
	mtk_dvo_mask(dvo, DVO_TGEN_H0, sync->front_porch << HFP_SHIFT, HFP_MASK);
	mtk_dvo_mask(dvo, DVO_TGEN_H1, (sync->back_porch + sync->sync_width) << HSYNC2ACT_SHIFT,
		     HSYNC2ACT_MASK);
}

static void mtk_dvo_config_vsync(const struct mtk_dvo *dvo,
				 const struct mtk_dvo_sync_param *sync,
				 u32 width_addr, u32 porch_addr)
{
	mtk_dvo_mask(dvo, width_addr, sync->sync_width << VSYNC_SHIFT, VSYNC_MASK);
	mtk_dvo_mask(dvo, width_addr, sync->front_porch << VFP_SHIFT, VFP_MASK);
	mtk_dvo_mask(dvo, porch_addr, (sync->back_porch + sync->sync_width) << VSYNC2ACT_SHIFT,
		     VSYNC2ACT_MASK);
}

static void mtk_dvo_config_vsync_lodd(const struct mtk_dvo *dvo,
				      struct mtk_dvo_sync_param *sync)
{
	mtk_dvo_config_vsync(dvo, sync, DVO_TGEN_V0, DVO_TGEN_V1);
}

static void mtk_dvo_config_interface(const struct mtk_dvo *dvo, bool inter)
{
	mtk_dvo_mask(dvo, DVO_CON, inter ? INTL_EN : 0, INTL_EN);
}

static void mtk_dvo_config_fb_size(const struct mtk_dvo *dvo, u32 width, u32 height)
{
	mtk_dvo_mask(dvo, DVO_SRC_SIZE, width << SRC_HSIZE_SHIFT, SRC_HSIZE_MASK);
	mtk_dvo_mask(dvo, DVO_SRC_SIZE, height << SRC_VSIZE_SHIFT, PIC_VSIZE_MASK);

	mtk_dvo_mask(dvo, DVO_PIC_SIZE, width << PIC_HSIZE_SHIFT, PIC_HSIZE_MASK);
	mtk_dvo_mask(dvo, DVO_PIC_SIZE, height << PIC_VSIZE_SHIFT, PIC_VSIZE_MASK);

	mtk_dvo_mask(dvo, DVO_TGEN_H1, DIV_ROUND_UP(width, 4) << HACT_SHIFT, HACT_MASK);
	mtk_dvo_mask(dvo, DVO_TGEN_V1, height << VACT_SHIFT, VACT_MASK);
}

static void mtk_dvo_irq_enable(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_INTEN, INT_VDE_END_EN, INT_VDE_END_EN);
}

static void mtk_dvo_info_queue_start(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_TGEN_INFOQ_LATENCY, 0,
		     INFOQ_START_LATENCY_MASK | INFOQ_END_LATENCY_MASK);
}

static void mtk_dvo_buffer_ctrl(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_BUF_CON0, DISP_BUF_EN, DISP_BUF_EN);
	mtk_dvo_mask(dvo, DVO_BUF_CON0, FIFO_UNDERFLOW_DONE_BLOCK, FIFO_UNDERFLOW_DONE_BLOCK);
}

static void mtk_dvo_trailing_blank_setting(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_TGEN_V_LAST_TRAILING_BLANK, 0x20 << V_LAST_TRAILING_BLANK,
		     V_LAST_TRAILING_BLANK_MASK);
	mtk_dvo_mask(dvo, DVO_TGEN_OUTPUT_DELAY_LINE, 0x20 << EXT_TG_DLY_LINE,
		     EXT_TG_DLY_LINE_MASK);
}

static void mtk_dvo_golden_setting(const struct mtk_dvo *dvo)
{
	assert(dvo->gs_level < MTK_DVO_GSL_MAX);

	printk(BIOS_INFO, "[eDPTX] gs_level %d sodi %d %d\n", dvo->gs_level,
	       mtk_dvo_gs[dvo->gs_level].dvo_buf_sodi_high,
	       mtk_dvo_gs[dvo->gs_level].dvo_buf_sodi_low);

	mtk_dvo_mask(dvo, DVO_BUF_SODI_HIGHT,
		     mtk_dvo_gs[dvo->gs_level].dvo_buf_sodi_high, 0xFFFFFFFF);
	mtk_dvo_mask(dvo, DVO_BUF_SODI_LOW,
		     mtk_dvo_gs[dvo->gs_level].dvo_buf_sodi_low, 0xFFFFFFFF);
}

static void mtk_dvo_shadow_ctrl(const struct mtk_dvo *dvo)
{
	mtk_dvo_mask(dvo, DVO_SHADOW_CTRL, 0, BYPASS_SHADOW);
	mtk_dvo_mask(dvo, DVO_SHADOW_CTRL, FORCE_COMMIT, FORCE_COMMIT);
}

static int mtk_dvo_power_on(const struct mtk_dvo *dvo, const struct edid *edid)
{
	u32 clksrc;
	u32 pll_rate;

	if (edid->mode.pixel_clock < 70000)
		clksrc = TVDPLL_D16;
	else if (edid->mode.pixel_clock < 200000)
		clksrc = TVDPLL_D8;
	else
		clksrc = TVDPLL_D4;

	pll_rate = edid->mode.pixel_clock * 1000 * (1 << (clksrc + 1));

	mt_pll_set_tvd_pll1_freq(pll_rate / 4);
	mt_pll_edp_mux_set_sel(clksrc);
	printk(BIOS_INFO, "[eDPTX] dvo pll=%u clksrc=%u\n", pll_rate, clksrc);
	mtk_dvo_enable(dvo);

	return 0;
}

static int mtk_dvo_set_display_mode(const struct mtk_dvo *dvo, const struct edid *edid)
{
	struct mtk_dvo_sync_param hsync = {0};
	struct mtk_dvo_sync_param vsync_lodd = {0};

	vsync_lodd.sync_width = edid->mode.vspw;
	vsync_lodd.front_porch = edid->mode.vso - edid->mode.vborder;
	vsync_lodd.back_porch =
		edid->mode.vbl - edid->mode.vso - edid->mode.vspw - edid->mode.vborder;

	hsync.sync_width = edid->mode.hspw / 4;
	hsync.front_porch = (edid->mode.hso - edid->mode.hborder) / 4;
	hsync.back_porch =
		(edid->mode.hbl - edid->mode.hso - edid->mode.hspw - edid->mode.hborder) / 4;

	mtk_dvo_sw_reset(dvo, true);
	mtk_dvo_irq_enable(dvo);

	mtk_dvo_config_hsync(dvo, &hsync);
	mtk_dvo_config_vsync_lodd(dvo, &vsync_lodd);

	mtk_dvo_config_interface(dvo, false);
	mtk_dvo_config_fb_size(dvo, edid->mode.ha, edid->mode.va);

	mtk_dvo_info_queue_start(dvo);
	mtk_dvo_buffer_ctrl(dvo);
	mtk_dvo_trailing_blank_setting(dvo);

	mtk_dvo_golden_setting(dvo);

	mtk_dvo_shadow_ctrl(dvo);

	mtk_dvo_mask(dvo, DVO_OUTPUT_SET, 0x2, OUT_NP_SEL);
	mtk_dvo_sw_reset(dvo, false);

	return 0;
}

static const struct mtk_dvo dvo_data = {
	.regs = (void *)DISP_DVO0,
	.gs_level = MTK_DVO_8K_30FPS,
};

void dp_intf_config(const struct edid *edid)
{
	const struct mtk_dvo *data = &dvo_data;

	printk(BIOS_INFO, "[eDPTX] dvo begin\n");
	mtk_dvo_power_on(data, edid);
	mtk_dvo_set_display_mode(data, edid);
}
