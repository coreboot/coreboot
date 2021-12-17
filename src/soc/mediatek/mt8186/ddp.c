/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 6.9
 */

#include <console/console.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>

static void disp_config_main_path_connection(void)
{
	/*
	 * Main path:
	 * OVL0->RDMA0->COLOR0->CCORR0->AAL0->GAMMA->POSTMASK0->DITHER->DSI0
	 */
	SET32_BITFIELDS(&mmsys_cfg->disp_ovl0_mout_en,
			DISP_OVL0_MOUT_EN, DISP_OVL0_MOUT_TO_RDMA0);
	SET32_BITFIELDS(&mmsys_cfg->disp_rdma0_sel_in,
			DISP_RDMA0_SEL_IN, DISP_RDMA0_FROM_OVL0);
	SET32_BITFIELDS(&mmsys_cfg->mmsys_ovl_con,
			DISP_MMSYS_OVL0_CON, DISP_OVL0_GO_BLEND);
	SET32_BITFIELDS(&mmsys_cfg->disp_rdma0_sout_sel,
			DISP_RDMA0_SOUT_SEL, DISP_RDMA0_SOUT_TO_COLOR0);
	SET32_BITFIELDS(&mmsys_cfg->disp_dither0_mout_en,
			DISP_DITHER0_MOUT_EN, DISP_DITHER0_MOUT_TO_DSI0);
	SET32_BITFIELDS(&mmsys_cfg->disp_dsi0_sel_in,
			DISP_DSI0_SEL_IN, DISP_DSI0_FROM_DITHER0);
}

static void disp_config_main_path_mutex(void)
{
	write32(&disp_mutex->mutex[0].mod, MUTEX_MOD_MAIN_PATH);

	/* Clock source from DSI0 */
	write32(&disp_mutex->mutex[0].ctl,
		MUTEX_SOF_DSI0 | (MUTEX_SOF_DSI0 << 6));
	write32(&disp_mutex->mutex[0].en, BIT(0));
}

static void ovl_layer_smi_id_en(u32 idx)
{
	SET32_BITFIELDS(&disp_ovl[idx]->datapath_con,
			SMI_ID_EN, SMI_ID_EN_VAL);
}

static void ccorr_config(u32 width, u32 height)
{
	struct disp_ccorr_regs *const regs = disp_ccorr;

	write32(&regs->size, width << 16 | height);

	/* Disable relay mode */
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_RELAY_MODE, 0);
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_ENGINE_EN, PQ_ENGINE_EN);

	write32(&regs->en, PQ_EN);
}

static void aal_config(u32 width, u32 height)
{
	struct disp_aal_regs *const regs = disp_aal;

	write32(&regs->size, width << 16 | height);
	write32(&regs->output_size, width << 16 | height);

	/* Enable relay mode */
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_RELAY_MODE, PQ_RELAY_MODE);
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_ENGINE_EN, 0);

	write32(&regs->en, PQ_EN);
}

static void gamma_config(u32 width, u32 height)
{
	struct disp_gamma_regs *const regs = disp_gamma;

	write32(&regs->size, width << 16 | height);

	/* Disable relay mode */
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_RELAY_MODE, 0);

	write32(&regs->en, PQ_EN);
}

static void postmask_config(u32 width, u32 height)
{
	struct disp_postmask_regs *const regs = disp_postmask;

	write32(&regs->size, width << 16 | height);

	/* Enable relay mode */
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_RELAY_MODE, PQ_RELAY_MODE);

	write32(&regs->en, PQ_EN);
}

static void dither_config(u32 width, u32 height)
{
	struct disp_dither_regs *const regs = disp_dither;

	write32(&regs->size, width << 16 | height);

	/* Enable relay mode */
	SET32_BITFIELDS(&regs->cfg, PQ_CFG_RELAY_MODE, PQ_RELAY_MODE);

	write32(&regs->en, PQ_EN);
}

static void main_disp_path_setup(u32 width, u32 height, u32 vrefresh)
{
	u32 pixel_clk = width * height * vrefresh;

	/* One ovl in main path */
	ovl_set_roi(0, width, height, 0xff0000ff);
	ovl_layer_smi_id_en(0);
	rdma_config(width, height, pixel_clk, 5 * KiB);
	color_start(width, height);
	ccorr_config(width, height);
	aal_config(width, height);
	gamma_config(width, height);
	postmask_config(width, height);
	dither_config(width, height);
	disp_config_main_path_connection();
	disp_config_main_path_mutex();
}

static void disp_clock_on(void)
{
	clrbits32(&mmsys_cfg->mmsys_cg_con0, CG_CON0_DISP_ALL);
	clrbits32(&mmsys_cfg->mmsys_cg_con2, CG_CON2_DISP_ALL);
}

void mtk_ddp_init(void)
{
	disp_clock_on();

	/* Turn off M4U port */
	write32((void *)(SMI_LARB0 + SMI_LARB_PORT_L0_OVL_RDMA0), 0);
}

void mtk_ddp_mode_set(const struct edid *edid)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;
	u32 width = edid->mode.ha;
	u32 height = edid->mode.va;
	u32 vrefresh = edid->mode.refresh;

	printk(BIOS_INFO, "%s: display resolution: %ux%u@%u bpp %u\n",
	       __func__, width, height, vrefresh, bpp);

	if (!vrefresh) {
		vrefresh = 60;
		printk(BIOS_INFO, "%s: invalid vrefresh; setting to %u\n",
		       __func__, vrefresh);
	}

	main_disp_path_setup(width, height, vrefresh);
	rdma_start();
	ovl_layer_config(fmt, bpp, width, height);
}
