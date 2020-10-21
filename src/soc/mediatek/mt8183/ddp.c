/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>
#include <types.h>

static void disp_config_main_path_connection(void)
{
	write32(&mmsys_cfg->disp_ovl0_mout_en, OVL0_MOUT_EN_OVL0_2L);
	write32(&mmsys_cfg->disp_ovl0_2l_mout_en, OVL0_2L_MOUT_EN_DISP_PATH0);
	write32(&mmsys_cfg->disp_path0_sel_in, DISP_PATH0_SEL_IN_OVL0_2L);
	write32(&mmsys_cfg->disp_rdma0_sout_sel_in, RDMA0_SOUT_SEL_IN_COLOR);
	write32(&mmsys_cfg->disp_dither0_mout_en, DITHER0_MOUT_EN_DISP_DSI0);
	write32(&mmsys_cfg->dsi0_sel_in, DSI0_SEL_IN_DITHER0_MOUT);
}

static void disp_config_main_path_mutex(void)
{
	write32(&disp_mutex->mutex[0].mod, MUTEX_MOD_MAIN_PATH);

	/* Clock source from DSI0 */
	write32(&disp_mutex->mutex[0].ctl,
		MUTEX_SOF_DSI0 | (MUTEX_SOF_DSI0 << 6));
	write32(&disp_mutex->mutex[0].en, BIT(0));
}

static void ovl_bgclr_in_sel(u32 idx)
{
	setbits32(&disp_ovl[idx]->datapath_con, BIT(2));
}

static void enable_pq(struct disp_pq_regs *const regs, u32 width, u32 height,
		      int enable_relay)
{
	write32(&regs->size, width << 16 | height);
	if (enable_relay)
		write32(&regs->cfg, PQ_RELAY_MODE);
	write32(&regs->en, PQ_EN);
}

static void main_disp_path_setup(u32 width, u32 height, u32 vrefresh)
{
	u32 idx = 0;
	u32 pixel_clk = width * height * vrefresh;

	for (idx = 0; idx < MAIN_PATH_OVL_NR; idx++)
		ovl_set_roi(idx, width, height, idx ? 0 : 0xff0000ff);

	rdma_config(width, height, pixel_clk, 5 * KiB);
	color_start(width, height);
	enable_pq(disp_ccorr, width, height, 1);
	enable_pq(disp_aal, width, height, 0);
	enable_pq(disp_gamma, width, height, 0);
	enable_pq(disp_dither, width, height, 1);
	disp_config_main_path_connection();
	disp_config_main_path_mutex();
}

static void disp_clock_on(void)
{
	clrbits32(&mmsys_cfg->mmsys_cg_con0, CG_CON0_DISP_ALL);

	clrbits32(&mmsys_cfg->mmsys_cg_con1, CG_CON1_DISP_DSI0 |
					     CG_CON1_DISP_DSI0_INTERFACE);
}

void mtk_ddp_init(void)
{
	disp_clock_on();
	/* Turn off M4U port. */
	write32((void *)(SMI_LARB0 + SMI_LARB_NON_SEC_CON), 0);
}

void mtk_ddp_mode_set(const struct edid *edid)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;
	u32 width = edid->mode.ha;
	u32 height = edid->mode.va;
	u32 vrefresh = edid->mode.refresh;

	main_disp_path_setup(width, height, vrefresh);
	rdma_start();
	ovl_layer_config(fmt, bpp, width, height);
	ovl_bgclr_in_sel(1);
}
