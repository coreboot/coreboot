/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _SOC_MEDIATEK_MT8189_DDP_H_
#define _SOC_MEDIATEK_MT8189_DDP_H_

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/ddp_common.h>
#include <soc/display.h>
#include <types.h>

#define MAIN_PATH_OVL_NR	1
#define RDMA_PORT_NR		5

struct smi_larbx_regs {
	u32 reserved[992];
	u32 port_l0_ovl_rdma[RDMA_PORT_NR];
};
check_member(smi_larbx_regs, port_l0_ovl_rdma[0], 0xF80);

struct mmsys_cfg_regs {
	u32 reserved_0x000[64];
	u32 mmsys_cg_con0;
	u32 mmsys_cg_set0;
	u32 mmsys_cg_clr0;
	u32 reserved_0x10c;
	u32 mmsys_cg_con1;
	u32 mmsys_cg_set1;
	u32 mmsys_cg_clr1;
	u32 reserved_0x11c[569];
	u32 mmsys_security_disable;
	u32 mmsys_security_disable1;
	u32 mmsys_security_disable2;
	u32 reserved_0xa0c[125];
	u32 disp_bypass_mux_shadow;
	u32 disp_mout_rst;
	u32 reserved_0xc08;
	u32 disp_crossbar_con;
	u32 disp_ovl0_out0_mout_en;
	u32 disp_ovl1_out0_mout_en;
	u32 disp_rsz0_mout_en;
	u32 disp_rsz1_mout_en;
	u32 reserved_0xc20[21];
	u32 ovl_pq_out_cross0_mout_en;
	u32 ovl_pq_out_cross1_mout_en;
	u32 ovl_pq_out_cross2_mout_en;
	u32 ovl_pq_out_cross3_mout_en;
	u32 ovl_pq_out_cross4_mout_en;
	u32 ovl_pq_out_cross5_mout_en;
	u32 ovl_pq_out_cross0_sel_in;
	u32 ovl_pq_out_cross1_sel_in;
	u32 ovl_pq_out_cross2_sel_in;
	u32 ovl_pq_out_cross3_sel_in;
	u32 ovl_pq_out_cross4_sel_in;
	u32 ovl_pq_out_cross5_sel_in;
	u32 reserved_0xca4[51];
	u32 comp_out_cross0_mout_en;
	u32 comp_out_cross1_mout_en;
	u32 comp_out_cross2_mout_en;
	u32 comp_out_cross3_mout_en;
	u32 comp_out_cross4_mout_en;
	u32 comp_out_cross5_mout_en;
	u32 comp_out_cross0_sel_in;
	u32 comp_out_cross1_sel_in;
	u32 comp_out_cross2_sel_in;
	u32 comp_out_cross3_sel_in;
	u32 comp_out_cross4_sel_in;
	u32 comp_out_cross5_sel_in;
	u32 reserved_0xda0[24];
	u32 disp_rdma0_rsz0_sout_sel;
	u32 disp_rdma0_sel_in;
	u32 disp_rdma1_rsz1_sout_sel;
	u32 disp_rdma1_sel_in;
	u32 disp_rsz0_sel_in;
	u32 disp_rsz1_sel_in;
	u32 reserved_0xe18[3];
	u32 disp_ovl0_bgclr_mout_en;
	u32 disp_ovl1_bgclr_mout_en;
};
check_member(mmsys_cfg_regs, mmsys_cg_con0, 0x100);
check_member(mmsys_cfg_regs, mmsys_cg_set0, 0x104);
check_member(mmsys_cfg_regs, mmsys_cg_clr0, 0x108);
check_member(mmsys_cfg_regs, mmsys_cg_con1, 0x110);
check_member(mmsys_cfg_regs, mmsys_cg_set1, 0x114);
check_member(mmsys_cfg_regs, mmsys_cg_clr1, 0x118);
check_member(mmsys_cfg_regs, mmsys_security_disable, 0xA00);
check_member(mmsys_cfg_regs, mmsys_security_disable1, 0xA04);
check_member(mmsys_cfg_regs, mmsys_security_disable2, 0xA08);
check_member(mmsys_cfg_regs, disp_bypass_mux_shadow, 0xC00);
check_member(mmsys_cfg_regs, disp_crossbar_con, 0xC0C);
check_member(mmsys_cfg_regs, disp_ovl0_out0_mout_en, 0xC10);
check_member(mmsys_cfg_regs, ovl_pq_out_cross1_mout_en, 0xC78);
check_member(mmsys_cfg_regs, comp_out_cross4_mout_en, 0xD80);
check_member(mmsys_cfg_regs, disp_rdma0_rsz0_sout_sel, 0xE00);
check_member(mmsys_cfg_regs, disp_rdma0_sel_in, 0xE04);
check_member(mmsys_cfg_regs, disp_ovl0_bgclr_mout_en, 0xE24);

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
	CG_CON0_DISP_OVL0	= BIT(0),
	CG_CON0_DISP_RSZ0	= BIT(2),
	CG_CON0_DISP_RDMA0	= BIT(4),
	CG_CON0_DISP_COLOR0	= BIT(6),
	CG_CON0_DISP_CCORR0	= BIT(8),
	CG_CON0_DISP_CCORR2	= BIT(10),
	CG_CON0_DISP_AAL0	= BIT(12),
	CG_CON0_DISP_GAMMA0	= BIT(14),
	CG_CON0_DISP_DITHER0	= BIT(16),
	CG_CON0_DISP_DSC_WRAP0	= BIT(18),
	CG_CON0_DISP_MERGE0	= BIT(19),
	CG_CON0_DISP_DVO0	= BIT(20),
	CG_CON0_DISP_WDMA0	= BIT(24),
	CG_CON0_DISP_FAKE_ENG0	= BIT(26),
	CG_CON0_SMI_LARB	= BIT(28),
	CG_CON0_DISP_MUTEX0	= BIT(29),
	CG_CON0_DISPSYS_CONFIG	= BIT(30),
	CG_CON0_DISP_ALL	= CG_CON0_DISP_OVL0 |
				  CG_CON0_DISP_RSZ0 |
				  CG_CON0_DISP_RDMA0 |
				  CG_CON0_DISP_COLOR0 |
				  CG_CON0_DISP_CCORR0 |
				  CG_CON0_DISP_CCORR2 |
				  CG_CON0_DISP_AAL0 |
				  CG_CON0_DISP_GAMMA0 |
				  CG_CON0_DISP_DITHER0 |
				  CG_CON0_DISP_DVO0 |
				  CG_CON0_SMI_LARB |
				  CG_CON0_DISP_MUTEX0 |
				  CG_CON0_DISPSYS_CONFIG,
	CG_CON0_ALL		= 0xFFFFFFFF,
};

enum {
	CG_CON1_DISP_DSI0		= BIT(0),
	CG_CON1_LVDS_ENCODER		= BIT(1),
	CG_CON1_DPI_DPI0		= BIT(2),
	CG_CON1_DISP_DVO		= BIT(3),
	CG_CON1_DP_INTF			= BIT(4),
	CG_CON1_LVDS_ENCODER1		= BIT(5),
	CG_CON1_DISP_DVO1		= BIT(6),
	CG_CON1_DISP_ALL		= CG_CON1_DISP_DSI0 |
					  CG_CON1_LVDS_ENCODER |
					  CG_CON1_DPI_DPI0 |
					  CG_CON1_DISP_DVO |
					  CG_CON1_DP_INTF |
					  CG_CON1_LVDS_ENCODER1 |
					  CG_CON1_DISP_DVO1,
	CG_CON1_ALL			= 0xFFFFFFFF,
};

DEFINE_BITFIELD(DISP_OVL0_BGCLR_MOUT_EN, 1, 0)
DEFINE_BITFIELD(DISP_OVL0_OUT0_MOUT_EN, 2, 0)
DEFINE_BITFIELD(DISP_RDMA0_SEL_IN, 0, 0)
DEFINE_BITFIELD(DISP_OVL0_MOUT_EN, 2, 0)
DEFINE_BITFIELD(DISP_RDMA0_SOUT_SEL, 1, 0)
DEFINE_BITFIELD(DISP_OVL0_PQ_OUT_CROSSBAR_EN, 5, 0)
DEFINE_BITFIELD(DISP_COMP_OUT_CROSSBAR, 5, 0)

DEFINE_BIT(SMI_ID_EN, 0)
DEFINE_BIT(PQ_CFG_RELAY_MODE, 0)
DEFINE_BIT(PQ_CFG_ENGINE_EN, 1)

#define DISP_MOUT_OVL_TO_BLENDOUT	BIT(0)
#define DISP_OVL0_MOUT_TO_RDMA0		BIT(1)
#define DISP_RDMA0_FROM_OVL0		BIT(0)
#define DISP_OVL0_GO_BLEND		BIT(0)
#define DISP_RDMA0_SOUT_TO_COLOR0	1
#define DISP_RDMA0_SOUT_TO_CROSSBAR	0
#define DISP_PQ_OUT_CROSSBAR_TO_DVO0	BIT(0)
#define DISP_PQ_OUT_CROSSBAR_TO_CROSSBAR4	BIT(4)
#define DISP_DVO0_FROM_DITHER0		1
#define DISP_DVO0_FROM_CROSSBAR4		BIT(1)

#define SMI_ID_EN_VAL			BIT(0)

enum {
	MUTEX_MOD_DISP_OVL0		= BIT(0),
	MUTEX_MOD_DISP_RDMA0		= BIT(4),
	MUTEX_MOD_DISP_COLOR0		= BIT(4),
	MUTEX_MOD_DISP_CCORR0		= BIT(5),
	MUTEX_MOD_DISP_CCORR2		= BIT(6),
	MUTEX_MOD_DISP_AAL0		= BIT(7),
	MUTEX_MOD_DISP_GAMMA0		= BIT(8),
	MUTEX_MOD_DISP_DITHER0		= BIT(10),

	MUTEX_MOD_MAIN_PATH		= MUTEX_MOD_DISP_OVL0 |
					  MUTEX_MOD_DISP_RDMA0,
};

enum {
	MUTEX_SOF_SINGLE_MODE = 0,
	MUTEX_SOF_DSI0 = 1,
	MUTEX_SOF_DPI0 = 2,
	MUTEX_SOF_DVO = 5,
};

#define PQ_EN		BIT(0)
#define PQ_RELAY_MODE	BIT(0)
#define PQ_ENGINE_EN	BIT(1)

static struct smi_larbx_regs *const smi_larb0 = (void *)SMI_LARB0;
static struct smi_larbx_regs *const smi_larb1 = (void *)SMI_LARB1;
static struct mmsys_cfg_regs *const mmsys_cfg = (void *)MMSYS_CONFIG_BASE;
static struct disp_mutex_regs *const disp_mutex = (void *)DISP_MUTEX_BASE;
static struct disp_ccorr_regs *const disp_ccorr = (void *)DISP_CCORR0_BASE;
static struct disp_ccorr_regs *const disp_ccorr2 = (void *)DISP_CCORR2_BASE;
static struct disp_aal_regs *const disp_aal = (void *)DISP_AAL0_BASE;
static struct disp_gamma_regs *const disp_gamma = (void *)DISP_GAMMA0_BASE;
static struct disp_dither_regs *const disp_dither = (void *)DISP_DITHER0_BASE;

#endif
