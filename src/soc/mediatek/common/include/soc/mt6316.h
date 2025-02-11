/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT6316_H__
#define __SOC_MEDIATEK_MT6316_H__

#include <soc/spmi.h>
#include <types.h>

struct mt6316_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6316_BUCK_1 = 0,
	MT6316_BUCK_2,
	MT6316_BUCK_3,
	MT6316_BUCK_4,
	MT6316_BUCK_MAX,
};

enum {
	MT6316_PMIC_TOP_RST_MISC	= 0x138,
	MT6316_PMIC_TOP_RST_MISC_SET	= 0x139,
	MT6316_PMIC_TOP_RST_MISC_CLR	= 0x13A,
	MT6316_PMIC_SWCID_H_ADDR	= 0x20B,
	MT6316_PMIC_TEST_CON9		= 0x222,
	MT6316_PMIC_PLT_DIG_WPK		= 0x3B1,
	MT6316_PMIC_PLT_DIG_WPK_H	= 0x3B2,
	MT6316_PMIC_SPMI_RSV1		= 0x43A,
	MT6316_BUCK_TOP_ELR0		= 0x1448,
	MT6316_BUCK_TOP_ELR1		= 0x1449,
	MT6316_BUCK_TOP_ELR2		= 0x144A,
	MT6316_BUCK_TOP_ELR3		= 0x144B,
	MT6316_BUCK_TOP_ELR4		= 0x144C,
	MT6316_BUCK_TOP_ELR5		= 0x144D,
	MT6316_BUCK_TOP_ELR6		= 0x144E,
	MT6316_BUCK_TOP_ELR7		= 0x144F,
	MT6316_BUCK_VBUCK1_DBG0		= 0x14A0,
	MT6316_BUCK_VBUCK1_DBG1		= 0x14A1,
	MT6316_BUCK_VBUCK3_DBG0		= 0x15A0,
	MT6316_BUCK_VBUCK3_DBG1		= 0x15A1,
	MT6316_BUCK_TOP_4PHASE_TOP_ANA_CON0	= 0x1688,
};

void mt6316_init(void);
void mt6316_buck_set_voltage(enum spmi_slave slvid, u32 buck_id, u32 buck_uv);
u32 mt6316_buck_get_voltage(enum spmi_slave slvid, u32 buck_id);
void mt6316_init_setting(void);
void mt6316_write_field(enum spmi_slave slvid, u32 reg, u32 val, u32 mask, u32 shift);
void mt6316_buck_enable(enum spmi_slave slvid, u32 buck_id, bool enable);
bool mt6316_buck_is_enabled(enum spmi_slave slvid, u32 buck_id);

#endif /* __SOC_MEDIATEK_MT6316_H__ */
