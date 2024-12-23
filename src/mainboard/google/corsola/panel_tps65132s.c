/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/regulator.h>
#include <soc/tps65132s.h>

#include "gpio.h"
#include "panel.h"

void tps65132s_power_on(struct tps65132s_cfg *config)
{
	const struct tps65132s_reg_setting reg_settings[] = {
		{ PMIC_TPS65132_VPOS, 0x14, 0x1F },
		{ PMIC_TPS65132_VNEG, 0x14, 0x1F },
		{ PMIC_TPS65132_DLYX, 0x95, 0xFF },
		{ PMIC_TPS65132_ASSDD, 0x5b, 0xFF },
	};
	config->settings = reg_settings;
	config->setting_counts = ARRAY_SIZE(reg_settings);

	mainboard_set_regulator_voltage(MTK_REGULATOR_VIO18, 1800000);
	mtk_i2c_bus_init(config->i2c_bus, I2C_SPEED_FAST);

	if (is_pmic_aw37503(config->i2c_bus)) {
		printk(BIOS_DEBUG, "Initialize and power on PMIC AW37503\n");
		aw37503_init(config->i2c_bus);
		gpio_output(config->en, 1);
		mdelay(2);
		gpio_output(config->sync, 1);
		mdelay(3);
	} else if (tps65132s_setup(config) != CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to setup tps65132s\n");
	}

	/* DISP_RST_1V8_L */
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
	mdelay(1);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	udelay(20);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}
