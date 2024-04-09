/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/i2c.h>

#include "gpio.h"
#include "panel.h"

#define PMIC_TPS65132_I2C	I2C3

static void power_on_mipi_boe_tv110c9m_ll0(void)
{
	const struct tps65132s_reg_setting reg_settings[] = {
		{ PMIC_TPS65132_VPOS, 0x11, 0x1f },
		{ PMIC_TPS65132_VNEG, 0x11, 0x1f },
		{ PMIC_TPS65132_DLYX, 0x95, 0xff },
		{ PMIC_TPS65132_ASSDD, 0x5b, 0xff },
	};
	const struct tps65132s_cfg cfg = {
		.i2c_bus = PMIC_TPS65132_I2C,
		.en = GPIO_EN_PPVAR_MIPI_DISP,
		.sync = GPIO_EN_PPVAR_MIPI_DISP_150MA,
		.settings = reg_settings,
		.setting_counts = ARRAY_SIZE(reg_settings),
	};
	power_on_mipi_panel(&cfg);
}

static struct panel_description panels[] = {
	[1] = {
		.name = "BOE_TV110C9M_LL0",
		.power_on = power_on_mipi_boe_tv110c9m_ll0,
		.configure_backlight = configure_mipi_pwm_backlight,
		.orientation = LB_FB_ORIENTATION_BOTTOM_UP,
		.disp_path = DISP_PATH_MIPI,
		.pwm_ctrl_gpio = true,
	},
};

struct panel_description *get_panel_description(uint32_t panel_id)
{
	/* Only PANEL_ID_LOW_CHANNEL value is valid for the reference board. */
	uint32_t id = panel_id & 0xF;

	if (id >= ARRAY_SIZE(panels))
		return NULL;

	return &panels[id];
}
