/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/regulator.h>
#include <soc/tps65132s.h>
#include <soc/pmif.h>
#include <string.h>

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

	/* Enable VM18V */
	mainboard_enable_regulator(MTK_REGULATOR_VDD18, true);
	mtk_i2c_bus_init(PMIC_TPS65132_I2C, I2C_SPEED_FAST);
	mdelay(10);
	if (tps65132s_setup(&cfg) != CB_SUCCESS)
		printk(BIOS_ERR, "Failed to set up voltage regulator tps65132s\n");
	gpio_output(GPIO_DISP_RST_1V8_L, 0);
	mdelay(1);
	gpio_output(GPIO_DISP_RST_1V8_L, 1);
	mdelay(1);
	gpio_output(GPIO_DISP_RST_1V8_L, 0);
	mdelay(1);
	gpio_output(GPIO_DISP_RST_1V8_L, 1);
	mdelay(6);
}

static struct panel_description panels[] = {
	[1] = {
		.name = "BOE_TV110C9M_LL0",
		.power_on = power_on_mipi_boe_tv110c9m_ll0,
		.configure_panel_backlight = configure_mipi_pwm_backlight,
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
