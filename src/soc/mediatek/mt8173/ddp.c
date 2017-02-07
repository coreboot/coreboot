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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>

#define RDMA_FIFO_PSEUDO_SIZE(bytes)            (((bytes) / 16) << 16)
#define RDMA_OUTPUT_VALID_FIFO_THRESHOLD(bytes) ((bytes) / 16)

static void disp_config_main_path_connection(bool dual_dsi_mode)
{
	write32(&mmsys_cfg->disp_ovl0_mout_en, OVL0_MOUT_EN_COLOR0);

	write32(&mmsys_cfg->disp_color0_sel_in, COLOR0_SEL_IN_OVL0);

	write32(&mmsys_cfg->disp_od_mout_en, OD_MOUT_EN_RDMA0);

	if (dual_dsi_mode) {
		write32(&mmsys_cfg->disp_ufoe_mout_en, UFOE_MOUT_EN_SPLIT1);
		write32(&mmsys_cfg->dsi0_sel_in, DSI0_SEL_IN_SPLIT1);
		write32(&mmsys_cfg->dsi1_sel_in, DSI1_SEL_IN_SPLIT1);
	} else {
		write32(&mmsys_cfg->disp_ufoe_mout_en, UFOE_MOUT_EN_DSI0);
		write32(&mmsys_cfg->dsi0_sel_in, DSI0_SEL_IN_UFOE);
	}
}

static void disp_config_main_path_mutex(void)
{
	write32(&disp_mutex->mutex[0].mod, MUTEX_MOD_MAIN_PATH);

	/* Clock source from DSI0 */
	write32(&disp_mutex->mutex[0].sof, BIT(0));
	write32(&disp_mutex->mutex[0].en, BIT(0));
}

static void ovl_set_roi(u32 width, u32 height, u32 color)
{
	write32(&disp_ovl[0]->roi_size, height << 16 | width);
	write32(&disp_ovl[0]->roi_bgclr, color);
}

static void ovl_layer_enable(void)
{
	write32(&disp_ovl[0]->rdma[0].ctrl, BIT(0));
	write32(&disp_ovl[0]->rdma[0].mem_gmc_setting, RDMA_MEM_GMC);

	setbits_le32(&disp_ovl[0]->src_con, BIT(0));
}

static void rdma_start(void)
{
	setbits_le32(&disp_rdma[0]->global_con, RDMA_ENGINE_EN);
}

static void rdma_config(u32 width, u32 height, u32 pixel_clk)
{
	u32 threshold;
	u32 reg;

	/* Config width */
	clrsetbits_le32(&disp_rdma[0]->size_con_0, 0x1FFF, width);

	/* Config height */
	clrsetbits_le32(&disp_rdma[0]->size_con_1, 0xFFFFF, height);

	/*
	 * Enable FIFO underflow since DSI and DPI can't be blocked. Keep the
	 * FIFO pseudo size reset default of 8 KiB. Set the output threshold to
	 * 6 microseconds with 7/6 overhead to account for blanking, and with a
	 * pixel depth of 4 bytes:
	 */

	threshold = pixel_clk * 4 * 7 / 1000;

	reg = RDMA_FIFO_UNDERFLOW_EN |
	      RDMA_FIFO_PSEUDO_SIZE(8 * KiB) |
	      RDMA_OUTPUT_VALID_FIFO_THRESHOLD(threshold);

	write32(&disp_rdma[0]->fifo_con, reg);
}

static void od_start(u32 width, u32 height)
{
	write32(&disp_od->size, width << 16 | height);
	write32(&disp_od->cfg, OD_RELAY_MODE);

	write32(&disp_od->en, 1);
}

static void ufoe_start(u32 width, u32 height, bool dual_dsi_mode)
{
	if (dual_dsi_mode) {
		write32(&disp_ufoe->frame_width, width);
		write32(&disp_ufoe->frame_height, height);
		write32(&disp_ufoe->start, UFO_LR);
	} else {
		write32(&disp_ufoe->start, UFO_BYPASS);
	}
}

static void color_start(u32 width, u32 height)
{
	write32(&disp_color[0]->width, width);
	write32(&disp_color[0]->height, height);
	write32(&disp_color[0]->cfg_main, COLOR_BYPASS_ALL | COLOR_SEQ_SEL);
	write32(&disp_color[0]->start, BIT(0));
}

static void split_start(void)
{
	write32(&disp_split->start, 1);
}

static void ovl_layer_config(u32 fmt, u32 bpp, u32 width, u32 height)
{
	write32(&disp_ovl[0]->layer[0].con, fmt << 12);
	write32(&disp_ovl[0]->layer[0].src_size, height << 16 | width);
	write32(&disp_ovl[0]->layer[0].pitch, (width * bpp) & 0xFFFF);

	ovl_layer_enable();
}

static void main_disp_path_setup(u32 width, u32 height, u32 pixel_clk,
				 bool dual_dsi_mode)
{
	/* Setup OVL */
	ovl_set_roi(width, height, 0);

	/* Setup RDMA0 */
	rdma_config(width, height, pixel_clk);

	/* Setup OD */
	od_start(width, height);

	/* Setup UFOE */
	ufoe_start(width, height, dual_dsi_mode);

	if (dual_dsi_mode)
		split_start();

	/* Setup Color */
	color_start(width, height);

	/* Setup main path connection */
	disp_config_main_path_connection(dual_dsi_mode);

	/* Setup main path mutex */
	disp_config_main_path_mutex();
}

static void disp_clock_on(bool dual_dsi_mode)
{
	u32 dual_dsi_cg_con0;
	u32 dual_dsi_cg_con1;

	if (dual_dsi_mode) {
		dual_dsi_cg_con0 = CG_CON0_DISP_SPLIT1;
		dual_dsi_cg_con1 = CG_CON1_DSI1_ENGINE | CG_CON1_DSI1_DIGITAL;
	} else {
		dual_dsi_cg_con0 = 0;
		dual_dsi_cg_con1 = 0;
	}

	clrbits_le32(&mmsys_cfg->mmsys_cg_con0, CG_CON0_SMI_COMMON |
						CG_CON0_SMI_LARB0 |
						CG_CON0_MUTEX_32K |
						CG_CON0_DISP_OVL0 |
						CG_CON0_DISP_RDMA0 |
						CG_CON0_DISP_COLOR0 |
						CG_CON0_DISP_UFOE |
						dual_dsi_cg_con0 |
						CG_CON0_DISP_OD);

	clrbits_le32(&mmsys_cfg->mmsys_cg_con1, CG_CON1_DSI0_ENGINE |
						CG_CON1_DSI0_DIGITAL |
						dual_dsi_cg_con1);
}

void mtk_ddp_init(bool dual_dsi_mode)
{
	disp_clock_on(dual_dsi_mode);
}

void mtk_ddp_mode_set(const struct edid *edid, bool dual_dsi_mode)
{
	u32 fmt = OVL_INFMT_RGBA8888;
	u32 bpp = edid->framebuffer_bits_per_pixel / 8;

	main_disp_path_setup(edid->mode.ha, edid->mode.va,
			     edid->mode.pixel_clock, dual_dsi_mode);

	rdma_start();

	ovl_layer_config(fmt, bpp, edid->mode.ha, edid->mode.va);
}
