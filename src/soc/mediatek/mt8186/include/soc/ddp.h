/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 6.9
 */

#ifndef SOC_MEDIATEK_MT8186_DDP_H
#define SOC_MEDIATEK_MT8186_DDP_H

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/ddp_common.h>
#include <types.h>

#define SMI_LARB_PORT_L0_OVL_RDMA0 0x388

struct mmsys_cfg_regs {
	u32 reserved_0x000[64];		/* 0x000 */
	u32 mmsys_cg_con0;		/* 0x100 */
	u32 mmsys_cg_set0;		/* 0x104 */
	u32 mmsys_cg_clr0;		/* 0x108 */
	u32 reserved_0x10c;		/* 0x10C */
	u32 mmsys_cg_con1;		/* 0x110 */
	u32 mmsys_cg_set1;		/* 0x114 */
	u32 mmsys_cg_clr1;		/* 0x118 */
	u32 reserved_0x11c[33];		/* 0x11C */
	u32 mmsys_cg_con2;		/* 0x1A0 */
	u32 mmsys_cg_set2;		/* 0x1A4 */
	u32 mmsys_cg_clr2;		/* 0x1A8 */
	u32 reserved_0x1ac[853];	/* 0x1AC */
	u32 reserved_0xf00;		/* 0xF00 */
	u32 mmsys_ovl_con;		/* 0xF04 */
	u32 reserved_0xf08;		/* 0xF08 */
	u32 disp_rdma0_sout_sel;	/* 0xF0C */
	u32 reserved_0xf10;		/* 0xF10 */
	u32 disp_ovl0_2l_mout_en;	/* 0xF14 */
	u32 disp_ovl0_mout_en;		/* 0xF18 */
	u32 reserved_0xf1c;		/* 0xF1C */
	u32 disp_dither0_mout_en;	/* 0xF20 */
	u32 reserved_0xf24;		/* 0xF24 */
	u32 disp_rdma0_sel_in;		/* 0xF28 */
	u32 reserved_0xf2c;		/* 0xF2C */
	u32 disp_dsi0_sel_in;		/* 0xF30 */
	u32 reserved_0xf34[2];		/* 0xF34 */
	u32 disp_rdma1_mout_en;		/* 0xF3C */
	u32 disp_rdma1_sel_in;		/* 0xF40 */
	u32 disp_dpi0_sel_in;		/* 0xF44 */
};
check_member(mmsys_cfg_regs, mmsys_cg_con0, 0x100);
check_member(mmsys_cfg_regs, mmsys_cg_con1, 0x110);
check_member(mmsys_cfg_regs, mmsys_cg_con2, 0x1A0);
check_member(mmsys_cfg_regs, mmsys_ovl_con, 0xF04);
check_member(mmsys_cfg_regs, disp_rdma0_sout_sel, 0xF0C);
check_member(mmsys_cfg_regs, disp_ovl0_mout_en, 0xF18);
check_member(mmsys_cfg_regs, disp_dither0_mout_en, 0xF20);
check_member(mmsys_cfg_regs, disp_rdma0_sel_in, 0xF28);
check_member(mmsys_cfg_regs, disp_dsi0_sel_in, 0xF30);

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
	} mutex[16];
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
	u32 disp_dither_0;
};
check_member(disp_dither_regs, disp_dither_0, 0x100);

/*
 * DISP_REG_CONFIG_MMSYS_CG_CON0
 * Configures free-run clock gating 0
 * 0: Enable clock
 * 1: Clock gating
 */
enum {
	CG_CON0_DISP_MUTEX0	= BIT(0),
	CG_CON0_APB_MM_BUS	= BIT(1),
	CG_CON0_DISP_OVL0	= BIT(2),
	CG_CON0_DISP_RDMA0	= BIT(3),
	CG_CON0_DISP_OVL0_2L	= BIT(4),
	CG_CON0_DISP_WDMA0	= BIT(5),
	CG_CON0_DISP_RSZ0	= BIT(7),
	CG_CON0_DISP_AAL0	= BIT(8),
	CG_CON0_DISP_CCORR0	= BIT(9),
	CG_CON0_DISP_COLOR0	= BIT(10),
	CG_CON0_SMI_INFRA	= BIT(11),
	CG_CON0_DISP_GAMMA0	= BIT(13),
	CG_CON0_DISP_POSTMASK0	= BIT(14),
	CG_CON0_DISP_DITHER0	= BIT(16),
	CG_CON0_SMI_COMMON	= BIT(17),
	CG_CON0_DISP_DSI0	= BIT(19),
	CG_CON0_DISP_FAKE_ENG0	= BIT(20),
	CG_CON0_DISP_FAKE_ENG1	= BIT(21),
	CG_CON0_SMI_GALS	= BIT(22),
	CG_CON0_SMI_IOMMU	= BIT(24),
	CG_CON0_DISP_ALL	= CG_CON0_DISP_MUTEX0 |
				  CG_CON0_APB_MM_BUS |
				  CG_CON0_DISP_OVL0 |
				  CG_CON0_DISP_RDMA0 |
				  CG_CON0_DISP_AAL0 |
				  CG_CON0_DISP_CCORR0 |
				  CG_CON0_DISP_COLOR0 |
				  CG_CON0_SMI_INFRA |
				  CG_CON0_DISP_GAMMA0 |
				  CG_CON0_DISP_POSTMASK0 |
				  CG_CON0_DISP_DITHER0 |
				  CG_CON0_SMI_COMMON |
				  CG_CON0_DISP_DSI0 |
				  CG_CON0_SMI_GALS |
				  CG_CON0_SMI_IOMMU,
	CG_CON0_ALL		= 0xFFFFFFFF,
};

enum {
	CG_CON2_DSI0_DSI_CK_DOMAIN	= BIT(0),
	CG_CON2_DISP_26M		= BIT(10),
	CG_CON2_DISP_ALL		= CG_CON2_DSI0_DSI_CK_DOMAIN |
					  CG_CON2_DISP_26M,
	CG_CON2_ALL			= 0xFFFFFFFF,
};

DEFINE_BITFIELD(DISP_OVL0_MOUT_EN, 3, 0)
DEFINE_BITFIELD(DISP_RDMA0_SEL_IN, 3, 0)
DEFINE_BITFIELD(DISP_MMSYS_OVL0_CON, 1, 0)
DEFINE_BITFIELD(DISP_RDMA0_SOUT_SEL, 3, 0)
DEFINE_BITFIELD(DISP_DITHER0_MOUT_EN, 3, 0)
DEFINE_BITFIELD(DISP_DSI0_SEL_IN, 3, 0)

DEFINE_BIT(SMI_ID_EN, 0)
DEFINE_BIT(PQ_CFG_RELAY_MODE, 0)
DEFINE_BIT(PQ_CFG_ENGINE_EN, 1)

#define DISP_OVL0_MOUT_TO_RDMA0		BIT(0)
#define DISP_RDMA0_FROM_OVL0		0
#define DISP_OVL0_GO_BLEND		BIT(0)
#define DISP_RDMA0_SOUT_TO_COLOR0	1
#define DISP_DITHER0_MOUT_TO_DSI0	BIT(0)
#define DISP_DSI0_FROM_DITHER0		1

#define SMI_ID_EN_VAL			BIT(0)

enum {
	MUTEX_MOD_DISP_OVL0		= BIT(0),
	MUTEX_MOD_DISP_RDMA0		= BIT(2),
	MUTEX_MOD_DISP_COLOR0		= BIT(4),
	MUTEX_MOD_DISP_CCORR0		= BIT(5),
	MUTEX_MOD_DISP_AAL0		= BIT(7),
	MUTEX_MOD_DISP_GAMMA0		= BIT(8),
	MUTEX_MOD_DISP_POSTMASK0	= BIT(9),
	MUTEX_MOD_DISP_DITHER0		= BIT(10),
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
	MUTEX_SOF_SINGLE_MODE = 0,
	MUTEX_SOF_DSI0 = 1,
	MUTEX_SOF_DPI0 = 2,
};

#define PQ_EN		BIT(0)
#define PQ_RELAY_MODE	BIT(0)
#define PQ_ENGINE_EN	BIT(1)

static struct mmsys_cfg_regs *const mmsys_cfg = (void *)MMSYS_BASE;
static struct disp_mutex_regs *const disp_mutex = (void *)DISP_MUTEX_BASE;
static struct disp_ccorr_regs *const disp_ccorr = (void *)DISP_CCORR0_BASE;
static struct disp_aal_regs *const disp_aal = (void *)DISP_AAL0_BASE;
static struct disp_gamma_regs *const disp_gamma = (void *)DISP_GAMMA0_BASE;
static struct disp_postmask_regs *const disp_postmask = (void *)DISP_POSTMASK0_BASE;
static struct disp_dither_regs *const disp_dither = (void *)DISP_DITHER0_BASE;

void mtk_ddp_init(void);
void mtk_ddp_mode_set(const struct edid *edid);

#endif
