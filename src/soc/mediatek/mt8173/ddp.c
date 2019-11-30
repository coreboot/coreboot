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

#include <device/mmio.h>
#include <edid.h>
#include <stddef.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>
#include <types.h>

static void disp_config_main_path_connection(void)
{
	write32(&mmsys_cfg->disp_ovl0_mout_en, OVL0_MOUT_EN_COLOR0);
	write32(&mmsys_cfg->disp_color0_sel_in, COLOR0_SEL_IN_OVL0);
	write32(&mmsys_cfg->disp_od_mout_en, OD_MOUT_EN_RDMA0);
	write32(&mmsys_cfg->disp_ufoe_mout_en, UFOE_MOUT_EN_DSI0);
	write32(&mmsys_cfg->dsi0_sel_in, DSI0_SEL_IN_UFOE);
}

static void disp_config_main_path_mutex(void)
{
	write32(&disp_mutex->mutex[0].mod, MUTEX_MOD_MAIN_PATH);

	/* Clock source from DSI0 */
	write32(&disp_mutex->mutex[0].sof, BIT(0));
	write32(&disp_mutex->mutex[0].en, BIT(0));
}

static void od_start(u32 width, u32 height)
{
	write32(&disp_od->size, width << 16 | height);
	write32(&disp_od->cfg, OD_RELAY_MODE);

	write32(&disp_od->en, 1);
}

static void main_disp_path_setup(u32 width, u32 height, u32 pixel_clk)
{
	ovl_set_roi(0, width, height, 0);
	rdma_config(width, height, pixel_clk, 8 * KiB);
	od_start(width, height);
	write32(&disp_ufoe->start, UFO_BYPASS);
	color_start(width, height);
	disp_config_main_path_connection();
	disp_config_main_path_mutex();
}

static void disp_clock_on(void)
{
	clrbits32(&mmsys_cfg->mmsys_cg_con0, CG_CON0_SMI_COMMON |
					     CG_CON0_SMI_LARB0 |
					     CG_CON0_MUTEX_32K |
					     CG_CON0_DISP_OVL0 |
					     CG_CON0_DISP_RDMA0 |
					     CG_CON0_DISP_COLOR0 |
					     CG_CON0_DISP_UFOE |
					     CG_CON0_DISP_OD);

	clrbits32(&mmsys_cfg->mmsys_cg_con1, CG_CON1_DSI0_ENGINE |
					     CG_CON1_DSI0_DIGITAL);
}

void mtk_ddp_init(void)
{
	disp_clock_on();
}

void mtk_ddp_mode_set(const struct edid *edid)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;

	main_disp_path_setup(edid->mode.ha, edid->mode.va,
			     edid->mode.pixel_clock);

	rdma_start();
	ovl_layer_config(fmt, bpp, edid->mode.ha, edid->mode.va);
}
