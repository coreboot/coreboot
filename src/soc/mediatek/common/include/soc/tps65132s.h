/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_TPS65132S_H__
#define __SOC_MEDIATEK_TPS65132S_H__

#include <commonlib/bsd/cb_err.h>
#include <gpio.h>

/* TPS65132S I2C slave address */
#define PMIC_TPS65132_SLAVE	0x3E

/* TPS65132 register address */
#define PMIC_TPS65132_VPOS	0x00
#define PMIC_TPS65132_VNEG	0x01
#define PMIC_TPS65132_DLYX	0x02
/* Sequencing at startup and shutdown Field */
#define PMIC_TPS65132_ASSDD	0x03
#define PMIC_TPS65132_CONTROL	0xFF

struct tps65132s_reg_setting {
	u8 addr;
	u8 val;
	u8 mask;
};

struct tps65132s_cfg {
	uint32_t i2c_bus;
	gpio_t en;
	gpio_t sync;
	const struct tps65132s_reg_setting *settings;
	uint32_t setting_counts;
};

enum cb_err tps65132s_setup(const struct tps65132s_cfg *cfg);

#endif /* __SOC_MEDIATEK_TPS65132S_H__ */
