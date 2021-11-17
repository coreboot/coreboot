/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.7
 */

#ifndef __SOC_MEDIATEK_MT6366_H__
#define __SOC_MEDIATEK_MT6366_H__

#include <types.h>

enum {
	PMIC_SWCID			= 0x000a,
	PMIC_VM_MODE			= 0x004e,
	PMIC_TOP_CKPDN_CON0_SET		= 0x010e,
	PMIC_TOP_CKPDN_CON0_CLR		= 0x0110,
	PMIC_TOP_CKHWEN_CON0_SET	= 0x012c,
	PMIC_TOP_CKHWEN_CON0_CLR	= 0x012e,
	PMIC_TOP_RST_MISC		= 0x014c,
	PMIC_TOP_RST_MISC_SET		= 0x014e,
	PMIC_TOP_RST_MISC_CLR		= 0x0150,
	PMIC_OTP_CON0			= 0x038a,
	PMIC_OTP_CON8			= 0x039a,
	PMIC_OTP_CON11			= 0x03a0,
	PMIC_OTP_CON12			= 0x03a2,
	PMIC_OTP_CON13			= 0x03a4,
	PMIC_TOP_TMA_KEY		= 0x03a8,
	PMIC_PWRHOLD			= 0x0a08,
	PMIC_CPSDSA4			= 0x0a2e,
	PMIC_VPROC12_OP_EN		= 0x1410,
	PMIC_VPROC12_DBG0		= 0x141e,
	PMIC_VPROC12_VOSEL		= 0x1426,
	PMIC_VCORE_OP_EN		= 0x1490,
	PMIC_VCORE_DBG0			= 0x149e,
	PMIC_VCORE_VOSEL		= 0x14aa,
	PMIC_VDRAM1_VOSEL_SLEEP		= 0x160a,
	PMIC_VDRAM1_OP_EN		= 0x1610,
	PMIC_VDRAM1_DBG0		= 0x161e,
	PMIC_VDRAM1_VOSEL		= 0x1626,
	PMIC_SMPS_ANA_CON0		= 0x1808,
	PMIC_VDDQ_OP_EN			= 0x1b16,
	PMIC_VSRAM_PROC12_OP_EN		= 0x1b90,
	PMIC_VSRAM_PROC12_DBG0		= 0x1ba2,
	PMIC_VSRAM_PROC12_VOSEL		= 0x1bf0,
	PMIC_LDO_VMC_CON0		= 0x1cc4,
	PMIC_LDO_VMC_OP_EN		= 0x1cc6,
	PMIC_LDO_VMCH_CON0		= 0x1cd8,
	PMIC_LDO_VMCH_OP_EN		= 0x1cda,
	PMIC_VSIM2_ANA_CON0		= 0x1e30,
	PMIC_VMCH_ANA_CON0		= 0x1e48,
	PMIC_VMC_ANA_CON0		= 0x1e4c,
	PMIC_VDDQ_ELR_0			= 0x1ec4,
};

enum mt6366_regulator_id {
	MT6366_VCORE = 0,
	MT6366_VDRAM1,
	MT6366_VDDQ,
	MT6366_VMCH,
	MT6366_VMC,
	MT6366_VPROC12,
	MT6366_VSRAM_PROC12,
	MT6366_REGULATOR_NUM,
};

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

void mt6366_init(void);
void mt6366_set_power_hold(bool enable);
void mt6366_set_vsim2_cali_mv(u32 vsim2_mv);
void mt6366_init_scp_voltage(void);
void mt6366_set_voltage(enum mt6366_regulator_id id, u32 vcore_uv);
u32 mt6366_get_voltage(enum mt6366_regulator_id id);

#endif /* __SOC_MEDIATEK_MT6366_H__ */
