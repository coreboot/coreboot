/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT6373_H__
#define __SOC_MEDIATEK_MT6373_H__

#include <soc/spmi.h>
#include <types.h>

enum {
	MT6373_SWCID0			= 0xA,
	MT6373_SWCID1			= 0xB,
	MT6373_TOP_VRCTL_VOSEL_VBUCK0	= 0x24C,
	MT6373_TOP_VRCTL_VOSEL_VBUCK1	= 0x24D,
	MT6373_TOP_VRCTL_VOSEL_VBUCK2	= 0x24E,
	MT6373_BUCK_TOP_4PHASE_1_ANA_CON42	= 0x1A32,
	MT6373_BUCK_TOP_KEY_PROT_LO	= 0x142A,
	MT6373_BUCK_TOP_KEY_PROT_HI	= 0x142B,
	MT6373_BUCK_VBUCK0_DBG0		= 0x1496,
	MT6373_BUCK_VBUCK1_DBG0		= 0x1516,
	MT6373_BUCK_VBUCK2_DBG0		= 0x1596,
	MT6373_LDO_VCN33_3_CON0		= 0x1C23,
	MT6373_LDO_VCN33_3_CON1		= 0x1C24,
	MT6373_LDO_VANT18_CON0		= 0x1C87,
	MT6373_LDO_VANT18_CON1		= 0x1C88,
	MT6373_LDO_VMCH_CON0		= 0x1CB1,
	MT6373_LDO_VMC_CON0		= 0x1CC0,
	MT6373_LDO_VSIM1_CON0		= 0x1D31,
	MT6373_LDO_VSIM1_CON1		= 0x1D32,
	MT6373_LDO_VSIM2_CON0		= 0x1D40,
	MT6373_LDO_VSIM2_CON1		= 0x1D41,
	MT6373_VCN33_3_ANA_CON0		= 0x1E18,
	MT6373_VCN33_3_ANA_CON1		= 0x1E19,
	MT6373_VMCH_ANA_CON0		= 0x1E1C,
	MT6373_VMCH_ANA_CON1		= 0x1E1D,
	MT6373_VMC_ANA_CON0		= 0x1E24,
	MT6373_VMC_ANA_CON1		= 0x1E25,
};

struct mt6373_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

struct mt6373_efuse {
	unsigned short efuse_bit;
	unsigned short addr;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6373_VBUCK0 = 0,
	MT6373_VBUCK1,
	MT6373_VBUCK2,
	MT6373_VMC,
	MT6373_VMCH,
	MT6373_MAX,
};

void mt6373_init(void);
void mt6373_set_vmc_voltage(u32 vmc_uv);
u32 mt6373_get_vmc_voltage(void);
void mt6373_set_vmch_voltage(u32 vmch_uv);
u32 mt6373_get_vmch_voltage(void);
void mt6373_set_vcn33_3_voltage(u32 vcn33_3_uv);
void mt6373_enable_vcn33_3(bool enable);
void mt6373_enable_vmc(bool enable);
void mt6373_enable_vmch(bool enable);
void mt6373_enable_vant18(bool enable);
void mt6373_enable_vsim1(bool enable);
void mt6373_enable_vsim2(bool enable);
void mt6373_init_pmif_arb(void);
void mt6373_write_field(u32 reg, u32 val, u32 mask, u32 shift);
void mt6373_init_setting(void);
void mt6373_lp_setting(void);

#endif /* __SOC_MEDIATEK_MT6373_H__ */
