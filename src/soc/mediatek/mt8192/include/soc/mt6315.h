/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT6315_H__
#define __SOC_MEDIATEK_MT6315_H__

#include <soc/spmi.h>
#include <types.h>

struct mt6315_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

enum {
	MT6315_CPU = SPMI_SLAVE_6,
	MT6315_GPU = SPMI_SLAVE_7,
	MT6315_MAX,
};

enum {
	MT6315_BUCK_1 = 0,
	MT6315_BUCK_2,
	MT6315_BUCK_3,
	MT6315_BUCK_4,
	MT6315_BUCK_max,
};

enum {
	MT6315_BUCK_TOP_ELR0    = 0x1449,
	MT6315_BUCK_TOP_ELR3    = 0x144d,
	MT6315_BUCK_VBUCK1_DBG0 = 0x1499,
	MT6315_BUCK_VBUCK1_DBG3 = 0x1599,
};

void mt6315_init(void);
void mt6315_romstage_init(void);
void mt6315_buck_set_voltage(u32 slvid, u32 buck_id, u32 buck_uv);
u32 mt6315_buck_get_voltage(u32 slvid, u32 buck_id);
#endif /* __SOC_MEDIATEK_MT6315_H__ */
