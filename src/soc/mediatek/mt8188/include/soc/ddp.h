/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _SOC_MEDIATEK_MT8188_DDP_H_
#define _SOC_MEDIATEK_MT8188_DDP_H_

#include <soc/addressmap.h>
#include <soc/ddp_common.h>
#include <types.h>

#define MAIN_PATH_OVL_NR 1

struct mmsys_cfg_regs {
	u32 reserved_0x000[64];		/* 0x000 */
	u32 mmsys_cg_con0;		/* 0x100 */
	u32 mmsys_cg_set0;		/* 0x104 */
	u32 mmsys_cg_clr0;		/* 0x108 */
	u32 reserved_0x10c;		/* 0x10C */
	u32 mmsys_cg_con1;		/* 0x110 */
	u32 mmsys_cg_set1;		/* 0x114 */
	u32 mmsys_cg_clr1;		/* 0x118 */
	u32 reserved_0x11c;		/* 0x11C */
	u32 mmsys_cg_con2;		/* 0x120 */
	u32 mmsys_cg_set2;		/* 0x124 */
	u32 mmsys_cg_clr2;		/* 0x128 */
	u32 reserved_0x12c[885];	/* 0x12C */
	u32 reserved_0xf00;		/* 0xF00 */
	u32 reserved_0xf04;		/* 0xF04 */
	u32 reserved_0xf08;		/* 0xF08 */
	u32 reserved_0xf0c;		/* 0xF0C */
	u32 reserved_0xf10;		/* 0xF10 */
	u32 mmsys_ovl_mout_en;		/* 0xF14 */
	u32 reserved_0xf18;		/* 0xF18 */
	u32 reserved_0xf1c;		/* 0xF1C */
	u32 reserved_0xf20;		/* 0xF20 */
	u32 reserved_0xf24;		/* 0xF24 */
	u32 reserved_0xf28;		/* 0xF28 */
	u32 reserved_0xf2c;		/* 0xF2C */
	u32 reserved_0xf30;		/* 0xF30 */
	u32 mmsys_sel_in;		/* 0xF34 */
	u32 mmsys_sel_out;		/* 0xF38 */
	u32 reserved_0xf3c;		/* 0xF3C */
	u32 reserved_0xf40;		/* 0xF40 */
	u32 mmsys_dsi0_sel_in;		/* 0xF44 */
	u32 reserved_0xf48;		/* 0xF48 */
	u32 mmsys_dp_intf0_sel_in;	/* 0xF4C */
	u32 reserved_0xf50;		/* 0xF50 */
	u32 reserved_0xf54;		/* 0xF54 */
	u32 mmsys_dither0_sel_out;	/* 0xF58 */
};
check_member(mmsys_cfg_regs, mmsys_cg_con0, 0x100);
check_member(mmsys_cfg_regs, mmsys_cg_con1, 0x110);
check_member(mmsys_cfg_regs, mmsys_cg_con2, 0x120);
check_member(mmsys_cfg_regs, mmsys_ovl_mout_en, 0xF14);

static struct mmsys_cfg_regs *const mmsys_cfg = (void *)VDOSYS0_BASE;

/*
 * DISP_REG_CONFIG_MMSYS_CG_CON0
 * Configures free-run vdo0_clks gating 0
 * - 0: Enable clock
 * - 1: Clock gating
 */
enum {
	CG_CON0_DISP_OVL0	= BIT(0),
	CG_CON0_DISP_CCORR0	= BIT(4),
	CG_CON0_DISP_MUTEX0	= BIT(6),
	CG_CON0_DISP_GAMMA0	= BIT(8),
	CG_CON0_DISP_DITHER0	= BIT(10),
	CG_CON0_DISP_RDMA0	= BIT(19),
	CG_CON0_DISP_DSI0	= BIT(21),
	CG_CON0_DISP_DSC_WRAP0	= BIT(23),
	CG_CON0_DISP_VPP_MERGE0	= BIT(24),
	CG_CON0_DISP_DP_INTF0	= BIT(25),
	CG_CON0_DISP_AAL0	= BIT(26),
	CG_CON0_DISP_COLOR0	= BIT(29),

	CG_CON0_DISP_ALL	= CG_CON0_DISP_MUTEX0 |
				  CG_CON0_DISP_OVL0 |
				  CG_CON0_DISP_RDMA0 |
				  CG_CON0_DISP_COLOR0 |
				  CG_CON0_DISP_CCORR0 |
				  CG_CON0_DISP_AAL0 |
				  CG_CON0_DISP_GAMMA0 |
				  CG_CON0_DISP_DITHER0 |
				  CG_CON0_DISP_DP_INTF0 |
				  CG_CON0_DISP_DSI0,
	CG_CON0_ALL		= 0xffffffff
};

/*
 * DISP_REG_CONFIG_MMSYS_CG_CON1
 * Configures free-run clock gating 0
 * - 0: Enable clock
 * - 1: Clock gating
 */
enum {
	CG_CON1_DISP_POSTMASK0	= BIT(0),
	CG_CON1_SMI_GALS	= BIT(10),
	CG_CON1_SMI_COMMON	= BIT(11),
	CG_CON1_SMI_EMI		= BIT(12),
	CG_CON1_SMI_IOMMU	= BIT(13),
	CG_CON1_SMI_LARB	= BIT(14),
	CG_CON1_SMI_RSI		= BIT(15),

	CG_CON1_DISP_ALL	= CG_CON1_DISP_POSTMASK0 |
				  CG_CON1_SMI_GALS |
				  CG_CON1_SMI_COMMON |
				  CG_CON1_SMI_EMI |
				  CG_CON1_SMI_IOMMU |
				  CG_CON1_SMI_LARB |
				  CG_CON1_SMI_RSI,
	CG_CON1_ALL		= 0xffffffff
};

/*
 * DISP_REG_CONFIG_MMSYS_CG_CON2
 * Configures free-run clock gating 0
 * - 0: Enable clock
 * - 1: Clock gating
 */
enum {
	CG_CON2_DSI_DSI0	= BIT(0),
	CG_CON2_DPI_DPI0	= BIT(8),
	CG_CON2_DP_INTF0	= BIT(16),

	CG_CON2_DISP_ALL	= CG_CON2_DSI_DSI0 |
				  CG_CON2_DP_INTF0,
	CG_CON2_ALL		= 0xffffffff
};

enum {
	DISP_OVL0_GO_BLEND	= BIT(0),
	DISP_OVL0_GO_BG		= BIT(1),
	DISP_OVL0_TO_DISP_RDMA0	= BIT(0),
	DITHER0_MOUT_DSI0	= BIT(0),
};


enum {
	SEL_IN_DSI0_FROM_DSC_WRAP0_OUT	= (0 << 0),
	SEL_IN_DSI0_FROM_DISP_DITHER0	= (1 << 0),
};

enum {
	SEL_IN_DP_INTF0_FROM_DSC_WRAP0C1_OUT	= (0 << 0),
	SEL_IN_DP_INTF0_FROM_VPP_MERGE		= (1 << 0),
	SEL_IN_DP_INTF0_FROM_DISP_DITHER0	= (3 << 0),
};

enum {
	SEL_OUT_DISP_DITHER0_TO_DSC_WRAP0_IN	= (0 << 0),
	SEL_OUT_DISP_DITHER0_TO_DSI0		= (1 << 0),
	SEL_OUT_DISP_DITHER0_TO_VPP_MERGE0	= (6 << 0),
	SEL_OUT_DISP_DITHER0_TO_DP_INTF0	= (7 << 0),
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
	MUTEX_MOD_DISP_OVL0		= BIT(0),
	MUTEX_MOD_DISP_RDMA0		= BIT(2),
	MUTEX_MOD_DISP_COLOR0		= BIT(3),
	MUTEX_MOD_DISP_CCORR0		= BIT(4),
	MUTEX_MOD_DISP_AAL0		= BIT(5),
	MUTEX_MOD_DISP_GAMMA0		= BIT(6),
	MUTEX_MOD_DISP_DITHER0		= BIT(7),
	MUTEX_MOD_DISP_POSTMASK0	= BIT(24),
	MUTEX_MOD_MAIN_PATH		= MUTEX_MOD_DISP_OVL0 |
					  MUTEX_MOD_DISP_RDMA0 |
					  MUTEX_MOD_DISP_COLOR0 |
					  MUTEX_MOD_DISP_CCORR0 |
					  MUTEX_MOD_DISP_AAL0 |
					  MUTEX_MOD_DISP_GAMMA0 |
					  MUTEX_MOD_DISP_POSTMASK0 |
					  MUTEX_MOD_DISP_DITHER0,
};

enum {
	MUTEX_SOF_SINGLE_MODE	= 0,
	MUTEX_SOF_DSI0		= 1,
	MUTEX_SOF_DPI0		= 2,
	MUTEX_SOF_DP_INTF0	= 3,
};

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
check_member(disp_ccorr_regs, shadow, 0xa0);

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

struct disp_aal_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 intsta;
	u32 status;
	u32 reserved0[3];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
	u32 reserved2[47];
	u32 shadow;
	u32 reserved3[249];
	u32 output_size;
};
check_member(disp_aal_regs, shadow, 0xf0);
check_member(disp_aal_regs, output_size, 0x4d8);

struct disp_postmask_regs {
	u32 en;
	u32 reserved0[7];
	u32 cfg;
	u32 reserved1[3];
	u32 size;
};
check_member(disp_postmask_regs, size, 0x30);

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

enum {
	PQ_EN		= BIT(0),
	PQ_RELAY_MODE	= BIT(0),
	PQ_ENGINE_EN	= BIT(1),
};

static struct disp_ccorr_regs *const disp_ccorr = (void *)DISP_CCORR0_BASE;
static struct disp_aal_regs *const disp_aal = (void *)DISP_AAL0_BASE;
static struct disp_gamma_regs *const disp_gamma = (void *)DISP_GAMMA0_BASE;
static struct disp_postmask_regs *const disp_postmask = (void *)DISP_POSTMASK0_BASE;
static struct disp_dither_regs *const disp_dither = (void *)DISP_DITHER0_BASE;

enum {
	SMI_LARB_PORT_L0_OVL_RDMA0	= 0xF88,
};

enum disp_path_sel {
	DISP_PATH_EDP = 0,
	DISP_PATH_MIPI,
};

void mtk_ddp_init(void);
void mtk_ddp_mode_set(const struct edid *edid, enum disp_path_sel);

#endif
