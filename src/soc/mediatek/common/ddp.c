/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 MediaTek Inc.
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
#include <stdlib.h>
#include <stddef.h>
#include <soc/addressmap.h>
#include <soc/ddp.h>

#define RDMA_FIFO_PSEUDO_SIZE(bytes)            (((bytes) / 16) << 16)
#define RDMA_OUTPUT_VALID_FIFO_THRESHOLD(bytes) ((bytes) / 16)

void ovl_set_roi(u32 idx, u32 width, u32 height, u32 color)
{
	write32(&disp_ovl[idx]->roi_size, height << 16 | width);
	write32(&disp_ovl[idx]->roi_bgclr, color);
}

void rdma_start(void)
{
	setbits32(&disp_rdma0->global_con, RDMA_ENGINE_EN);
}

void rdma_config(u32 width, u32 height, u32 pixel_clk, u32 fifo_size)
{
	u32 threshold;
	u32 reg;

	clrsetbits32(&disp_rdma0->size_con_0, 0x1FFF, width);
	clrsetbits32(&disp_rdma0->size_con_1, 0xFFFFF, height);

	/*
	 * Enable FIFO underflow since DSI and DPI can't be blocked. Set the
	 * output threshold to 6 microseconds with 7/6 overhead to account for
	 * blanking, and with a pixel depth of 4 bytes:
	 */
	threshold = pixel_clk * 4 * 7 / 1000;

	if (threshold > fifo_size)
		threshold = fifo_size;

	reg = RDMA_FIFO_UNDERFLOW_EN | RDMA_FIFO_PSEUDO_SIZE(fifo_size) |
	      RDMA_OUTPUT_VALID_FIFO_THRESHOLD(threshold);

	write32(&disp_rdma0->fifo_con, reg);
}

void color_start(u32 width, u32 height)
{

	write32(&disp_color0->width, width);
	write32(&disp_color0->height, height);
	write32(&disp_color0->cfg_main, COLOR_BYPASS_ALL | COLOR_SEQ_SEL);
	write32(&disp_color0->start, BIT(0));
}

void ovl_layer_config(u32 fmt, u32 bpp, u32 width, u32 height)
{
	struct disp_ovl_regs *const ovl0 = disp_ovl[0];
	write32(&ovl0->layer[0].con, fmt << 12);
	write32(&ovl0->layer[0].src_size, height << 16 | width);
	write32(&ovl0->layer[0].pitch, (width * bpp) & 0xFFFF);

	/* Enable layer */
	write32(&ovl0->rdma[0].ctrl, BIT(0));
	write32(&ovl0->rdma[0].mem_gmc_setting, RDMA_MEM_GMC);

	setbits32(&ovl0->src_con, BIT(0));
}
