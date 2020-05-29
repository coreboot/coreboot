/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT6359P_H__
#define __SOC_MEDIATEK_MT6359P_H__

#include <types.h>

enum {
	PMIC_HWCID		= 0x0008,
	PMIC_SWCID		= 0x000a,
	PMIC_TOP_RST_MISC_SET	= 0x014c,
	PMIC_TOP_RST_MISC_CLR	= 0x014e,
	PMIC_PWRHOLD		= 0x0a08,
	PMIC_VGPU11_DBG0	= 0x15a6,
	PMIC_VGPU11_ELR0	= 0x15b4,
	PMIC_VS2_VOTER		= 0x18aa,
	PMIC_VS2_VOTER_CFG	= 0x18b0,
	PMIC_VS2_ELR0		= 0x18b4,
	PMIC_VSRAM_PROC1_ELR	= 0x1b44,
	PMIC_VSRAM_PROC2_ELR	= 0x1b46,
	PMIC_VSRAM_PROC1_VOSEL1	= 0x1e90,
	PMIC_VSRAM_PROC2_VOSEL1	= 0x1eb0,
	PMIC_VM18_ANA_CON0	= 0x2020,
};

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6359P_GPU11 = 0,
	MT6359P_SRAM_PROC1,
	MT6359P_SRAM_PROC2,
	MT6359P_MAX,
};

#define VM18_VOL_REG_SHIFT  8
#define VM18_VOL_OFFSET 600

void mt6359p_init(void);
void mt6359p_romstage_init(void);
void mt6359p_buck_set_voltage(u32 buck_id, u32 buck_uv);
u32 mt6359p_buck_get_voltage(u32 buck_id);
void mt6359p_set_vm18_voltage(u32 vm18_uv);
u32 mt6359p_get_vm18_voltage(void);
#endif /* __SOC_MEDIATEK_MT6359P_H__ */
