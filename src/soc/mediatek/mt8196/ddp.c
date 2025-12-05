/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>
#include <soc/display.h>

#define SIZE(w, h) ((u32)(h) << 16 | (w))
#define DUAL_PIPE(path) ((path) == DISP_PATH_DUAL_MIPI)

struct disp_pipe_regs {
	struct disp_mdp_rsz_regs *const mdp_rsz;
	struct disp_tdshp_regs *const tdshp;
	struct disp_ccorr_regs *const ccorr0;
	struct disp_ccorr_regs *const ccorr1;
	struct disp_gamma_regs *const gamma;
	struct disp_postmask_regs *const postmask;
	struct disp_dither_regs *const dither;
	struct disp_dsc_regs *const dsc;
};

static const struct disp_pipe_regs disp_pipe0_regs = {
	.mdp_rsz  = disp_mdp_rsz0_reg,
	.tdshp    = disp_tdshp0_reg,
	.ccorr0   = disp_ccorr0_reg,
	.ccorr1   = disp_ccorr1_reg,
	.gamma    = disp_gamma0_reg,
	.postmask = disp_postmask0_reg,
	.dither   = disp_dither0_reg,
	.dsc      = disp_dsc2_reg,
};

static const struct disp_pipe_regs disp_pipe1_regs = {
	.mdp_rsz  = disp_mdp_rsz1_reg,
	.tdshp    = disp_tdshp1_reg,
	.ccorr0   = disp_ccorr2_reg,
	.ccorr1   = disp_ccorr3_reg,
	.gamma    = disp_gamma1_reg,
	.postmask = disp_postmask1_reg,
	.dither   = disp_dither1_reg,
	.dsc      = disp_dsc3_reg,
};

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

static void dsc_config(struct disp_dsc_regs *reg, u16 width, u16 height)
{
	write32(&reg->dsc_con, BIT(5));
	write32(&reg->pic_w, width);
	write32(&reg->pic_h, height);
	write32(&reg->chunk_size, width << 16);
}

static void dsc_start(struct disp_dsc_regs *reg)
{
	setbits32(&reg->dsc_con, BIT(0));
}

static void ovlsys_path_connect(struct ovlsys_cfg *reg)
{
	setbits32(&reg->bypass_mux_shadow, BIT(0));
	write32(&reg->cb_con, 0xFF << 16);
	setbits32(&reg->rsz_in_cb2, BIT(1));
	setbits32(&reg->exdma_out_cb3, BIT(2));
	setbits32(&reg->blender_out_cb4, BIT(0));
	setbits32(&reg->outproc_out_cb0, BIT(0));
}

static void disp_config_main_path_connection(enum disp_path_sel path)
{
	/* ovlsys */
	ovlsys_path_connect(ovlsys_cfg);

	/* dispsys */
	write32(&mmsys_cfg->bypass_mux_shadow, 0xFF << 16 | BIT(0));
	setbits32(&mmsys_cfg->pq_in_cb[0], BIT(0));
	setbits32(&mmsys_cfg->disp_mdp_rsz0_mout, BIT(0));
	write32(&mmsys_cfg->disp_tdshp0_sout, 0x2);
	write32(&mmsys_cfg->disp_ccorr0_sel, 0x2);
	write32(&mmsys_cfg->disp_ccorr0_sout, 0x1);
	write32(&mmsys_cfg->disp_ccorr1_sel, 0x1);
	write32(&mmsys_cfg->disp_ccorr1_sout, 0x1);
	write32(&mmsys_cfg->disp_gamma0_sel, 0x1);
	write32(&mmsys_cfg->disp_postmask_sout, 0x0);
	setbits32(&mmsys_cfg->pq_out_cb[0], BIT(1));
	setbits32(&mmsys_cfg->panel_comp_out_cb1, BIT(1));

	/* dispsys1 */
	write32(&mmsys1_cfg->bypass_mux_shadow, 0xFF << 16 | BIT(0));
	setbits32(&mmsys1_cfg->splitter_in_cb1, BIT(5));
	setbits32(&mmsys1_cfg->splitter_out_cb9, BIT(5));
	setbits32(&mmsys1_cfg->comp_out_cb3, BIT(0));

	if (DUAL_PIPE(path)) {
		/* ovlsys1 */
		ovlsys_path_connect(ovlsys1_cfg);
		/* dispsys */
		setbits32(&mmsys_cfg->pq_in_cb[8], BIT(1));
		setbits32(&mmsys_cfg->disp_mdp_rsz1_mout, BIT(0));
		write32(&mmsys_cfg->disp_tdshp1_sout, 0x2);
		write32(&mmsys_cfg->disp_ccorr2_sel, 0x2);
		write32(&mmsys_cfg->disp_ccorr2_sout, 0x1);
		write32(&mmsys_cfg->disp_ccorr3_sel, 0x1);
		write32(&mmsys_cfg->disp_ccorr3_sout, 0x1);
		write32(&mmsys_cfg->disp_gamma1_sel, 0x1);
		write32(&mmsys_cfg->disp_postmask1_sout, 0x0);
		setbits32(&mmsys_cfg->pq_out_cb[3], BIT(2));
		setbits32(&mmsys_cfg->panel_comp_out_cb2, BIT(2));

		/* dispsys1 */
		setbits32(&mmsys1_cfg->splitter_in_cb2, BIT(8));
		setbits32(&mmsys1_cfg->splitter_out_cb12, BIT(7));
		setbits32(&mmsys1_cfg->comp_out_cb4, BIT(3));
	}

	if (path == DISP_PATH_EDP) {
		setbits32(&mmsys1_cfg->merge_out_cb0, BIT(9));
	} else if (path == DISP_PATH_MIPI) {
		setbits32(&mmsys1_cfg->merge_out_cb0, BIT(0));
	} else {
		setbits32(&mmsys1_cfg->merge_out_cb0, BIT(0));
		setbits32(&mmsys1_cfg->merge_out_cb3, BIT(3));
	}
}

static void async_config(u16 width, u16 height, enum disp_path_sel path)
{
	u32 relay = BIT(30) | SIZE(width, height);

	write32(&ovlsys_cfg->relay5_size, SIZE(width, height));
	write32(&mmsys_cfg->dl_in_relay[0], relay);
	write32(&mmsys_cfg->dl_out_relay[1], relay);
	write32(&mmsys1_cfg->dl_in_relay21, relay);

	if (DUAL_PIPE(path)) {
		write32(&ovlsys1_cfg->relay5_size, SIZE(width, height));
		write32(&mmsys_cfg->dl_in_relay[8], relay);
		write32(&mmsys_cfg->dl_out_relay[2], relay);
		write32(&mmsys1_cfg->dl_in_relay22, relay);
	}
}

static void disp_config_main_path_mutex(enum disp_path_sel path)
{
	u32 val, val1;

	/* ovlsys mutex */
	write32(&mmsys_mutex[OVL0]->mutex[0].mod, MUTEX_MOD_OVL_MAIN_PATH);
	write32(&mmsys_mutex[OVL0]->mutex[0].mod1, MUTEX_MOD1_OVL_MAIN_PATH);

	if (DUAL_PIPE(path)) {
		write32(&mmsys_mutex[OVL1]->mutex[0].mod, MUTEX_MOD_OVL_MAIN_PATH_DUAL);
		write32(&mmsys_mutex[OVL1]->mutex[0].mod1, MUTEX_MOD1_OVL_MAIN_PATH_DUAL);
	}

	/* dispsys mutex */
	val = MUTEX_MOD_DISP_MAIN_PATH;
	val1 = MUTEX_MOD1_DISP_MAIN_PATH;
	if (DUAL_PIPE(path)) {
		val |= MUTEX_MOD_DISP_MAIN_PATH_DUAL;
		val1 |= MUTEX_MOD1_DISP_MAIN_PATH_DUAL;
	}
	write32(&mmsys_mutex[DISP0]->mutex[0].mod, val);
	write32(&mmsys_mutex[DISP0]->mutex[0].mod1, val1);

	/* dispsys1 mutex */
	if (path == DISP_PATH_EDP)
		val = MUTEX_MOD_DISP1_MAIN_PATH_EDP;
	else if (path == DISP_PATH_MIPI)
		val = MUTEX_MOD_DISP1_MAIN_PATH_DSI0;
	else
		val = MUTEX_MOD_DISP1_MAIN_PATH_DUAL_DSI;
	write32(&mmsys_mutex[DISP1]->mutex[0].mod, val);

	/* mutex source from DVO */
	if (path == DISP_PATH_EDP)
		val = MUTEX_SOF_DVO | (MUTEX_SOF_DVO << 7);
	else
		val = MUTEX_SOF_DSI0 | (MUTEX_SOF_DSI0 << 7);
	write32(&mmsys_mutex[OVL0]->mutex[0].ctl, val);
	if (DUAL_PIPE(path))
		write32(&mmsys_mutex[OVL1]->mutex[0].ctl, val);
	write32(&mmsys_mutex[DISP0]->mutex[0].ctl, val);
	write32(&mmsys_mutex[DISP1]->mutex[0].ctl, val);

	/* mutex enable */
	write32(&mmsys_mutex[OVL0]->mutex[0].en, BIT(0));
	if (DUAL_PIPE(path))
		write32(&mmsys_mutex[OVL1]->mutex[0].en, BIT(0));
	write32(&mmsys_mutex[DISP0]->mutex[0].en, BIT(0));
	write32(&mmsys_mutex[DISP1]->mutex[0].en, BIT(0));
}

static void disp_config_blender(struct blender *const blenders[], size_t size, u16 width,
				u16 height)
{
	int i;
	static const enum mtk_disp_blender_layer blender_type[] = {
		FIRST_BLENDER,
		OTHER_BLENDER,
		OTHER_BLENDER,
		LAST_BLENDER,
	};

	assert(size <= ARRAY_SIZE(blender_type));

	/* ovlsys config */
	for (i = 0; i < size; i++) {
		blender_config(blenders[i], width, height, blender_type[i]);
		blender_start(blenders[i]);
	}
}

static void main_disp_path_setup(u16 width, u16 height, u32 vrefresh, enum disp_path_sel path)
{
	u16 w = width;
	size_t num_pipe = DUAL_PIPE(path) ? 2 : 1;
	const struct disp_pipe_regs pipes[] = {disp_pipe0_regs, disp_pipe1_regs};

	if (DUAL_PIPE(path))
		w /= 2;

	disp_config_blender(ovl_blenders, ARRAY_SIZE(ovl_blenders), w, height);
	if (DUAL_PIPE(path))
		disp_config_blender(ovl1_blenders, ARRAY_SIZE(ovl1_blenders), w, height);

	outproc_config(ovl_outproc0_reg, w, height);
	outproc_start(ovl_outproc0_reg);
	if (DUAL_PIPE(path)) {
		outproc_config(ovl1_outproc0_reg, w, height);
		outproc_start(ovl1_outproc0_reg);
	}

	/* disp config */
	for (int i = 0; i < num_pipe; i++) {
		mdp_rsz_config(pipes[i].mdp_rsz, w, height);
		mdp_rsz_start(pipes[i].mdp_rsz);
		tdshp_config(pipes[i].tdshp, w, height);
		tdshp_start(pipes[i].tdshp);
		ccorr_config(pipes[i].ccorr0, w, height);
		ccorr_start(pipes[i].ccorr0);
		ccorr_config(pipes[i].ccorr1, w, height);
		ccorr_start(pipes[i].ccorr1);
		gamma_config(pipes[i].gamma, w, height);
		gamma_start(pipes[i].gamma);
		postmask_config(pipes[i].postmask, w, height);
		postmask_start(pipes[i].postmask);
		dither_config(pipes[i].dither, w, height);
		dither_start(pipes[i].dither);
		dsc_config(pipes[i].dsc, w, height);
		dsc_start(pipes[i].dsc);
	}

	/* async config */
	async_config(w, height, path);

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

static void ovlsys_layer_config_pipe(struct exdma *exdma, struct blender *blender,
				     u32 fmt, u16 width, u16 height, u16 pitch)
{
	/* exdma config */
	write32(&exdma->roi_size, SIZE(width, height));
	write32(&exdma->ovl_l_size, SIZE(width, height));
	write32(&exdma->pitch, pitch & 0xFFFF);
	write32(&exdma->ovl_l_clrfmt, fmt);

	/* exdma start */
	clrsetbits32(&exdma->rdma_burst_ctl, BIT(28) | BIT(30) | BIT(31),
		     BIT(28) | BIT(31));
	setbits32(&exdma->dummy, BIT(2) | BIT(3));
	setbits32(&exdma->datapath_con, BIT(0) | BIT(24) | BIT(25));
	clrsetbits32(&exdma->ovl_mout, BIT(0) | BIT(1), BIT(1));
	write32(&exdma->gdrdy_period, 0xFFFFFFFF);

	/* Enable layer */
	setbits32(&exdma->rdma0_ctl, BIT(0));
	setbits32(&blender->bld_l_fmt, fmt);
}

static void ovlsys_layer_config(u32 fmt, u32 bpp, u16 width, u16 height,
				enum disp_path_sel path)
{
	u16 w = width;

	if (DUAL_PIPE(path))
		w /= 2;

	ovlsys_layer_config_pipe(ovl_exdma2_reg, ovl_blenders[0], fmt, w, height, width * bpp);
	if (DUAL_PIPE(path))
		ovlsys_layer_config_pipe(ovl1_exdma2_reg, ovl1_blenders[0], fmt, w, height,
					 width * bpp);
}

void mtk_ddp_init(void)
{
	disp_clock_on();
}

void mtk_ddp_soc_mode_set(u32 fmt, u32 bpp, u32 width, u32 height, u32 vrefresh,
			  enum disp_path_sel path, struct dsc_config *dsc_config)
{
	if (width > 0x1FFF || height > 0x1FFF)
		printk(BIOS_WARNING, "%s: w/h: %d/%d exceed hw limit %u\n", __func__,
		       width, height, 0x1FFF);
	main_disp_path_setup(width, height, vrefresh, path);
	ovlsys_layer_config(fmt, bpp, width, height, path);
}

void mtk_ddp_ovlsys_start(uintptr_t fb_addr)
{
	write32(&ovl_exdma2_reg->ovl_addr, fb_addr);
	setbits32(&ovl_exdma2_reg->ovl_en, BIT(0));
	setbits32(&ovl_exdma2_reg->ovl_l_en, BIT(0));
	setbits32(&ovl_blenders[0]->bld_l_en, BIT(0));
}
