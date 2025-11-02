/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/regulator.h>
#include <soc/tps65132s.h>
#include <soc/mt6363.h>

#include "gpio.h"
#include "panel.h"

void tps65132s_power_on(struct tps65132s_cfg *config)
{
	const struct tps65132s_reg_setting reg_settings[] = {
		{ PMIC_TPS65132_VPOS, 0x14, 0x1F },
		{ PMIC_TPS65132_VNEG, 0x14, 0x1F },
		{ PMIC_TPS65132_DLYX, 0x95, 0xFF },
		{ PMIC_TPS65132_ASSDD, 0x5B, 0xFF },
	};

	mt6363_enable_vrf18(true);
	config->settings = reg_settings;
	config->setting_counts = ARRAY_SIZE(reg_settings);

	mtk_i2c_bus_init(config->i2c_bus, I2C_SPEED_FAST);

	if (tps65132s_setup(config) != CB_SUCCESS)
		printk(BIOS_ERR, "Failed to set up tps65132s\n");

	/* DISP_RST_1V8_L */
	mdelay(10);
	gpio_output(GPIO_LCM_RST_1V8_L, 1);
	mdelay(10);
	gpio_output(GPIO_LCM_RST_1V8_L, 0);
	mdelay(10);
	gpio_output(GPIO_LCM_RST_1V8_L, 1);
}
