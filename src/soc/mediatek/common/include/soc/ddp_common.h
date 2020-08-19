/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DDP_COMMON_H_
#define _DDP_COMMON_H_

#include <soc/addressmap.h>
#include <types.h>

struct disp_ovl_regs {
	u32 sta;
	u32 inten;
	u32 intsta;
	u32 en;
	u32 trig;
	u32 rst;
	u8 reserved0[8];
	u32 roi_size;
	u32 datapath_con;
	u32 roi_bgclr;
	u32 src_con;
	struct {
		u32 con;
		u32 srckey;
		u32 src_size;
		u32 offset;
		u32 reserved0;
		u32 pitch;
		u32 reserved1[2];
	} layer[4];
	u8 reserved8[16];
	struct {
		u32 ctrl;
		u32 mem_start_trig;
		u32 mem_gmc_setting;
		u32 mem_slow_con;
		u32 fifo_ctrl;
		u8 reserved[12];
	} rdma[4];
	u8 reserved12[148];
	u32 debug_mon_sel;
	u8 reserved13[8];
	u32 rdma_mem_gmc_setting2[4];
	u8 reserved14[16];
	u32 dummy;
	u8 reserved15[60];
	u32 flow_ctrl_dbg;
	u32 addcon_dbg;
	u32 outmux_dbg;
	u32 rdma_dbg[4];
	u8 reserved16[3300];
	u32 l0_addr;
	u8 reserved17[28];
	u32 l1_addr;
	u8 reserved18[28];
	u32 l2_addr;
	u8 reserved19[28];
	u32 l3_addr;
};

check_member(disp_ovl_regs, l3_addr, 0xFA0);
static struct disp_ovl_regs *const disp_ovl[2] = {
	(void *)DISP_OVL0_BASE, (void *)DISP_OVL1_BASE
};

struct disp_rdma_regs {
	u32 int_enable;
	u32 int_status;
	u8 reserved0[8];
	u32 global_con;
	u32 size_con_0;
	u32 size_con_1;
	u32 target_line;
	u8 reserved1[4];
	u32 mem_con;
	u32 mem_start_addr;
	u32 mem_src_pitch;
	u32 mem_gmc_setting_0;
	u32 mem_slow_con;
	u32 mem_gmc_setting_1;
	u8 reserved2[4];
	u32 fifo_con;
	u8 reserved3[16];
	u32 cf[3][3];
	u32 cf_pre_add[3];
	u32 cf_post_add[3];
	u32 dummy;
	u32 debug_out_sel;
};

enum {
	RDMA_ENGINE_EN		= BIT(0),
	RDMA_FIFO_UNDERFLOW_EN	= BIT(31),
	RDMA_MEM_GMC = 0x40402020,
};

check_member(disp_rdma_regs, debug_out_sel, 0x94);
static struct disp_rdma_regs *const disp_rdma0 = (void *)DISP_RDMA0_BASE;

struct disp_color_regs {
	u8 reserved0[1024];
	u32 cfg_main;
	u8 reserved1[2044];
	u32 start;
	u8 reserved2[76];
	u32 width;
	u32 height;
};

check_member(disp_color_regs, cfg_main, 0x400);
check_member(disp_color_regs, start, 0xC00);
check_member(disp_color_regs, width, 0xC50);
check_member(disp_color_regs, height, 0xC54);
static struct disp_color_regs *const disp_color0 = (void *)DISP_COLOR0_BASE;

enum {
	COLOR_BYPASS_ALL = BIT(7),
	COLOR_SEQ_SEL    = BIT(13),
};

enum OVL_INPUT_FORMAT {
	OVL_INFMT_RGB565 = 0,
	OVL_INFMT_RGB888 = 1,
	OVL_INFMT_RGBA8888 = 2,
	OVL_INFMT_ARGB8888 = 3,
	OVL_INFMT_UYVY = 4,
	OVL_INFMT_YUYV = 5,
	OVL_INFMT_UNKNOWN = 16,

	OVL_COLOR_BASE = 30,
	OVL_INFMT_BGR565 = OVL_INFMT_RGB565 + OVL_COLOR_BASE,
	OVL_INFMT_BGR888 = OVL_INFMT_RGB888 + OVL_COLOR_BASE,
	OVL_INFMT_BGRA8888 = OVL_INFMT_RGBA8888 + OVL_COLOR_BASE,
	OVL_INFMT_ABGR8888 = OVL_INFMT_ARGB8888 + OVL_COLOR_BASE,
};

void ovl_set_roi(u32 idx, u32 width, u32 height, u32 color);
void rdma_start(void);
void rdma_config(u32 width, u32 height, u32 pixel_clk, u32 fifo_size);
void color_start(u32 width, u32 height);
void ovl_layer_config(u32 fmt, u32 bpp, u32 width, u32 height);

#endif
