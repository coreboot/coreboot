/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8195_MT6360_H__
#define __SOC_MEDIATEK_MT8195_MT6360_H__

#include <stdint.h>

enum mt6360_ldo_id {
	MT6360_LDO1 = 0,
	MT6360_LDO2,
	MT6360_LDO3,
	MT6360_LDO5,
	MT6360_LDO_COUNT,
};

#define MT6360_LDO_I2C_ADDR	0x64
#define MT6360_PMIC_I2C_ADDR	0x1A

void mt6360_init(uint8_t bus);
void mt6360_ldo_enable(enum mt6360_ldo_id ldo_id, uint8_t enable);
uint8_t mt6360_ldo_is_enabled(enum mt6360_ldo_id ldo_id);
void mt6360_ldo_set_voltage(enum mt6360_ldo_id ldo_id, u32 voltage_uv);
u32 mt6360_ldo_get_voltage(enum mt6360_ldo_id ldo_id);
#endif
