/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>

#define SIZE(w, h) ((u32)(h) << 16 | (w))

static void blender_config(struct blender *reg, u16 width, u16 height,
			   enum mtk_disp_blender_layer type)
{
	setbits32(&reg->shadow_ctl, BIT(2));

	switch (type) {
	case FIRST_BLENDER:
		clrsetbits32(&reg->datapath_con, BIT(4) | BIT(5), BIT(5));
		break;
	case LAST_BLENDER:
		clrsetbits32(&reg->datapath_con, BIT(0) | BIT(4) | BIT(5), BIT(0) | BIT(4));
		break;
	case SINGLE_BLENDER:
		clrsetbits32(&reg->datapath_con, BIT(4) | BIT(5), BIT(4));
		break;
	case OTHER_BLENDER:
		clrsetbits32(&reg->datapath_con, BIT(0) | BIT(4) | BIT(5), BIT(0) | BIT(5));
		break;
	default:
		printk(BIOS_ERR, "%s: unknown blender type %d\n", __func__, type);
		break;
	}

	write32(&reg->roi_size, SIZE(width, height));
	write32(&reg->bld_l_size, SIZE(width, height));
	write32(&reg->bg_clr, 0xFF000000);
}

static void blender_start(struct blender *reg)
{
	setbits32(&reg->bld_en, BIT(0));
}

static void outproc_config(struct outproc *reg, u16 width, u16 height)
{
	setbits32(&reg->shadow_ctl, BIT(2));
	setbits32(&reg->datapath_con, BIT(26));
	write32(&reg->roi_size, SIZE(width, height));
}

static void outproc_start(struct outproc *reg)
{
	setbits32(&reg->outproc_en, BIT(0));
}

static void mdp_rsz_config(struct disp_mdp_rsz_regs *reg, u16 width, u16 height)
{
	write32(&reg->input_size, SIZE(width, height));
	write32(&reg->output_size, SIZE(width, height));
}

static void mdp_rsz_start(struct disp_mdp_rsz_regs *reg)
{
	clrbits32(&reg->en, BIT(0));
}

static void postmask_config(struct disp_postmask_regs *reg, u16 width, u16 height)
{
	write32(&reg->size, SIZE(width, height));
	setbits32(&reg->cfg, BIT(0));
}

static void postmask_start(struct disp_postmask_regs *reg)
{
	setbits32(&reg->en, BIT(0));
}

static void tdshp_config(struct disp_tdshp_regs *reg, u16 width, u16 height)
{
	write32(&reg->input_size, SIZE(width, height));
	write32(&reg->output_size, SIZE(width, height));
	write32(&reg->output_offset, 0x0);
	write32(&reg->cfg, 0x1);
	setbits32(&reg->tdshp_00, BIT(31));
}

static void tdshp_start(struct disp_tdshp_regs *reg)
{
	setbits32(&reg->ctrl, BIT(0));
}

static void ccorr_config(struct disp_ccorr_regs *reg, u16 width, u16 height)
{
	write32(&reg->size, SIZE(width, height));
	setbits32(&reg->cfg, BIT(1));
}

static void ccorr_start(struct disp_ccorr_regs *reg)
{
	setbits32(&reg->en, BIT(0));
}

static void gamma_config(struct disp_gamma_regs *reg, u16 width, u16 height)
{
	write32(&reg->size, SIZE(width, height));
}

static void gamma_start(struct disp_gamma_regs *reg)
{
	setbits32(&reg->en, BIT(0));
}

static void dither_config(struct disp_dither_regs *reg, u16 width, u16 height)
{
	write32(&reg->size, SIZE(width, height));
	setbits32(&reg->cfg, BIT(0));
}

static void dither_start(struct disp_dither_regs *reg)
{
	setbits32(&reg->en, BIT(0));
}

static void disp_config_main_path_connection(enum disp_path_sel path)
{
	/* ovlsys */
	setbits32(&ovlsys_cfg->bypass_mux_shadow, BIT(0));
	write32(&ovlsys_cfg->cb_con, 0xFF << 16);
	setbits32(&ovlsys_cfg->rsz_in_cb2, BIT(1));
	setbits32(&ovlsys_cfg->exdma_out_cb3, BIT(2));
	setbits32(&ovlsys_cfg->blender_out_cb4, BIT(0));
	setbits32(&ovlsys_cfg->outproc_out_cb0, BIT(0));

	/* dispsys */
	write32(&mmsys_cfg->bypass_mux_shadow, 0xFF << 16 | BIT(0));
	setbits32(&mmsys_cfg->pq_in_cb0, BIT(0));
	setbits32(&mmsys_cfg->disp_mdp_rsz0_mout, BIT(0));
	write32(&mmsys_cfg->disp_tdshp0_sout, 0x2);
	write32(&mmsys_cfg->disp_ccorr0_sel, 0x2);
	write32(&mmsys_cfg->disp_ccorr0_sout, 0x1);
	write32(&mmsys_cfg->disp_ccorr1_sel, 0x1);
	write32(&mmsys_cfg->disp_ccorr1_sout, 0x1);
	write32(&mmsys_cfg->disp_gamma0_sel, 0x1);
	write32(&mmsys_cfg->disp_postmask_sout, 0x0);
	setbits32(&mmsys_cfg->pq_out_cb0, BIT(1));
	setbits32(&mmsys_cfg->panel_comp_out_cb1, BIT(1));

	/* dispsys1 */
	write32(&mmsys1_cfg->bypass_mux_shadow, 0xFF << 16 | BIT(0));
	setbits32(&mmsys1_cfg->splitter_in_cb1, BIT(5));
	setbits32(&mmsys1_cfg->splitter_out_cb9, BIT(10));
	setbits32(&mmsys1_cfg->comp_out_cb6, BIT(0));
	if (path == DISP_PATH_EDP)
		setbits32(&mmsys1_cfg->merge_out_cb0, BIT(9));
	else
		setbits32(&mmsys1_cfg->merge_out_cb0, BIT(0));
}

static void async_config(u16 width, u16 height)
{
	write32(&ovlsys_cfg->relay5_size, SIZE(width, height));
	write32(&mmsys_cfg->dl_in_relay0, BIT(30) | SIZE(width, height));
	write32(&mmsys_cfg->dl_out_relay1, BIT(30) | SIZE(width, height));
	write32(&mmsys1_cfg->dl_in_relay21, BIT(30) | SIZE(width, height));
}

static void disp_config_main_path_mutex(enum disp_path_sel path)
{
	u32 val;

	/* ovlsys mutex */
	write32(&mmsys_mutex[OVL0]->mutex[0].mod, MUTEX_MOD_OVL_MAIN_PATH);
	write32(&mmsys_mutex[OVL0]->mutex[0].mod1, MUTEX_MOD1_OVL_MAIN_PATH);

	/* dispsys mutex */
	write32(&mmsys_mutex[DISP0]->mutex[0].mod, MUTEX_MOD_DISP_MAIN_PATH);
	write32(&mmsys_mutex[DISP0]->mutex[0].mod1, MUTEX_MOD1_DISP_MAIN_PATH);

	/* dispsys1 mutex */
	if (path == DISP_PATH_EDP)
		write32(&mmsys_mutex[DISP1]->mutex[0].mod, MUTEX_MOD_DISP1_MAIN_PATH_EDP);
	else
		write32(&mmsys_mutex[DISP1]->mutex[0].mod, MUTEX_MOD_DISP1_MAIN_PATH_DSI0);

	/* mutex source from DVO */
	if (path == DISP_PATH_EDP)
		val = MUTEX_SOF_DVO | (MUTEX_SOF_DVO << 7);
	else
		val = MUTEX_SOF_DSI0 | (MUTEX_SOF_DSI0 << 7);
	write32(&mmsys_mutex[OVL0]->mutex[0].ctl, val);
	write32(&mmsys_mutex[DISP0]->mutex[0].ctl, val);
	write32(&mmsys_mutex[DISP1]->mutex[0].ctl, val);

	/* mutex enable */
	write32(&mmsys_mutex[OVL0]->mutex[0].en, BIT(0));
	write32(&mmsys_mutex[DISP0]->mutex[0].en, BIT(0));
	write32(&mmsys_mutex[DISP1]->mutex[0].en, BIT(0));
}

static void main_disp_path_setup(u16 width, u16 height, u32 vrefresh, enum disp_path_sel path)
{
	int i;
	enum mtk_disp_blender_layer blender_type[ARRAY_SIZE(blenders)] = {
		FIRST_BLENDER,
		OTHER_BLENDER,
		OTHER_BLENDER,
		LAST_BLENDER,
	};

	/* ovlsys config */
	for (i = 0; i < ARRAY_SIZE(blenders); i++) {
		blender_config(blenders[i], width, height, blender_type[i]);
		blender_start(blenders[i]);
	}

	outproc_config(outproc0_reg, width, height);
	outproc_start(outproc0_reg);

	/* disp config */
	mdp_rsz_config(disp_mdp_rsz0_reg, width, height);
	mdp_rsz_start(disp_mdp_rsz0_reg);
	tdshp_config(disp_tdshp0_reg, width, height);
	tdshp_start(disp_tdshp0_reg);
	ccorr_config(disp_ccorr0_reg, width, height);
	ccorr_start(disp_ccorr0_reg);
	ccorr_config(disp_ccorr1_reg, width, height);
	ccorr_start(disp_ccorr1_reg);
	gamma_config(disp_gamma0_reg, width, height);
	gamma_start(disp_gamma0_reg);
	postmask_config(disp_postmask0_reg, width, height);
	postmask_start(disp_postmask0_reg);
	dither_config(disp_dither0_reg, width, height);
	dither_start(disp_dither0_reg);

	/* async config */
	async_config(width, height);

	/* path connect */
	disp_config_main_path_connection(path);

	/* mutex config */
	disp_config_main_path_mutex(path);
}

static void disp_clock_on(void)
{
	clrbits32(&mmsys_cfg->mmsys_cg_con0, CG_CON_ALL);
	clrbits32(&mmsys_cfg->mmsys_cg_con1, CG_CON_ALL);
	clrbits32(&mmsys_cfg->mmsys_cg_con2, CG_CON_ALL);
	clrbits32(&mmsys1_cfg->mmsys_cg_con0, CG_CON_ALL);
	clrbits32(&mmsys1_cfg->mmsys_cg_con1, CG_CON_ALL);
	clrbits32(&mmsys1_cfg->mmsys_cg_con2, CG_CON_ALL);
	clrbits32(&ovlsys_cfg->mmsys_cg_con0, CG_CON_ALL);
	clrbits32(&ovlsys_cfg->mmsys_cg_con1, CG_CON_ALL);
	clrbits32(&ovlsys_cfg->mmsys_cg_con2, CG_CON_ALL);
}

static void ovlsys_layer_config(u32 fmt, u32 bpp, u16 width, u16 height)
{
	/* exdma config */
	write32(&exdma2_reg->roi_size, SIZE(width, height));
	write32(&exdma2_reg->ovl_l_size, SIZE(width, height));
	write32(&exdma2_reg->pitch, (width * bpp) & 0xFFFF);
	write32(&exdma2_reg->ovl_l_clrfmt, fmt);

	/* exdma start */
	clrsetbits32(&exdma2_reg->rdma_burst_ctl, BIT(28) | BIT(30) | BIT(31),
		     BIT(28) | BIT(31));
	setbits32(&exdma2_reg->dummy, BIT(2) | BIT(3));
	setbits32(&exdma2_reg->datapath_con, BIT(0) | BIT(24) | BIT(25));
	clrsetbits32(&exdma2_reg->ovl_mout, BIT(0) | BIT(1), BIT(1));
	write32(&exdma2_reg->gdrdy_period, 0xFFFFFFFF);

	/* Enable layer */
	setbits32(&exdma2_reg->rdma0_ctl, BIT(0));
	setbits32(&blenders[0]->bld_l_fmt, fmt);
}

void mtk_ddp_init(void)
{
	disp_clock_on();
}

void mtk_ddp_mode_set(const struct edid *edid, enum disp_path_sel path)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;
	u32 width = edid->mode.ha;
	u32 height = edid->mode.va;
	u32 vrefresh = edid->mode.refresh;

	printk(BIOS_DEBUG, "%s: display resolution: %ux%u@%u bpp %u\n", __func__, width, height,
	       vrefresh, bpp);

	if (!vrefresh) {
		if (!width || !height)
			vrefresh = 60;
		else
			vrefresh = edid->mode.pixel_clock * 1000 /
				   ((width + edid->mode.hbl) * (height + edid->mode.vbl));

		printk(BIOS_WARNING, "%s: vrefresh is not provided; using %u\n", __func__,
		       vrefresh);
	}

	if (width > 0x1FFF || height > 0x1FFF)
		printk(BIOS_WARNING, "%s: w/h: %d/%d exceed hw limit %u\n", __func__,
		       width, height, 0x1FFF);

	main_disp_path_setup(width, height, vrefresh, path);
	ovlsys_layer_config(fmt, bpp, width, height);
}
