/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_PMIF_SPMI_H__
#define __SOC_MEDIATEK_PMIF_SPMI_H__

#include <soc/addressmap.h>
#include <soc/pmif.h>
#include <soc/spmi.h>

#define DEFAULT_VALUE_READ_TEST		(0x5a)
#define DEFAULT_VALUE_WRITE_TEST	(0xa5)

#define PMIF_CMD_PER_3		(0x1 << PMIF_CMD_EXT_REG_LONG)
#define PMIF_CMD_PER_1_3	((0x1 << PMIF_CMD_REG) | (0x1 << PMIF_CMD_EXT_REG_LONG))

struct mtk_rgu_regs {
	u32 reserved[36];
	u32 wdt_swsysrst2;
};
check_member(mtk_rgu_regs, wdt_swsysrst2, 0x90);

struct mtk_spmi_mst_reg {
	u32 op_st_ctrl;
	u32 grp_id_en;
	u32 op_st_sta;
	u32 mst_sampl;
	u32 mst_req_en;
	u32 rcs_ctrl;
	u32 reserved1[10];
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

extern const struct spmi_device spmi_dev[];
extern const size_t spmi_dev_cnt;

int pmif_spmi_init(struct pmif *arb);
int spmi_config_master(void);
void pmif_spmi_iocfg(void);
void pmif_spmi_config(struct pmif *arb, int mstid);
#endif /* __SOC_MEDIATEK_PMIF_SPMI_H__ */
