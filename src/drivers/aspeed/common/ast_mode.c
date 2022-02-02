/* SPDX-License-Identifier: MIT */
/*
 * Copied from Linux drivers/gpu/drm/ast/ast_mode.c
 * Please try to keep as close as possible to the upstream source.
 */
#include "ast_drv.h"
#include "ast_tables.h"

static inline void ast_load_palette_index(struct ast_private *ast,
				     u8 index, u8 red, u8 green,
				     u8 blue)
{
	ast_io_write8(ast, AST_IO_DAC_INDEX_WRITE, index);
	ast_io_read8(ast, AST_IO_SEQ_PORT);
	ast_io_write8(ast, AST_IO_DAC_DATA, red);
	ast_io_read8(ast, AST_IO_SEQ_PORT);
	ast_io_write8(ast, AST_IO_DAC_DATA, green);
	ast_io_read8(ast, AST_IO_SEQ_PORT);
	ast_io_write8(ast, AST_IO_DAC_DATA, blue);
	ast_io_read8(ast, AST_IO_SEQ_PORT);
}

static void ast_crtc_load_lut(struct drm_crtc *crtc)
{
	struct ast_private *ast = crtc->dev->dev_private;
	/* FIXME: Gamma cor 2.6 ? */
	for (int i = 0; i < 256; i++)
		ast_load_palette_index(ast, i, i, i, i);
}

static bool ast_get_vbios_mode_info(struct drm_crtc *crtc, struct drm_display_mode *mode,
				    struct drm_display_mode *adjusted_mode,
				    struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = crtc->dev->dev_private;
	const struct drm_framebuffer *fb = crtc->primary->fb;
	u32 refresh_rate_index = 0, mode_id, color_index, refresh_rate;
	const struct ast_vbios_enhtable *best = NULL;
	u32 hborder, vborder;
	bool check_sync;

	switch (fb->format->cpp[0] * 8) {
	case 8:
		vbios_mode->std_table = &vbios_stdtable[VGAModeIndex];
		color_index = VGAModeIndex - 1;
		break;
	case 16:
		vbios_mode->std_table = &vbios_stdtable[HiCModeIndex];
		color_index = HiCModeIndex;
		break;
	case 24:
	case 32:
		vbios_mode->std_table = &vbios_stdtable[TrueCModeIndex];
		color_index = TrueCModeIndex;
		break;
	default:
		return false;
	}

	switch (crtc->mode.crtc_hdisplay) {
	case 640:
		vbios_mode->enh_table = &res_640x480[refresh_rate_index];
		break;
	case 800:
		vbios_mode->enh_table = &res_800x600[refresh_rate_index];
		break;
	case 1024:
		vbios_mode->enh_table = &res_1024x768[refresh_rate_index];
		break;
	case 1280:
		if (crtc->mode.crtc_vdisplay == 800)
			vbios_mode->enh_table = &res_1280x800[refresh_rate_index];
		else
			vbios_mode->enh_table = &res_1280x1024[refresh_rate_index];
		break;
	case 1360:
		vbios_mode->enh_table = &res_1360x768[refresh_rate_index];
		break;
	case 1440:
		vbios_mode->enh_table = &res_1440x900[refresh_rate_index];
		break;
	case 1600:
		if (crtc->mode.crtc_vdisplay == 900)
			vbios_mode->enh_table = &res_1600x900[refresh_rate_index];
		else
			vbios_mode->enh_table = &res_1600x1200[refresh_rate_index];
		break;
	case 1680:
		vbios_mode->enh_table = &res_1680x1050[refresh_rate_index];
		break;
	case 1920:
		if (crtc->mode.crtc_vdisplay == 1080)
			vbios_mode->enh_table = &res_1920x1080[refresh_rate_index];
		else
			vbios_mode->enh_table = &res_1920x1200[refresh_rate_index];
		break;
	default:
		return false;
	}

	refresh_rate = mode->vrefresh;
	check_sync = vbios_mode->enh_table->flags & WideScreenMode;
	do {
		const struct ast_vbios_enhtable *loop = vbios_mode->enh_table;

		while (loop->refresh_rate != 0xff) {
			if ((check_sync) &&
			    (((mode->flags & DRM_MODE_FLAG_NVSYNC)  &&
			      (loop->flags & PVSync))  ||
			     ((mode->flags & DRM_MODE_FLAG_PVSYNC)  &&
			      (loop->flags & NVSync))  ||
			     ((mode->flags & DRM_MODE_FLAG_NHSYNC)  &&
			      (loop->flags & PHSync))  ||
			     ((mode->flags & DRM_MODE_FLAG_PHSYNC)  &&
			      (loop->flags & NHSync)))) {
				loop++;
				continue;
			}
			if (loop->refresh_rate <= refresh_rate
			    && (!best || loop->refresh_rate > best->refresh_rate))
				best = loop;
			loop++;
		}
		if (best || !check_sync)
			break;
		check_sync = 0;
	} while (1);
	if (best)
		vbios_mode->enh_table = best;

	hborder = (vbios_mode->enh_table->flags & HBorder) ? 8 : 0;
	vborder = (vbios_mode->enh_table->flags & VBorder) ? 8 : 0;

	adjusted_mode->crtc_htotal = vbios_mode->enh_table->ht;
	adjusted_mode->crtc_hblank_start = vbios_mode->enh_table->hde + hborder;
	adjusted_mode->crtc_hblank_end = vbios_mode->enh_table->ht - hborder;
	adjusted_mode->crtc_hsync_start = vbios_mode->enh_table->hde + hborder +
		vbios_mode->enh_table->hfp;
	adjusted_mode->crtc_hsync_end = (vbios_mode->enh_table->hde + hborder +
					 vbios_mode->enh_table->hfp +
					 vbios_mode->enh_table->hsync);

	adjusted_mode->crtc_vtotal = vbios_mode->enh_table->vt;
	adjusted_mode->crtc_vblank_start = vbios_mode->enh_table->vde + vborder;
	adjusted_mode->crtc_vblank_end = vbios_mode->enh_table->vt - vborder;
	adjusted_mode->crtc_vsync_start = vbios_mode->enh_table->vde + vborder +
		vbios_mode->enh_table->vfp;
	adjusted_mode->crtc_vsync_end = (vbios_mode->enh_table->vde + vborder +
					 vbios_mode->enh_table->vfp +
					 vbios_mode->enh_table->vsync);

	refresh_rate_index = vbios_mode->enh_table->refresh_rate_index;
	mode_id = vbios_mode->enh_table->mode_id;

	if (ast->chip == AST1180) {
		/* TODO 1180 */
	} else {
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x8c, (u8)((color_index & 0xf) << 4));
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x8d, refresh_rate_index & 0xff);
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x8e, mode_id & 0xff);

		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x91, 0x00);
		if (vbios_mode->enh_table->flags & NewModeInfo) {
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x91, 0xa8);
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x92,
					  fb->format->cpp[0] * 8);
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x93,
					  adjusted_mode->clock / 1000);
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x94,
					  adjusted_mode->crtc_hdisplay);
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x95,
					  adjusted_mode->crtc_hdisplay >> 8);

			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x96,
					  adjusted_mode->crtc_vdisplay);
			ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x97,
					  adjusted_mode->crtc_vdisplay >> 8);
		}
	}

	return true;
}

static void ast_set_std_reg(struct drm_crtc *crtc, struct drm_display_mode *mode,
			    struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = crtc->dev->dev_private;
	const struct drm_framebuffer *fb = crtc->primary->fb;
	const struct ast_vbios_stdtable *stdtable;
	u32 i;
	u8 jreg;

	switch (fb->format->cpp[0] * 8) {
	case 8:
		stdtable = &vbios_stdtable[VGAModeIndex];
		break;
	case 16:
		stdtable = &vbios_stdtable[HiCModeIndex];
		break;
	case 24:
	case 32:
		stdtable = &vbios_stdtable[TrueCModeIndex];
		break;
	default:
		return;
	}

	jreg = stdtable->misc;
	ast_io_write8(ast, AST_IO_MISC_PORT_WRITE, jreg);

	/* Set SEQ */
	ast_set_index_reg(ast, AST_IO_SEQ_PORT, 0x00, 0x03);
	for (i = 0; i < 4; i++) {
		jreg = stdtable->seq[i];
		if (!i)
			jreg |= 0x20;
		ast_set_index_reg(ast, AST_IO_SEQ_PORT, (i + 1), jreg);
	}

	/* Set CRTC */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x11, 0x7f, 0x00);
	for (i = 0; i < 25; i++)
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, i, stdtable->crtc[i]);

	/* set AR */
	jreg = ast_io_read8(ast, AST_IO_INPUT_STATUS1_READ);
	for (i = 0; i < 20; i++) {
		jreg = stdtable->ar[i];
		ast_io_write8(ast, AST_IO_AR_PORT_WRITE, (u8)i);
		ast_io_write8(ast, AST_IO_AR_PORT_WRITE, jreg);
	}
	ast_io_write8(ast, AST_IO_AR_PORT_WRITE, 0x14);
	ast_io_write8(ast, AST_IO_AR_PORT_WRITE, 0x00);

	jreg = ast_io_read8(ast, AST_IO_INPUT_STATUS1_READ);
	ast_io_write8(ast, AST_IO_AR_PORT_WRITE, 0x20);

	/* Set GR */
	for (i = 0; i < 9; i++)
		ast_set_index_reg(ast, AST_IO_GR_PORT, i, stdtable->gr[i]);
}

static void ast_set_crtc_reg(struct drm_crtc *crtc, struct drm_display_mode *mode,
			     struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = crtc->dev->dev_private;
	u8 jreg05 = 0, jreg07 = 0, jreg09 = 0, jregAC = 0, jregAD = 0, jregAE = 0;
	u16 temp, precache = 0;

	if ((ast->chip == AST2500) &&
	    (vbios_mode->enh_table->flags & AST2500PreCatchCRT))
		precache = 40;

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x11, 0x7f, 0x00);

	temp = (mode->crtc_htotal >> 3) - 5;
	if (temp & 0x100)
		jregAC |= 0x01; /* HT D[8] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x00, 0x00, temp);

	temp = (mode->crtc_hdisplay >> 3) - 1;
	if (temp & 0x100)
		jregAC |= 0x04; /* HDE D[8] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x01, 0x00, temp);

	temp = (mode->crtc_hblank_start >> 3) - 1;
	if (temp & 0x100)
		jregAC |= 0x10; /* HBS D[8] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x02, 0x00, temp);

	temp = ((mode->crtc_hblank_end >> 3) - 1) & 0x7f;
	if (temp & 0x20)
		jreg05 |= 0x80;  /* HBE D[5] */
	if (temp & 0x40)
		jregAD |= 0x01;  /* HBE D[5] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x03, 0xE0, (temp & 0x1f));

	temp = ((mode->crtc_hsync_start-precache) >> 3) - 1;
	if (temp & 0x100)
		jregAC |= 0x40; /* HRS D[5] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x04, 0x00, temp);

	temp = (((mode->crtc_hsync_end-precache) >> 3) - 1) & 0x3f;
	if (temp & 0x20)
		jregAD |= 0x04; /* HRE D[5] */
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x05, 0x60, (u8)((temp & 0x1f) | jreg05));

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xAC, 0x00, jregAC);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xAD, 0x00, jregAD);

	/* vert timings */
	temp = (mode->crtc_vtotal) - 2;
	if (temp & 0x100)
		jreg07 |= 0x01;
	if (temp & 0x200)
		jreg07 |= 0x20;
	if (temp & 0x400)
		jregAE |= 0x01;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x06, 0x00, temp);

	temp = (mode->crtc_vsync_start) - 1;
	if (temp & 0x100)
		jreg07 |= 0x04;
	if (temp & 0x200)
		jreg07 |= 0x80;
	if (temp & 0x400)
		jregAE |= 0x08;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x10, 0x00, temp);

	temp = (mode->crtc_vsync_end - 1) & 0x3f;
	if (temp & 0x10)
		jregAE |= 0x20;
	if (temp & 0x20)
		jregAE |= 0x40;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x11, 0x70, temp & 0xf);

	temp = mode->crtc_vdisplay - 1;
	if (temp & 0x100)
		jreg07 |= 0x02;
	if (temp & 0x200)
		jreg07 |= 0x40;
	if (temp & 0x400)
		jregAE |= 0x02;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x12, 0x00, temp);

	temp = mode->crtc_vblank_start - 1;
	if (temp & 0x100)
		jreg07 |= 0x08;
	if (temp & 0x200)
		jreg09 |= 0x20;
	if (temp & 0x400)
		jregAE |= 0x04;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x15, 0x00, temp);

	temp = mode->crtc_vblank_end - 1;
	if (temp & 0x100)
		jregAE |= 0x10;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x16, 0x00, temp);

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x07, 0x00, jreg07);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x09, 0xdf, jreg09);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xAE, 0x00, (jregAE | 0x80));

	if (precache)
		ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb6, 0x3f, 0x80);
	else
		ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb6, 0x3f, 0x00);

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0x11, 0x7f, 0x80);
}

void ast_set_offset_reg(struct drm_crtc *crtc)
{
	struct ast_private *ast = crtc->dev->dev_private;
	const struct drm_framebuffer *fb = crtc->primary->fb;

	u16 offset;

	offset = fb->pitches[0] >> 3;
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x13, (offset & 0xff));
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xb0, (offset >> 8) & 0x3f);
}

static void ast_set_dclk_reg(struct drm_device *dev, struct drm_display_mode *mode,
			     struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = dev->dev_private;
	const struct ast_vbios_dclk_info *clk_info;

	if (ast->chip == AST2500)
		clk_info = &dclk_table_ast2500[vbios_mode->enh_table->dclk_index];
	else
		clk_info = &dclk_table[vbios_mode->enh_table->dclk_index];

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xc0, 0x00, clk_info->param1);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xc1, 0x00, clk_info->param2);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xbb, 0x0f,
			       (clk_info->param3 & 0xc0) |
			       ((clk_info->param3 & 0x3) << 4));
}

static void ast_set_ext_reg(struct drm_crtc *crtc, struct drm_display_mode *mode,
			     struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = crtc->dev->dev_private;
	const struct drm_framebuffer *fb = crtc->primary->fb;
	u8 jregA0 = 0, jregA3 = 0, jregA8 = 0;

	switch (fb->format->cpp[0] * 8) {
	case 8:
		jregA0 = 0x70;
		jregA3 = 0x01;
		jregA8 = 0x00;
		break;
	case 15:
	case 16:
		jregA0 = 0x70;
		jregA3 = 0x04;
		jregA8 = 0x02;
		break;
	case 24:
	case 32:
		jregA0 = 0x70;
		jregA3 = 0x08;
		jregA8 = 0x02;
		break;
	}

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xa0, 0x8f, jregA0);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xa3, 0xf0, jregA3);
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xa8, 0xfd, jregA8);

	/* Set Threshold */
	if (ast->chip == AST2300 || ast->chip == AST2400 ||
	    ast->chip == AST2500) {
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa7, 0x78);
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa6, 0x60);
	} else if (ast->chip == AST2100 ||
		   ast->chip == AST1100 ||
		   ast->chip == AST2200 ||
		   ast->chip == AST2150) {
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa7, 0x3f);
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa6, 0x2f);
	} else {
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa7, 0x2f);
		ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xa6, 0x1f);
	}
}

static void ast_set_sync_reg(struct drm_device *dev, struct drm_display_mode *mode,
		      struct ast_vbios_mode_info *vbios_mode)
{
	struct ast_private *ast = dev->dev_private;
	u8 jreg;

	jreg  = ast_io_read8(ast, AST_IO_MISC_PORT_READ);
	jreg &= ~0xC0;
	if (vbios_mode->enh_table->flags & NVSync)
		jreg |= 0x80;
	if (vbios_mode->enh_table->flags & NHSync)
		jreg |= 0x40;
	ast_io_write8(ast, AST_IO_MISC_PORT_WRITE, jreg);
}

void ast_set_start_address_crt1(struct ast_private *ast, u32 offset)
{
	u32 addr;

	addr = offset >> 2;
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x0d, (u8)(addr & 0xff));
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0x0c, (u8)((addr >> 8) & 0xff));
	ast_set_index_reg(ast, AST_IO_CRTC_PORT, 0xaf, (u8)((addr >> 16) & 0xff));
}

void ast_hide_cursor(struct drm_crtc *crtc)
{
	struct ast_private *ast = crtc->dev->dev_private;
	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xcb, 0xfc, 0x00);
}

int ast_crtc_mode_set(struct drm_crtc *crtc,
		      struct drm_display_mode *mode,
		      struct drm_display_mode *adjusted_mode)
{
	struct drm_device *dev = crtc->dev;
	struct ast_private *ast = crtc->dev->dev_private;
	struct ast_vbios_mode_info vbios_mode;
	bool ret;
	int err;

	if (ast->chip == AST1180) {
		dev_err(dev->pdev, "AST 1180 modesetting not supported\n");
		return -EINVAL;
	}

	/* DPMS, set on */
	ast_set_index_reg_mask(ast, AST_IO_SEQ_PORT, 0x1, 0xdf, 0);
	if (ast->tx_chip_type == AST_TX_DP501)
		ast_set_dp501_video_output(crtc->dev, 1);
	ast_crtc_load_lut(crtc);

	/* Get mode */
	ret = ast_get_vbios_mode_info(crtc, mode, adjusted_mode, &vbios_mode);
	if (ret == false) {
		dev_err(dev->pdev, "Failed to find compatible vbios mode\n");
		return -EINVAL;
	}
	ast_open_key(ast);

	ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xa1, 0xff, 0x04);

	ast_set_std_reg(crtc, adjusted_mode, &vbios_mode);
	ast_set_crtc_reg(crtc, adjusted_mode, &vbios_mode);
	ast_set_offset_reg(crtc);
	ast_set_dclk_reg(dev, adjusted_mode, &vbios_mode);
	ast_set_ext_reg(crtc, adjusted_mode, &vbios_mode);
	ast_set_sync_reg(dev, adjusted_mode, &vbios_mode);

	err = ast_crtc_do_set_base(crtc);
	if (err)
		return err;

	/* Commit changes */

	ast_set_index_reg_mask(ast, AST_IO_SEQ_PORT, 0x1, 0xdf, 0);
	ast_crtc_load_lut(crtc);

	return 0;
}

enum drm_mode_status ast_mode_valid(struct drm_connector *connector,
				    const unsigned int hdisplay, const unsigned int vdisplay)
{
	struct ast_private *ast = connector->dev->dev_private;
	int flags = MODE_NOMODE;
	uint32_t jtemp;

	if (ast->support_wide_screen) {
		if ((hdisplay == 1680) && (vdisplay == 1050))
			return MODE_OK;
		if ((hdisplay == 1280) && (vdisplay == 800))
			return MODE_OK;
		if ((hdisplay == 1440) && (vdisplay == 900))
			return MODE_OK;
		if ((hdisplay == 1360) && (vdisplay == 768))
			return MODE_OK;
		if ((hdisplay == 1600) && (vdisplay == 900))
			return MODE_OK;

		if ((ast->chip == AST2100) || (ast->chip == AST2200) ||
		    (ast->chip == AST2300) || (ast->chip == AST2400) ||
		    (ast->chip == AST2500) || (ast->chip == AST1180)) {
			if ((hdisplay == 1920) && (vdisplay == 1080))
				return MODE_OK;

			if ((hdisplay == 1920) && (vdisplay == 1200)) {
				jtemp = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xd1,
							       0xff);
				if (jtemp & 0x01)
					return MODE_NOMODE;
				else
					return MODE_OK;
			}
		}
	}
	switch (hdisplay) {
	case 640:
		if (vdisplay == 480)
			flags = MODE_OK;
		break;
	case 800:
		if (vdisplay == 600)
			flags = MODE_OK;
		break;
	case 1024:
		if (vdisplay == 768)
			flags = MODE_OK;
		break;
	case 1280:
		if (vdisplay == 1024)
			flags = MODE_OK;
		break;
	case 1600:
		if (vdisplay == 1200)
			flags = MODE_OK;
		break;
	default:
		return flags;
	}

	return flags;
}
