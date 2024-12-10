/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _SOC_MEDIATEK_MT8196_DDP_H_
#define _SOC_MEDIATEK_MT8196_DDP_H_

#include <soc/addressmap.h>
#include <soc/display.h>
#include <types.h>

#define CG_CON_ALL 0xFFFFFFFF

struct ovlsys_cfg {
	u32 reserved_0x000[64];
	u32 mmsys_cg_con0;
	u32 mmsys_cg_set0;
	u32 mmsys_cg_clr0;
	u32 reserved_0x10c;
	u32 mmsys_cg_con1;
	u32 mmsys_cg_set1;
	u32 mmsys_cg_clr1;
	u32 reserved_0x11c;
	u32 mmsys_cg_con2;
	u32 mmsys_cg_set2;
	u32 mmsys_cg_clr2;
	u32 reserved_0x12c[92];
	u32 relay5_size;
	u32 reserved_0x2a0[640];
	u32 bypass_mux_shadow;
	u32 reserved_0xca4;
	u32 reserved_0xca8;
	u32 cb_con;
	u32 reserved_0xcb0[88];
	u32 blender_out_cb4;
	u32 reserved_0xe14;
	u32 reserved_0xe18;
	u32 reserved_0xe1c[17];
	u32 reserved_0xe60;
	u32 reserved_0xe64;
	u32 exdma_out_cb3;
	u32 reserved_0xe6c[41];
	u32 outproc_out_cb0;
	u32 reserved_0xf14;
	u32 reserved_0xf18[22];
	u32 rsz_in_cb2;
};

check_member(ovlsys_cfg, mmsys_cg_con0, 0x100);
check_member(ovlsys_cfg, mmsys_cg_con1, 0x110);
check_member(ovlsys_cfg, mmsys_cg_con2, 0x120);
check_member(ovlsys_cfg, relay5_size, 0x29C);
check_member(ovlsys_cfg, bypass_mux_shadow, 0xCA0);
check_member(ovlsys_cfg, cb_con, 0xCAC);
check_member(ovlsys_cfg, blender_out_cb4, 0xE10);
check_member(ovlsys_cfg, exdma_out_cb3, 0xE68);
check_member(ovlsys_cfg, outproc_out_cb0, 0xF10);
check_member(ovlsys_cfg, rsz_in_cb2, 0xF70);

static struct ovlsys_cfg *const ovlsys_cfg = (void *)OVLSYS_CONFIG_BASE;

struct dispsys_cfg {
	u32 reserved_0x000[64];
	u32 mmsys_cg_con0;
	u32 mmsys_cg_set0;
	u32 mmsys_cg_clr0;
	u32 reserved_0x10c;
	u32 mmsys_cg_con1;
	u32 mmsys_cg_set1;
	u32 mmsys_cg_clr1;
	u32 reserved_0x11c;
	u32 mmsys_cg_con2;
	u32 mmsys_cg_set2;
	u32 mmsys_cg_clr2;
	u32 reserved_0x12c[53];
	u32 dl_in_relay0;
	u32 reserved_0x204[25];
	u32 dl_out_relay1;
	u32 reserved_0x26c[625];
	u32 bypass_mux_shadow;
	u32 reserved_0xc34[61];
	u32 disp_ccorr0_sel;
	u32 disp_ccorr0_sout;
	u32 disp_ccorr1_sel;
	u32 disp_ccorr1_sout;
	u32 reserved_0xd38[8];
	u32 disp_gamma0_sel;
	u32 reserved_0xd5c[3];
	u32 disp_postmask_sout;
	u32 reserved_0xd6c;
	u32 disp_tdshp0_sout;
	u32 reserved_0xd74;
	u32 disp_mdp_rsz0_mout;
	u32 reserved_0xd7c[2];
	u32 panel_comp_out_cb1;
	u32 reserved_0xd88[18];
	u32 pq_in_cb0;
	u32 reserved_0xdd4[26];
	u32 pq_out_cb0;
	u32 pq_out_cb1;
	u32 pq_out_cb2;
	u32 reserved_0xe40[3];
	u32 pq_out_cb6;
};

check_member(dispsys_cfg, mmsys_cg_con0, 0x100);
check_member(dispsys_cfg, mmsys_cg_con1, 0x110);
check_member(dispsys_cfg, mmsys_cg_con2, 0x120);
check_member(dispsys_cfg, dl_in_relay0, 0x200);
check_member(dispsys_cfg, dl_out_relay1, 0x268);
check_member(dispsys_cfg, bypass_mux_shadow, 0xC30);
check_member(dispsys_cfg, disp_ccorr0_sel, 0xD28);
check_member(dispsys_cfg, disp_gamma0_sel, 0xD58);
check_member(dispsys_cfg, disp_postmask_sout, 0xD68);
check_member(dispsys_cfg, disp_tdshp0_sout, 0xD70);
check_member(dispsys_cfg, panel_comp_out_cb1, 0xD84);
check_member(dispsys_cfg, pq_in_cb0, 0xDD0);
check_member(dispsys_cfg, pq_out_cb0, 0xE3C);
check_member(dispsys_cfg, pq_out_cb6, 0xE54);

static struct dispsys_cfg *const mmsys_cfg = (void *)MMSYS_CONFIG_BASE;

struct dispsys1_cfg {
	u32 reserved_0x000[64];
	u32 mmsys_cg_con0;
	u32 mmsys_cg_set0;
	u32 mmsys_cg_clr0;
	u32 reserved_0x10c;
	u32 mmsys_cg_con1;
	u32 mmsys_cg_set1;
	u32 mmsys_cg_clr1;
	u32 reserved_0x11c;
	u32 mmsys_cg_con2;
	u32 mmsys_cg_set2;
	u32 mmsys_cg_clr2;
	u32 reserved_0x12c[54];
	u32 dl_in_relay21;
	u32 reserved_0x208[700];
	u32 bypass_mux_shadow;
	u32 reserved_0xcfc[13];
	u32 comp_out_cb6;
	u32 reserved_0xd34[38];
	u32 merge_out_cb0;
	u32 reserved_0xdd0[55];
	u32 splitter_in_cb1;
	u32 reserved_0xeb0[45];
	u32 splitter_out_cb9;
};

check_member(dispsys1_cfg, mmsys_cg_con0, 0x100);
check_member(dispsys1_cfg, mmsys_cg_con1, 0x110);
check_member(dispsys1_cfg, mmsys_cg_con2, 0x120);
check_member(dispsys1_cfg, dl_in_relay21, 0x204);
check_member(dispsys1_cfg, bypass_mux_shadow, 0xCF8);
check_member(dispsys1_cfg, comp_out_cb6, 0xD30);
check_member(dispsys1_cfg, merge_out_cb0, 0xDCC);
check_member(dispsys1_cfg, splitter_in_cb1, 0xEAC);
check_member(dispsys1_cfg, splitter_out_cb9, 0xF64);

static struct dispsys1_cfg *const mmsys1_cfg = (void *)MMSYS1_CONFIG_BASE;

struct exdma {
	u32 reserved_0x000[5];
	u32 datapath_con;
	u32 reserved_0x018[2];
	u32 ovl_en;
	u32 reserved_0x024[3];
	u32 roi_size;
	u32 reserved_0x034[3];
	u32 ovl_l_en;
	u32 reserved_0x044;
	u32 ovl_l_size;
	u32 reserved_0x04c;
	u32 ovl_l_clrfmt;
	u32 reserved_0x054[43];
	u32 rdma0_ctl;
	u32 reserved_0x104[60];
	u32 rdma_burst_ctl;
	u32 reserved_0x1F8[2];
	u32 dummy;
	u32 reserved_0x204;
	u32 gdrdy_period;
	u32 reserved_0x20c[57];
	u32 pitch_msb;
	u32 pitch;
	u32 reserved_0x2f8[2];
	u32 ovl_con;
	u32 reserved_0x304[783];
	u32 ovl_addr;
	u32 reserved_0xf44[43];
	u32 ovl_mout;
};

check_member(exdma, datapath_con, 0x014);
check_member(exdma, ovl_en, 0x020);
check_member(exdma, roi_size, 0x030);
check_member(exdma, ovl_l_en, 0x040);
check_member(exdma, ovl_l_size, 0x048);
check_member(exdma, ovl_l_clrfmt, 0x050);
check_member(exdma, rdma0_ctl, 0x100);
check_member(exdma, rdma_burst_ctl, 0x1F4);
check_member(exdma, dummy, 0x200);
check_member(exdma, gdrdy_period, 0x208);
check_member(exdma, pitch_msb, 0x2F0);
check_member(exdma, pitch, 0x2F4);
check_member(exdma, ovl_addr, 0xF40);
check_member(exdma, ovl_mout, 0xFF0);

static struct exdma *const exdma2_reg = (void *)OVLSYS_EXDMA2_BASE;

struct blender {
	u32 reserved_0x000[4];
	u32 datapath_con;
	u32 reserved_0x014[3];
	u32 bld_en;
	u32 reserved_0x024;
	u32 shadow_ctl;
	u32 reserved_0x02c;
	u32 roi_size;
	u32 reserved_0x034[3];
	u32 bld_l_en;
	u32 reserved_0x044;
	u32 bld_l_size;
	u32 reserved_0x04c;
	u32 bld_l_fmt;
	u32 reserved_0x054[44];
	u32 bg_clr;
	u32 reserved_0x108[62];
	u32 bld_l_con2;
};

check_member(blender, datapath_con, 0x010);
check_member(blender, bld_en, 0x020);
check_member(blender, shadow_ctl, 0x028);
check_member(blender, roi_size, 0x030);
check_member(blender, bld_l_en, 0x040);
check_member(blender, bld_l_size, 0x048);
check_member(blender, bld_l_fmt, 0x050);
check_member(blender, bg_clr, 0x104);
check_member(blender, bld_l_con2, 0x200);

static struct blender *const blenders[] = {
	(void *)OVLSYS_BLENDER1_BASE,
	(void *)OVLSYS_BLENDER2_BASE,
	(void *)OVLSYS_BLENDER3_BASE,
	(void *)OVLSYS_BLENDER4_BASE,
};

struct outproc {
	u32 reserved_0x000[4];
	u32 datapath_con;
	u32 reserved_0x014[3];
	u32 outproc_en;
	u32 reserved_0x024;
	u32 shadow_ctl;
	u32 reserved_0x02c;
	u32 roi_size;
};

check_member(outproc, datapath_con, 0x010);
check_member(outproc, outproc_en, 0x020);
check_member(outproc, shadow_ctl, 0x028);
check_member(outproc, roi_size, 0x030);

static struct outproc *const outproc0_reg = (void *)OVLSYS_OUTPROC0_BASE;

struct disp_mutex {
	u32 inten;
	u32 intsta;
	u32 reserved0[6];
	struct {
		u32 en;
		u32 dummy;
		u32 dummy1;
		u32 rst;
		u32 ctl;
		u32 mod;
		u32 mod1;
		u32 reserved;
	} mutex[12];
};

enum mmsys {
	DISP0,
	DISP1,
	OVL0,
};

static struct disp_mutex *const mmsys_mutex[] = {
	[DISP0] = (void *)MMSYS_MUTEX_BASE,
	[DISP1] = (void *)MMSYS1_MUTEX_BASE,
	[OVL0] = (void *)OVLSYS_MUTEX_BASE,
};

enum {
	OVL_INFMT_RGB565 = 0,
	OVL_INFMT_RGB888 = 1,
	OVL_INFMT_RGBA8888 = 2,
	OVL_INFMT_ARGB8888 = 3,
	OVL_INFMT_UYVY = 4,
	OVL_INFMT_YUYV = 5,
	OVL_INFMT_UNKNOWN = 16,
};

/* OVLSYS mutex module */
enum {
	MUTEX_MOD_OVL_EXDMA2		= BIT(2),
	MUTEX_MOD_OVL_BLENDER1		= BIT(11),
	MUTEX_MOD_OVL_BLENDER2		= BIT(12),
	MUTEX_MOD_OVL_BLENDER3		= BIT(13),
	MUTEX_MOD_OVL_BLENDER4		= BIT(14),
	MUTEX_MOD_OVL_OUTPROC0		= BIT(20),
	MUTEX_MOD_OVL_MAIN_PATH		= MUTEX_MOD_OVL_EXDMA2 |
					  MUTEX_MOD_OVL_BLENDER1 |
					  MUTEX_MOD_OVL_BLENDER2 |
					  MUTEX_MOD_OVL_BLENDER3 |
					  MUTEX_MOD_OVL_BLENDER4 |
					  MUTEX_MOD_OVL_OUTPROC0,
};

enum {
	MUTEX_MOD1_OVL_DLO_ASYNC5	= BIT(16),
	MUTEX_MOD1_OVL_MAIN_PATH	= MUTEX_MOD1_OVL_DLO_ASYNC5,
};

/* DISPSYS mutex module */
enum {
	MUTEX_MOD_DISP_CCORR0		= BIT(6),
	MUTEX_MOD_DISP_CCORR1		= BIT(7),
	MUTEX_MOD_DISP_DITHER0		= BIT(14),
	MUTEX_MOD_DISP_DLI_ASYNC0	= BIT(16),
	MUTEX_MOD_DISP_MAIN_PATH	= MUTEX_MOD_DISP_DLI_ASYNC0 |
					  MUTEX_MOD_DISP_CCORR0 |
					  MUTEX_MOD_DISP_CCORR1 |
					  MUTEX_MOD_DISP_DITHER0,

};

enum {
	MUTEX_MOD1_DISP_DLO_ASYNC1	= BIT(1),
	MUTEX_MOD1_DISP_GAMMA0		= BIT(9),
	MUTEX_MOD1_DISP_POSTMASK0	= BIT(14),
	MUTEX_MOD1_DISP_MDP_RSZ0	= BIT(18),
	MUTEX_MOD1_DISP_TDSHP0		= BIT(21),
	MUTEX_MOD1_DISP_MAIN_PATH	= MUTEX_MOD1_DISP_MDP_RSZ0 |
					  MUTEX_MOD1_DISP_TDSHP0 |
					  MUTEX_MOD1_DISP_GAMMA0 |
					  MUTEX_MOD1_DISP_POSTMASK0 |
					  MUTEX_MOD1_DISP_DLO_ASYNC1,
};

/* DISPSYS1 mutex module */
enum {
	MUTEX_MOD_DISP1_DLI_ASYNC21	= BIT(1),
	MUTEX_MOD_DISP1_DSI0		= BIT(23),
	MUTEX_MOD_DISP1_DVO		= BIT(29),
	MUTEX_MOD_DISP1_MAIN_PATH_EDP	= MUTEX_MOD_DISP1_DLI_ASYNC21 |
					  MUTEX_MOD_DISP1_DVO,
	MUTEX_MOD_DISP1_MAIN_PATH_DSI0	= MUTEX_MOD_DISP1_DLI_ASYNC21 |
					  MUTEX_MOD_DISP1_DSI0,
};

enum {
	MUTEX_SOF_SINGLE_MODE	= 0,
	MUTEX_SOF_DSI0		= 1,
	MUTEX_SOF_DPI0		= 5,
	MUTEX_SOF_DVO		= 7,
};

enum mtk_disp_blender_layer {
	FIRST_BLENDER,
	LAST_BLENDER,
	SINGLE_BLENDER,
	OTHER_BLENDER,
};

struct disp_tdshp_regs {
	u32 tdshp_00;
	u32 reserved0[63];
	u32 ctrl;
	u32 reserved1[3];
	u32 cfg;
	u32 reserved2[3];
	u32 input_size;
	u32 output_offset;
	u32 output_size;
};

check_member(disp_tdshp_regs, output_size, 0x128);

struct disp_ccorr_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 intsta;
	u32 status;
	u32 reserved0[3];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
	u32 reserved2[27];
	u32 shadow;
};

check_member(disp_ccorr_regs, shadow, 0xA0);

struct disp_gamma_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 intsta;
	u32 status;
	u32 reserved0[3];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
};

check_member(disp_gamma_regs, size, 0x30);

struct disp_dither_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 intsta;
	u32 status;
	u32 reserved0[3];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
	u32 reserved2[51];
	u32 shadow;
};

check_member(disp_dither_regs, shadow, 0x100);

struct disp_mdp_rsz_regs {
	u32 en;
	u32 reserved0[3];
	u32 input_size;
	u32 output_size;
};

check_member(disp_mdp_rsz_regs, output_size, 0x14);

struct disp_postmask_regs {
	u32 en;
	u32 reserved0[7];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
};

check_member(disp_postmask_regs, size, 0x30);

static struct disp_mdp_rsz_regs *const disp_mdp_rsz0_reg	= (void *)DISP_MDP_RSZ0_BASE;
static struct disp_postmask_regs *const disp_postmask0_reg	= (void *)DISP_POSTMASK0_BASE;
static struct disp_tdshp_regs *const disp_tdshp0_reg		= (void *)DISP_TDSHP0_BASE;
static struct disp_ccorr_regs *const disp_ccorr0_reg		= (void *)DISP_CCORR0_BASE;
static struct disp_ccorr_regs *const disp_ccorr1_reg		= (void *)DISP_CCORR1_BASE;
static struct disp_gamma_regs *const disp_gamma0_reg		= (void *)DISP_GAMMA0_BASE;
static struct disp_dither_regs *const disp_dither0_reg		= (void *)DISP_DITHER0_BASE;

#endif
