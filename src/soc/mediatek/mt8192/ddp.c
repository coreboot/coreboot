/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>

static void disp_config_main_path_connection(void)
{
	/* ovl0->ovl0_2l */
	write32(&mmsys_cfg->mmsys_ovl_mout_en,
		(DISP_OVL0_GO_BG | DISP_OVL0_2L_GO_BLEND));
	write32(&mmsys_cfg->ovl0_2l_mout_en, OVL0_MOUT_EN_DISP_RDMA0);
	write32(&mmsys_cfg->rdma0_sel_in, RDMA0_SEL_IN_OVL0_2L);
	write32(&mmsys_cfg->rdma0_sout_sel, RDMA0_SOUT_COLOR0);
	write32(&mmsys_cfg->ccorr0_sout_sel, CCORR0_SOUT_AAL0);
	write32(&mmsys_cfg->aal0_sel_in, AAL0_SEL_IN_CCORR0);
	write32(&mmsys_cfg->dither0_mout_en, DITHER0_MOUT_DSI0);
	write32(&mmsys_cfg->dsi0_sel_in, DSI0_SEL_IN_DITHER0);
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

static void ovl_layer_smi_id_en(u32 idx)
{
	printk(BIOS_INFO, "%s\n", __func__);

	setbits32(&disp_ovl[idx]->datapath_con, BIT(0));
}

static void ccorr_config(u32 width, u32 height)
{
	struct disp_ccorr_regs *const regs = disp_ccorr;
	int enable_relay = 0;

	printk(BIOS_INFO, "%s\n", __func__);

	write32(&regs->size, width << 16 | height);

	if (enable_relay) {
		setbits32(&regs->cfg, PQ_RELAY_MODE);
		clrbits32(&regs->cfg, PQ_ENGINE_EN);
	} else {
		clrbits32(&regs->cfg, PQ_RELAY_MODE);
		setbits32(&regs->cfg, PQ_ENGINE_EN);
	}

	write32(&regs->en, PQ_EN);
}

static void aal_config(u32 width, u32 height)
{
	struct disp_aal_regs *const regs = disp_aal;
	int enable_relay = 1;

	printk(BIOS_INFO, "%s\n", __func__);

	write32(&regs->size, width << 16 | height);
	write32(&regs->output_size, width << 16 | height);

	if (enable_relay) {
		setbits32(&regs->cfg, PQ_RELAY_MODE);
		clrbits32(&regs->cfg, PQ_ENGINE_EN);
	} else {
		clrbits32(&regs->cfg, PQ_RELAY_MODE);
		setbits32(&regs->cfg, PQ_ENGINE_EN);
	}

	write32(&regs->en, PQ_EN);
}

static void gamma_config(u32 width, u32 height)
{
	struct disp_gamma_regs *const regs = disp_gamma;
	int enable_relay = 0;

	printk(BIOS_INFO, "%s\n", __func__);

	write32(&regs->size, width << 16 | height);

	if (enable_relay)
		setbits32(&regs->cfg, PQ_RELAY_MODE);
	else
		clrbits32(&regs->cfg, PQ_RELAY_MODE);

	write32(&regs->en, PQ_EN);
}

static void postmask_config(u32 width, u32 height)
{
	struct disp_postmask_regs *const regs = disp_postmask;
	int enable_relay = 1;

	printk(BIOS_INFO, "%s\n", __func__);

	write32(&regs->size, width << 16 | height);

	if (enable_relay)
		setbits32(&regs->cfg, PQ_RELAY_MODE);
	else
		clrbits32(&regs->cfg, PQ_RELAY_MODE);

	write32(&regs->en, PQ_EN);
}

static void dither_config(u32 width, u32 height)
{
	struct disp_dither_regs *const regs = disp_dither;
	int enable_relay = 1;

	printk(BIOS_INFO, "%s\n", __func__);

	write32(&regs->size, width << 16 | height);

	if (enable_relay)
		setbits32(&regs->cfg, PQ_RELAY_MODE);
	else
		clrbits32(&regs->cfg, PQ_RELAY_MODE);

	write32(&regs->en, PQ_EN);
}


static void main_disp_path_setup(u32 width, u32 height, u32 vrefresh)
{
	u32 idx = 0;
	u32 pixel_clk = width * height * vrefresh;

	printk(BIOS_INFO, "%s\n", __func__);

	for (idx = 0; idx < MAIN_PATH_OVL_NR; idx++) {
		ovl_set_roi(idx, width, height, idx ? 0 : 0xff0000ff);
		ovl_layer_smi_id_en(idx);
	}

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

	clrbits32(&mmsys_cfg->mmsys_cg_con1, CG_CON1_DISP_ALL);

	clrbits32(&mmsys_cfg->mmsys_cg_con2, CG_CON2_DISP_ALL);
}

void mtk_ddp_init(void)
{
	disp_clock_on();
	/* Turn off M4U port. */
	write32((void *)(SMI_LARB0 + SMI_LARB_PORT_L0_OVL_RDMA0), 0);
}

void mtk_ddp_mode_set(const struct edid *edid)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;
	u32 width = edid->mode.ha;
	u32 height = edid->mode.va;
	u32 vrefresh = edid->mode.refresh;

	printk(BIOS_INFO, "%s display resolution: %dx%d@%d bpp %d\n",
		__func__, width, height, vrefresh, bpp);

	if (!vrefresh) {
		vrefresh = 60;
		printk(BIOS_INFO, "%s invalid vrefresh %d\n",
		__func__, vrefresh);
	}

	main_disp_path_setup(width, height, vrefresh);
	rdma_start();
	ovl_layer_config(fmt, bpp, width, height);
	ovl_bgclr_in_sel(1);
}
