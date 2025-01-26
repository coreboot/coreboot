/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/i2c.h>
#include <soc/regulator.h>

#include "gpio.h"
#include "panel.h"

#define PMIC_TPS65132_I2C	I2C3

static void power_on_mipi_himax_83102j(void)
{
	const struct tps65132s_reg_setting reg_settings[] = {
		{ PMIC_TPS65132_VPOS, 0x11, 0x1f },
		{ PMIC_TPS65132_VNEG, 0x11, 0x1f },
		{ PMIC_TPS65132_DLYX, 0x55, 0xff },
		{ PMIC_TPS65132_ASSDD, 0x5b, 0xff },
	};
	const struct tps65132s_cfg cfg = {
		.i2c_bus = PMIC_TPS65132_I2C,
		.en = GPIO_EN_PPVAR_MIPI_DISP,
		.sync = GPIO_EN_PPVAR_MIPI_DISP_150MA,
		.settings = reg_settings,
		.setting_counts = ARRAY_SIZE(reg_settings),
	};
	mainboard_set_regulator_voltage(MTK_REGULATOR_VDD18, 1900000);
	power_on_mipi_panel(&cfg);
}

static struct panel_description ciri_panels[] = {
	[17] = {
		.name = "BOE_NV110WUM_L60",
		.power_on = power_on_mipi_himax_83102j,
		.configure_backlight = configure_mipi_pwm_backlight,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
		.disp_path = DISP_PATH_MIPI,
		.pwm_ctrl_gpio = true,
	},

	[34] = {
		.name = "IVO_T109NW41",
		.power_on = power_on_mipi_himax_83102j,
		.configure_backlight = configure_mipi_pwm_backlight,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
		.disp_path = DISP_PATH_MIPI,
		.pwm_ctrl_gpio = true,
	},

	[51] = {
		.name = "CSOT_PNA957QT1_1",
		.power_on = power_on_mipi_himax_83102j,
		.configure_backlight = configure_mipi_pwm_backlight,
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
		.disp_path = DISP_PATH_MIPI,
		.pwm_ctrl_gpio = true,
	}
};

struct panel_description *get_panel_description(uint32_t panel_id)
{
	uint32_t id = panel_id & 0xFF;

	if (id >= ARRAY_SIZE(ciri_panels))
		return NULL;

	return &ciri_panels[id];
}
