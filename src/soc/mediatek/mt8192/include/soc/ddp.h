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
	u32 reserved_0x11C[33];		/* 0x11C */
	u32 mmsys_cg_con2;		/* 0x1A0 */
	u32 mmsys_cg_set2;		/* 0x1A4 */
	u32 mmsys_cg_clr2;		/* 0x1A8 */
	u32 reserved_0x1AC[853];	/* 0x1AC */
	u32 reserved_0xF00;		/* 0xF00 */
	u32 mmsys_ovl_mout_en;		/* 0xF04 */
	u32 reserved_0xF08;		/* 0xF08 */
	u32 reserved_0xF0C;		/* 0xF0C */
	u32 reserved_0xF10;		/* 0xF10 */
	u32 reserved_0xF14;		/* 0xF14 */
	u32 ovl0_2l_mout_en;		/* 0xF18 */
	u32 ovl0_mout_en;		/* 0xF1C */
	u32 reserved_0xF20;		/* 0xF20 */
	u32 reserved_0xF24;		/* 0xF24 */
	u32 reserved_0xF28;		/* 0xF28 */
	u32 rdma0_sel_in;		/* 0xF2C */
	u32 rdma0_sout_sel;		/* 0xF30 */
	u32 ccorr0_sout_sel;		/* 0xF34 */
	u32 aal0_sel_in;		/* 0xF38 */
	u32 dither0_mout_en;		/* 0xF3C*/
	u32 dsi0_sel_in;		/* 0xF40*/
};

check_member(mmsys_cfg_regs, mmsys_cg_con0, 0x100);
check_member(mmsys_cfg_regs, mmsys_cg_con1, 0x110);
check_member(mmsys_cfg_regs, mmsys_cg_con2, 0x1A0);
check_member(mmsys_cfg_regs, mmsys_ovl_mout_en, 0xF04);
check_member(mmsys_cfg_regs, ovl0_2l_mout_en, 0xF18);
check_member(mmsys_cfg_regs, dsi0_sel_in, 0xF40);
static struct mmsys_cfg_regs *const mmsys_cfg =
	(void *)MMSYS_BASE;


/* DISP_REG_CONFIG_MMSYS_CG_CON0
   Configures free-run clock gating 0
	0: Enable clock
	1: Clock gating  */
enum {
	CG_CON0_DISP_MUTEX0	= BIT(0),
	CG_CON0_DISPSYS_CONFIG	= BIT(1),
	CG_CON0_DISP_OVL0	= BIT(2),
	CG_CON0_DISP_RDMA0	= BIT(3),
	CG_CON0_DISP_OVL0_2L	= BIT(4),
	CG_CON0_DISP_AAL0	= BIT(8),
	CG_CON0_DISP_CCORR0	= BIT(9),
	CG_CON0_DISP_DITHER0	= BIT(10),
	CG_CON0_SMI_INFRA	= BIT(11),
	CG_CON0_DISP_GAMMA0	= BIT(12),
	CG_CON0_DISP_POSTMASK0	= BIT(13),
	CG_CON0_DISP_DSI0	= BIT(15),
	CG_CON0_DISP_COLOR0	= BIT(16),
	CG_CON0_SMI_COMMON	= BIT(17),

	CG_CON0_SMI_GALS	= BIT(27),
	CG_CON0_DISP_ALL	= CG_CON0_SMI_INFRA |
				  CG_CON0_SMI_COMMON |
				  CG_CON0_SMI_GALS |
				  CG_CON0_DISP_MUTEX0 |
				  CG_CON0_DISPSYS_CONFIG |
				  CG_CON0_DISP_OVL0 |
				  CG_CON0_DISP_RDMA0 |
				  CG_CON0_DISP_OVL0_2L |
				  CG_CON0_DISP_AAL0 |
				  CG_CON0_DISP_CCORR0 |
				  CG_CON0_DISP_DITHER0 |
				  CG_CON0_DISP_GAMMA0 |
				  CG_CON0_DISP_POSTMASK0 |
				  CG_CON0_DISP_DSI0 |
				  CG_CON0_DISP_COLOR0,
	CG_CON0_ALL		= 0xffffffff
};

/* DISP_REG_CONFIG_MMSYS_CG_CON1
   Configures free-run clock gating 1
	0: Enable clock
	1: Clock gating */
enum {
	CG_CON1_SMI_IOMMU	= BIT(0),
	CG_CON1_DISP_ALL	= CG_CON1_SMI_IOMMU,
	CG_CON1_ALL		= 0xffffffff
};

enum {
	CG_CON2_DSI_DSI0	= BIT(0),
	CG_CON2_DPI_DPI0	= BIT(8),
	CG_CON2_MM_26MHZ	= BIT(24),
	CG_CON2_DISP_ALL	= CG_CON2_DSI_DSI0 |
				  CG_CON2_MM_26MHZ,
	CG_CON2_ALL		= 0xffffffff
};


enum {
	DISP_OVL0_GO_BLEND	= BIT(0),
	DISP_OVL0_GO_BG		= BIT(1),
	DISP_OVL0_2L_GO_BLEND	= BIT(2),
	DISP_OVL0_2L_GO_BG	= BIT(3),
	OVL0_MOUT_EN_DISP_RDMA0	= BIT(0),
	DITHER0_MOUT_DSI0	= BIT(0),
};

enum {
	RDMA0_SEL_IN_OVL0_2L	= 0x3,
	RDMA0_SOUT_COLOR0	= 0x1,
	CCORR0_SOUT_AAL0	= 0x1,
	AAL0_SEL_IN_CCORR0	= 0x1,
	DSI0_SEL_IN_DITHER0	= 0x1,
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
	MUTEX_MOD_DISP_OVL0_2L		= BIT(1),
	MUTEX_MOD_DISP_RDMA0		= BIT(2),
	MUTEX_MOD_DISP_COLOR0		= BIT(4),
	MUTEX_MOD_DISP_CCORR0		= BIT(5),
	MUTEX_MOD_DISP_AAL0		= BIT(6),
	MUTEX_MOD_DISP_GAMMA0		= BIT(7),
	MUTEX_MOD_DISP_POSTMASK0	= BIT(8),
	MUTEX_MOD_DISP_DITHER0		= BIT(9),
	MUTEX_MOD_MAIN_PATH		= MUTEX_MOD_DISP_OVL0 |
					  MUTEX_MOD_DISP_OVL0_2L |
					  MUTEX_MOD_DISP_RDMA0 |
					  MUTEX_MOD_DISP_COLOR0 |
					  MUTEX_MOD_DISP_CCORR0 |
					  MUTEX_MOD_DISP_AAL0 |
					  MUTEX_MOD_DISP_GAMMA0 |
					  MUTEX_MOD_DISP_POSTMASK0 |
					  MUTEX_MOD_DISP_DITHER0,
};

enum {
	MUTEX_SOF_SINGLE_MODE = 0,
	MUTEX_SOF_DSI0 = 1,
	MUTEX_SOF_DPI0 = 2,
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
check_member(disp_aal_regs, shadow, 0xF0);
check_member(disp_aal_regs, output_size, 0x4D8);

struct disp_postmask_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 intsta;
	u32 reserved0[4];
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

static struct disp_dither_regs *const disp_dither = (void *)DISP_DITHER0_BASE;

static struct disp_postmask_regs *const disp_postmask = (void *)DISP_POSTMASK0_BASE;

enum {
	SMI_LARB_PORT_L0_OVL_RDMA0	= 0x388,
};

void mtk_ddp_init(void);
void mtk_ddp_mode_set(const struct edid *edid);

#endif
