/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8195_MT6360_H__
#define __SOC_MEDIATEK_MT8195_MT6360_H__

#include <stdint.h>

#define MT6360_LDO_I2C_ADDR	0x64
#define MT6360_PMIC_I2C_ADDR	0x1a

#define MT6360_DATA(_enreg, _enmask, _vreg, _vmask, _table)	\
{								\
	.enable_reg = _enreg,					\
	.enable_mask = _enmask,					\
	.vsel_reg = _vreg,					\
	.vsel_mask = _vmask,					\
	.vsel_table = _table,					\
	.vsel_table_len = ARRAY_SIZE(_table),			\
}

enum {
	MT6360_INDEX_LDO = 0,
	MT6360_INDEX_PMIC,
	MT6360_INDEX_COUNT,
};

/*
 * This must match the regulator IDs defined in EC's BC1.2 MT6360 driver.
 * Please do NOT change the order.
 */
enum mt6360_regulator_id {
	MT6360_LDO3 = 0,
	MT6360_LDO5,
	MT6360_LDO6,
	MT6360_LDO7,
	MT6360_BUCK1,
	MT6360_BUCK2,
	MT6360_LDO1,
	MT6360_LDO2,
	MT6360_REGULATOR_COUNT,
};

struct mt6360_i2c_data {
	u8 bus;
	u8 addr;
};

struct mt6360_data {
	uint8_t enable_reg;
	uint8_t enable_mask;
	uint8_t vsel_reg;
	uint8_t vsel_mask;
	const uint32_t *vsel_table;
	uint32_t vsel_table_len;
};

void mt6360_init(uint8_t bus);
void mt6360_enable(enum mt6360_regulator_id id, uint8_t enable);
uint8_t mt6360_is_enabled(enum mt6360_regulator_id id);
void mt6360_set_voltage(enum mt6360_regulator_id id, u32 voltage_uv);
u32 mt6360_get_voltage(enum mt6360_regulator_id id);

#endif
