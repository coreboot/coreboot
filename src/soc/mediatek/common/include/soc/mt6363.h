/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT6363_H__
#define __SOC_MEDIATEK_MT6363_H__

#include <soc/spmi.h>
#include <types.h>

enum {
	PMIC_HWCID		= 0x0008,
	PMIC_SWCID0		= 0x000A,
	PMIC_SWCID1		= 0x000B,
	PMIC_TOP_CKPDN_CON0	= 0x010C,
	PMIC_TOP_CKHWEN_CON0	= 0x012A,
	PMIC_TOP_RST_MISC1	= 0x0139,
	PMIC_TOP_RST_MISC1_SET	= 0x013A,
	PMIC_TOP_RST_MISC1_CLR	= 0x013B,
	PMIC_TOP_VRCTL_DBG_CON0	= 0x0231,
	PMIC_VRCTL_VOSEL_VBUCK2	= 0x024E,
	PMIC_OTP_CON0		= 0x038A,
	PMIC_OTP_CON8		= 0x039A,
	PMIC_OTP_CON11		= 0x03A0,
	PMIC_OTP_CON12		= 0x03A2,
	PMIC_OTP_CON13		= 0x03A4,
	PMIC_PWRHOLD		= 0x0A08,
	PMIC_VCORE_DBG0		= 0x1526,
	PMIC_VCORE_ELR0		= 0x152C,
	PMIC_VBUCK2_DBG0	= 0x1596,
	PMIC_VGPU11_DBG0	= 0x15A6,
	PMIC_VBUCK4_DBG0	= 0x1696,
	PMIC_VBUCK5_OP_EN_2	= 0x170F,
	PMIC_VBUCK5_DBG0	= 0x1716,
	PMIC_VS2_VOTER		= 0x18AA,
	PMIC_VS2_VOTER_CFG	= 0x18B0,
	PMIC_VS2_ELR0		= 0x18B4,
	PMIC_VPA_CON0		= 0x1908,
	PMIC_VPA_CON1		= 0x190E,
	PMIC_VPA_DBG0		= 0x1914,
	PMIC_BUCK_VPA_DLC_CON0	= 0x1918,
	PMIC_BUCK_VPA_DLC_CON1	= 0x191A,
	PMIC_VSRAM_PROC1_ELR	= 0x1B44,
	PMIC_VSRAM_PROC2_ELR	= 0x1B46,
	PMIC_LDO_VCN15_CON0	= 0x1B87,
	PMIC_LDO_VRF09_CON0	= 0x1B95,
	PMIC_LDO_VRF12_CON0	= 0x1Ba3,
	PMIC_LDO_VRF13_CON0	= 0x1BB1,
	PMIC_LDO_VRF18_CON0	= 0x1BBF,
	PMIC_LDO_VTREF18_CON0   = 0x1C07,
	PMIC_VSIM1_CON0		= 0x1CD0,
	PMIC_LDO_VSRAM_DIGRF_CON0	= 0x1D1D,
	PMIC_LDO_VSRAM_MDFE_CON0	= 0x1D87,
	PMIC_VM18_CON0		= 0x1D88,
	PMIC_VSRAM_PROC1_VOSEL1	= 0x1E90,
	PMIC_VSRAM_PROC2_VOSEL1	= 0x1EB0,
	PMIC_VTREF18_ANA_CON0	= 0x1F08,
	PMIC_VTREF18_ANA_CON1	= 0x1F09,
	PMIC_VM18_ANA_CON0	= 0x1F1F,
	PMIC_VM18_ANA_CON1	= 0x1F20,
	PMIC_VSIM1_ANA_CON0	= 0x1FA2,
};

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
};

struct pmic_efuse {
	unsigned short efuse_bit;
	unsigned short addr;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6363_VBUCK1 = 0,
	MT6363_VBUCK2,
	MT6363_VBUCK3,
	MT6363_VBUCK4,
	MT6363_VM18 = 7,
	MT6363_MAX,
};

#define EFUSE_WAIT_US		5000
#define EFUSE_BUSY		1

#define EFUSE_RG_VPA_OC_FT	78

void mt6363_init(void);
void mt6363_buck_set_voltage(u32 buck_id, u32 buck_uv);
u32 mt6363_buck_get_voltage(u32 buck_id);
void mt6363_set_vm18_voltage(u32 vm18_uv);
u32 mt6363_get_vm18_voltage(void);
void mt6363_set_vtref18_voltage(u32 vtref18_uv);
u32 mt6363_get_vtref18_voltage(void);
void mt6363_set_vsim1_voltage(u32 vsim1_uv);
u32 mt6363_get_vsim1_voltage(void);
void mt6363_enable_vpa(bool enable);
void mt6363_enable_vsim1(bool enable);
void mt6363_enable_vm18(bool enable);
void mt6363_enable_vtref18(bool enable);
void mt6363_enable_buck5(bool enable);
void mt6363_enable_vcn15(bool enable);
void mt6363_enable_vrf09(bool enable);
void mt6363_enable_vrf12(bool enable);
void mt6363_enable_vrf13(bool enable);
void mt6363_enable_vrf18(bool enable);
void mt6363_enable_vram_digrf(bool enable);
void mt6363_enable_vram_mdfe(bool enable);
void mt6363_init_pmif_arb(void);
u32 mt6363_read16(u32 reg);
void mt6363_write8(u32 reg, u8 reg_val);
void mt6363_write_field(u32 reg, u32 val, u32 mask, u32 shift);
void pmic_init_setting(void);

#endif /* __SOC_MEDIATEK_MT6363_H__ */
