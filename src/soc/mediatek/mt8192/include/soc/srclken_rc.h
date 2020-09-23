/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_SRCLKEN_RC_H
#define SOC_MEDIATEK_MT8192_SRCLKEN_RC_H

#include <device/mmio.h>

struct mtk_rc_regs {
	u32 srclken_rc_cfg;
	u32 rc_central_cfg1;
	u32 rc_central_cfg2;
	u32 rc_cmd_arb_cfg;
	u32 rc_pmic_rcen_addr;
	u32 rc_pmic_rcen_set_clr_addr;
	u32 rc_dcxo_fpm_cfg;
	u32 rc_central_cfg3;
	u32 rc_mxx_srclken_cfg[13];
	u32 srclken_sw_con_cfg;
	u32 rc_central_cfg4;
	u32 reserved1;
	u32 rc_protocol_chk_cfg;
	u32 rc_debug_cfg;
	u32 reserved2[19];
	u32 rc_misc_0;
	u32 rc_spm_ctrl;
	u32 rc_subsys_intf_cfg;
};

check_member(mtk_rc_regs, rc_central_cfg1, 0x4);
check_member(mtk_rc_regs, rc_mxx_srclken_cfg[0], 0x20);
check_member(mtk_rc_regs, rc_mxx_srclken_cfg[12], 0x50);
check_member(mtk_rc_regs, rc_central_cfg4, 0x58);
check_member(mtk_rc_regs, rc_protocol_chk_cfg, 0x60);
check_member(mtk_rc_regs, rc_misc_0, 0xb4);
check_member(mtk_rc_regs, rc_subsys_intf_cfg, 0xbc);

struct mtk_rc_status_regs {
	u32 rc_fsm_sta_0;
	u32 rc_cmd_sta_0;
	u32 rc_cmd_sta_1;
	u32 rc_spi_sta_0;
	u32 rc_pi_po_sta_0;
	u32 rc_mxx_req_sta_0[14];
	u32 reserved2[2];
	u32 rc_debug_trace;
};

check_member(mtk_rc_status_regs, rc_cmd_sta_1, 0x8);
check_member(mtk_rc_status_regs, rc_mxx_req_sta_0[0], 0x14);
check_member(mtk_rc_status_regs, rc_mxx_req_sta_0[13], 0x48);
check_member(mtk_rc_status_regs, rc_debug_trace, 0x54);

/* SPM Register */
/* SRCLKEN_RC_CFG */
DEFINE_BIT(SW_RESET, 0)
DEFINE_BIT(CG_32K_EN, 1)
DEFINE_BIT(CG_FCLK_EN, 2)
DEFINE_BIT(CG_FCLK_FR_EN, 3)
DEFINE_BIT(MUX_FCLK_FR, 4)

/* RC_CENTRAL_CFG1 */
DEFINE_BIT(SRCLKEN_RC_EN, 0)
DEFINE_BIT(RCEN_ISSUE_M, 1)
DEFINE_BIT(RC_SPI_ACTIVE, 2)
DEFINE_BIT(SRCLKEN_RC_EN_SEL, 3)
DEFINE_BITFIELD(VCORE_SETTLE_T, 7, 5)
DEFINE_BITFIELD(ULPOSC_SETTLE_T, 11, 8)
DEFINE_BITFIELD(NON_DCXO_SETTLE_T, 21, 12)
DEFINE_BITFIELD(DCXO_SETTLE_T, 31, 22)

/* RC_CENTRAL_CFG2 */
DEFINE_BITFIELD(SRCVOLTEN_CTRL, 3, 0)
DEFINE_BITFIELD(VREQ_CTRL, 7, 4)
DEFINE_BIT(SRCVOLTEN_VREQ_SEL, 8)
DEFINE_BIT(SRCVOLTEN_VREQ_M, 9)
DEFINE_BITFIELD(ULPOSC_CTRL_M, 15, 12)
DEFINE_BITFIELD(PWRAP_SLP_CTRL_M, 24, 21)
DEFINE_BIT(PWRAP_SLP_MUX_SEL, 25)

/* RC_DCXO_FPM_CFG */
DEFINE_BITFIELD(DCXO_FPM_CTRL_M, 3, 0)
DEFINE_BIT(SRCVOLTEN_FPM_MSK_B, 4)
DEFINE_BITFIELD(SUB_SRCLKEN_FPM_MSK_B, 28, 16)

/* RC_CENTRAL_CFG3 */
DEFINE_BIT(TO_LPM_SETTLE_EN, 0)
DEFINE_BIT(BLK_SCP_DXCO_MD_TARGET, 1)
DEFINE_BIT(BLK_COANT_DXCO_MD_TARGET, 2)
DEFINE_BIT(TO_BBLPM_SETTLE_EN, 3)
DEFINE_BITFIELD(TO_LPM_SETTLE_T, 21, 12)

/* RC_CENTRAL_CFG4 */
DEFINE_BITFIELD(KEEP_RC_SPI_ACTIVE, 8, 0)
DEFINE_BIT(PWRAP_VLD_FORCE, 16)
DEFINE_BIT(SLEEP_VLD_MODE, 17)

/* RC_MXX_SRCLKEN_CFG */
DEFINE_BIT(DCXO_SETTLE_BLK_EN, 1)
DEFINE_BIT(BYPASS_CMD_EN, 2)
DEFINE_BIT(SW_SRCLKEN_RC, 3)
DEFINE_BIT(SW_SRCLKEN_FPM, 4)
DEFINE_BIT(SW_SRCLKEN_BBLPM, 5)
DEFINE_BIT(XO_SOC_LINK_EN, 6)
DEFINE_BIT(REQ_ACK_LOW_IMD_EN, 7)
DEFINE_BIT(SRCLKEN_TRACK_M_EN, 8)
DEFINE_BITFIELD(CNT_PRD_STEP, 11, 10)
DEFINE_BITFIELD(XO_STABLE_PRD, 21, 12)
DEFINE_BITFIELD(DCXO_STABLE_PRD, 31, 22)

enum {
	SW_SRCLKEN_FPM_MSK = 0x1,
	SW_SRCLKEN_BBLPM_MSK = 0x1,
};

/* RC_DEBUG_CFG */
DEFINE_BIT(TRACE_MODE_EN, 24)

/* SUBSYS_INTF_CFG */
DEFINE_BITFIELD(SRCLKEN_FPM_MASK_B, 12, 0)
DEFINE_BITFIELD(SRCLKEN_BBLPM_MASK_B, 28, 16)

enum {
	PMIC_PMRC_CON0 = 0x1A6,
	PMIC_PMRC_CON0_SET = 0x1A8,
	PMIC_PMRC_CON0_CLR = 0x1AA,
};

enum chn_id {
	CHN_SUSPEND = 0,
	CHN_RF = 1,
	CHN_DEEPIDLE = 2,
	CHN_MD = 3,
	CHN_GPS = 4,
	CHN_BT = 5,
	CHN_WIFI = 6,
	CHN_MCU = 7,
	CHN_COANT = 8,
	CHN_NFC = 9,
	CHN_UFS = 10,
	CHN_SCP = 11,
	CHN_RESERVE = 12,
	MAX_CHN_NUM,
};

enum {
	SRCLKENAO_MODE,
	VREQ_MODE,
};

enum {
	MERGE_OR_MODE		= 0x0,
	BYPASS_MODE		= 0x1,
	MERGE_AND_MODE		= 0x1 << 1,
	BYPASS_RC_MODE		= 0x2 << 1,
	BYPASS_OTHER_MODE	= 0x3 << 1,
	ASYNC_MODE		= 0x1 << 3,
};

enum {
	RC_32K = 0,
	RC_ULPOSC1,
};

enum rc_ctrl_m {
	HW_MODE = 0,
	SW_MODE = 1,
	INIT_MODE = 0xff,
};

enum rc_support {
	SRCLKEN_RC_ENABLE = 0,
	SRCLKEN_RC_DISABLE,
};

struct subsys_rc_con {
	unsigned int dcxo_prd;
	unsigned int xo_prd;
	unsigned int cnt_step;
	unsigned int track_en;
	unsigned int req_ack_imd_en;
	unsigned int xo_soc_link_en;
	unsigned int sw_bblpm;
	unsigned int sw_fpm;
	unsigned int sw_rc;
	unsigned int bypass_cmd;
	unsigned int dcxo_settle_blk_en;
};

extern int srclken_rc_init(void);

#endif /* SOC_MEDIATEK_MT8192_SRCLKEN_RC_H */
