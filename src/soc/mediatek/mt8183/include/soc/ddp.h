/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _MT8183_SOC_DDP_H_
#define _MT8183_SOC_DDP_H_

#include <soc/addressmap.h>
#include <soc/ddp_common.h>
#include <types.h>

#define MAIN_PATH_OVL_NR 2

struct mmsys_cfg_regs {
	u32 reserved_0x000[64];		/* 0x000 */
	u32 mmsys_cg_con0;		/* 0x100 */
	u32 mmsys_cg_set0;		/* 0x104 */
	u32 mmsys_cg_clr0;		/* 0x108 */
	u32 reserved_0x10C;		/* 0x10C */
	u32 mmsys_cg_con1;		/* 0x110 */
	u32 mmsys_cg_set1;		/* 0x114 */
	u32 mmsys_cg_clr1;		/* 0x118 */
	u32 reserved_0x11C[889];	/* 0x11C */
	u32 disp_ovl0_mout_en;		/* 0xF00 */
	u32 disp_ovl0_2l_mout_en;	/* 0xF04 */
	u32 disp_ovl1_2l_mout_en;	/* 0xF08 */
	u32 disp_dither0_mout_en;	/* 0xF0C */
	u32 reserved_0xF10[5];		/* 0xF10 - 0xF20 */
	u32 disp_path0_sel_in;		/* 0xF24 */
	u32 reserved_0xF28;		/* 0xF28 */
	u32 dsi0_sel_in;		/* 0xF2C */
	u32 dpi0_sel_in;		/* 0xF30 */
	u32 reserved_0xF34;		/* 0xF34 */
	u32 disp_ovl0_2l_sel_in;	/* 0xF38 */
	u32 reserved_0xF3C[5];		/* 0xF3C - 0xF4C */
	u32 disp_rdma0_sout_sel_in;	/* 0xF50 */
	u32 disp_rdma1_sout_sel_in;	/* 0xF54 */
	u32 reserved_0xF58[3];		/* 0xF58 - 0xF60 */
	u32 dpi0_sel_sout_sel_in;	/* 0xF64 */
};

check_member(mmsys_cfg_regs, mmsys_cg_con0, 0x100);
check_member(mmsys_cfg_regs, dpi0_sel_sout_sel_in, 0xF64);
static struct mmsys_cfg_regs *const mmsys_cfg =
	(void *)MMSYS_BASE;

/* DISP_REG_CONFIG_MMSYS_CG_CON0
   Configures free-run clock gating 0
	0: Enable clock
	1: Clock gating  */
enum {
	CG_CON0_SMI_COMMON	= BIT(0),
	CG_CON0_SMI_LARB0	= BIT(1),
	CG_CON0_GALS_COMMON0	= BIT(3),
	CG_CON0_GALS_COMMON1	= BIT(4),
	CG_CON0_DISP_OVL0	= BIT(20),
	CG_CON0_DISP_OVL0_2L	= BIT(21),
	CG_CON0_DISP_OVL1_2L	= BIT(22),
	CG_CON0_DISP_RDMA0	= BIT(23),
	CG_CON0_DISP_RDMA1	= BIT(24),
	CG_CON0_DISP_WDMA0	= BIT(25),
	CG_CON0_DISP_COLOR0	= BIT(26),
	CG_CON0_DISP_CCORR0	= BIT(27),
	CG_CON0_DISP_AAL0	= BIT(28),
	CG_CON0_DISP_GAMMA0	= BIT(29),
	CG_CON0_DISP_DITHER0	= BIT(30),
	CG_CON0_DISP_ALL	= CG_CON0_SMI_COMMON |
				  CG_CON0_SMI_LARB0 |
				  CG_CON0_GALS_COMMON0 |
				  CG_CON0_GALS_COMMON1 |
				  CG_CON0_DISP_OVL0 |
				  CG_CON0_DISP_OVL0_2L |
				  CG_CON0_DISP_RDMA0 |
				  CG_CON0_DISP_COLOR0 |
				  CG_CON0_DISP_CCORR0 |
				  CG_CON0_DISP_AAL0 |
				  CG_CON0_DISP_DITHER0 |
				  CG_CON0_DISP_GAMMA0,
	CG_CON0_ALL		= 0xffffffff
};

/* DISP_REG_CONFIG_MMSYS_CG_CON1
   Configures free-run clock gating 1
	0: Enable clock
	1: Clock gating */
enum {
	CG_CON1_DISP_DSI0		= BIT(0),
	CG_CON1_DISP_DSI0_INTERFACE	= BIT(1),
	CG_CON1_DISP_26M		= BIT(7),

	CG_CON1_ALL	      = 0xffffffff
};

enum {
	OVL0_MOUT_EN_RDMA0		= BIT(0),
	OVL0_MOUT_EN_OVL0_2L		= BIT(4),
	OVL0_2L_MOUT_EN_DISP_PATH0	= BIT(0),
	OVL1_2L_MOUT_EN_DISP_RDMA1	= BIT(4),
	DITHER0_MOUT_EN_DISP_DSI0	= BIT(0),
};

enum {
	DISP_PATH0_SEL_IN_OVL0     = 0,
	DISP_PATH0_SEL_IN_OVL0_2L  = 1,
	DSI0_SEL_IN_DITHER0_MOUT   = 0,
	DSI0_SEL_IN_RDMA0	   = 1,
	RDMA0_SOUT_SEL_IN_DSI0	   = 0,
	RDMA0_SOUT_SEL_IN_COLOR    = 1,
};

struct disp_mutex_regs {
	u32 inten;
	u32 intsta;
	u32 reserved0[6];
	struct {
		u32 en;
		u32 dummy;
		u32 rst;
		u32 ctl;
		u32 mod;
		u32 reserved[3];
	} mutex[12];
};

static struct disp_mutex_regs *const disp_mutex = (void *)DISP_MUTEX_BASE;

enum {
	MUTEX_MOD_DISP_RDMA0	= BIT(0),
	MUTEX_MOD_DISP_RDMA1	= BIT(1),
	MUTEX_MOD_DISP_OVL0	= BIT(9),
	MUTEX_MOD_DISP_OVL0_2L	= BIT(10),
	MUTEX_MOD_DISP_OVL1_2L	= BIT(11),
	MUTEX_MOD_DISP_WDMA0	= BIT(12),
	MUTEX_MOD_DISP_COLOR0	= BIT(13),
	MUTEX_MOD_DISP_CCORR0	= BIT(14),
	MUTEX_MOD_DISP_AAL0	= BIT(15),
	MUTEX_MOD_DISP_GAMMA0	= BIT(16),
	MUTEX_MOD_DISP_DITHER0	= BIT(17),
	MUTEX_MOD_DISP_PWM0	= BIT(28),
	MUTEX_MOD_MAIN_PATH	= MUTEX_MOD_DISP_OVL0 | MUTEX_MOD_DISP_OVL0_2L |
				  MUTEX_MOD_DISP_RDMA0 | MUTEX_MOD_DISP_COLOR0 |
				  MUTEX_MOD_DISP_CCORR0 | MUTEX_MOD_DISP_AAL0 |
				  MUTEX_MOD_DISP_GAMMA0 |
				  MUTEX_MOD_DISP_DITHER0,
};

enum {
	MUTEX_SOF_SINGLE_MODE = 0,
	MUTEX_SOF_DSI0 = 1,
	MUTEX_SOF_DPI0 = 2,
};

struct disp_pq_regs {
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

enum {
	PQ_EN		= BIT(0),
	PQ_RELAY_MODE	= BIT(0),
};

static struct disp_pq_regs *const disp_ccorr = (void *)DISP_CCORR0_BASE;

static struct disp_pq_regs *const disp_aal = (void *)DISP_AAL0_BASE;

static struct disp_pq_regs *const disp_gamma = (void *)DISP_GAMMA0_BASE;

static struct disp_pq_regs *const disp_dither = (void *)DISP_DITHER0_BASE;

enum {
	SMI_LARB_NON_SEC_CON	= 0x380,
};

void mtk_ddp_init(void);
void mtk_ddp_mode_set(const struct edid *edid);

#endif
