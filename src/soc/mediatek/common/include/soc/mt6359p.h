/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT6359P_H__
#define __SOC_MEDIATEK_MT6359P_H__

#include <types.h>

enum {
	PMIC_HWCID		= 0x0008,
	PMIC_SWCID		= 0x000a,
	PMIC_TOP_CKPDN_CON0	= 0x010c,
	PMIC_TOP_CKHWEN_CON0	= 0x012a,
	PMIC_TOP_RST_MISC_SET	= 0x014c,
	PMIC_TOP_RST_MISC_CLR	= 0x014e,
	PMIC_OTP_CON0		= 0x038a,
	PMIC_OTP_CON8		= 0x039a,
	PMIC_OTP_CON11		= 0x03a0,
	PMIC_OTP_CON12		= 0x03a2,
	PMIC_OTP_CON13		= 0x03a4,
	PMIC_PWRHOLD		= 0x0a08,
	PMIC_VCORE_DBG0		= 0x1526,
	PMIC_VCORE_ELR0		= 0x152c,
	PMIC_VGPU11_DBG0	= 0x15a6,
	PMIC_VGPU11_ELR0	= 0x15b4,
	PMIC_VS2_VOTER		= 0x18aa,
	PMIC_VS2_VOTER_CFG	= 0x18b0,
	PMIC_VS2_ELR0		= 0x18b4,
	PMIC_VPA_CON0		= 0x1908,
	PMIC_VPA_CON1		= 0x190e,
	PMIC_VPA_DBG0		= 0x1914,
	PMIC_BUCK_VPA_DLC_CON0	= 0x1918,
	PMIC_BUCK_VPA_DLC_CON1	= 0x191a,
	PMIC_VSIM1_CON0		= 0x1cd0,
	PMIC_VSRAM_PROC1_ELR	= 0x1b44,
	PMIC_VSRAM_PROC2_ELR	= 0x1b46,
	PMIC_VSRAM_PROC1_VOSEL1	= 0x1e90,
	PMIC_VSRAM_PROC2_VOSEL1	= 0x1eb0,
	PMIC_VSIM1_ANA_CON0	= 0x1fa2,
	PMIC_VM18_ANA_CON0	= 0x2020,
};

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

struct pmic_efuse {
	unsigned short efuse_bit;
	unsigned short addr;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6359P_GPU11 = 0,
	MT6359P_SRAM_PROC1,
	MT6359P_SRAM_PROC2,
	MT6359P_CORE,
	MT6359P_PA,
	MT6359P_SIM1,
	MT6359P_MAX,
};

#define VSIM1_VOL_REG_SHIFT  8
#define VSIM1_VOL_OFFSET_1 1400
#define VSIM1_VOL_OFFSET_2 1900

#define VM18_VOL_REG_SHIFT  8
#define VM18_VOL_OFFSET 600

#define EFUSE_WAIT_US 5000
#define EFUSE_BUSY 1

#define EFUSE_RG_VPA_OC_FT 78

void mt6359p_init(void);
void mt6359p_buck_set_voltage(u32 buck_id, u32 buck_uv);
u32 mt6359p_buck_get_voltage(u32 buck_id);
void mt6359p_set_vm18_voltage(u32 vm18_uv);
u32 mt6359p_get_vm18_voltage(void);
void mt6359p_set_vsim1_voltage(u32 vsim1_uv);
u32 mt6359p_get_vsim1_voltage(void);
void mt6359p_enable_vpa(bool enable);
void mt6359p_enable_vsim1(bool enable);
void mt6359p_write_field(u32 reg, u32 val, u32 mask, u32 shift);
void pmic_init_setting(void);
void pmic_lp_setting(void);
#endif /* __SOC_MEDIATEK_MT6359P_H__ */
