/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PMIF_SPMI_H__
#define __PMIF_SPMI_H__

#include <soc/addressmap.h>

#define DEFAULT_VALUE_READ_TEST		(0x5a)
#define DEFAULT_VALUE_WRITE_TEST	(0xa5)

/* indicate which number SW channel start, by project */
#define PMIF_SPMI_SW_CHAN	BIT(6)
#define PMIF_SPMI_INF		0x2F7

struct mtk_rgu_regs {
	u32 reserved[36];
	u32 wdt_swsysrst2;
};
check_member(mtk_rgu_regs, wdt_swsysrst2, 0x90);

struct mtk_iocfg_bm_regs {
	u32 reserved[8];
	u32 drv_cfg2;
};
check_member(mtk_iocfg_bm_regs, drv_cfg2, 0x20);

struct mtk_spm_regs {
	u32 poweron_config_en;
	u32 reserved[263];
	u32 ulposc_con;
};
check_member(mtk_spm_regs, ulposc_con, 0x420);

struct mtk_spmi_mst_reg {
	u32 op_st_ctrl;
	u32 grp_id_en;
	u32 op_st_sta;
	u32 mst_sampl;
	u32 mst_req_en;
	u32 reserved1[11];
	u32 rec_ctrl;
	u32 rec0;
	u32 rec1;
	u32 rec2;
	u32 rec3;
	u32 rec4;
	u32 reserved2[41];
	u32 mst_dbg;
};

check_member(mtk_spmi_mst_reg, rec_ctrl, 0x40);
check_member(mtk_spmi_mst_reg, mst_dbg, 0xfc);

#define mtk_rug	((struct mtk_rgu_regs *)RGU_BASE)
#define mtk_iocfg_bm	((struct mtk_iocfg_bm_regs *)IOCFG_BM_BASE)
#define mtk_spm	((struct mtk_spm_regs *)SPM_BASE)
#define mtk_spmi_mst	((struct mtk_spmi_mst_reg *)SPMI_MST_BASE)

struct cali {
	unsigned int dly;
	unsigned int pol;
};

enum {
	SPMI_CK_NO_DLY = 0,
	SPMI_CK_DLY_1T,
};

enum {
	SPMI_CK_POL_NEG = 0,
	SPMI_CK_POL_POS,
};

enum spmi_regs {
	SPMI_OP_ST_CTRL,
	SPMI_GRP_ID_EN,
	SPMI_OP_ST_STA,
	SPMI_MST_SAMPL,
	SPMI_MST_REQ_EN,
	SPMI_REC_CTRL,
	SPMI_REC0,
	SPMI_REC1,
	SPMI_REC2,
	SPMI_REC3,
	SPMI_REC4,
	SPMI_MST_DBG
};

/* MT6315 registers */
enum {
	MT6315_BASE        = 0x0,
	MT6315_READ_TEST   = MT6315_BASE + 0x9,
	MT6315_READ_TEST_1 = MT6315_BASE + 0xb,
};

#define MT6315_DEFAULT_VALUE_READ 0x15

extern int pmif_spmi_init(struct pmif *arb);
#endif /*__PMIF_SPMI_H__*/
