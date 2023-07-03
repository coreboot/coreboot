/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/regulator.h>
#include <soc/tps65132s.h>

#include "display.h"
#include "gpio.h"

static void mipi_panel_power_on(void)
{
	const struct tps65132s_reg_setting reg_settings[] = {
		{ PMIC_TPS65132_VPOS, 0x14, 0x1F },
		{ PMIC_TPS65132_VNEG, 0x14, 0x1F },
		{ PMIC_TPS65132_DLYX, 0x95, 0xFF },
		{ PMIC_TPS65132_ASSDD, 0x5b, 0xFF },
	};
	const struct tps65132s_cfg cfg = {
		.i2c_bus = PMIC_I2C_BUS,
		.en = GPIO_EN_PP3300_DISP_X,
		.sync = GPIO_EN_PP3300_SDBRDG_X,
		.settings = reg_settings,
		.setting_counts = ARRAY_SIZE(reg_settings),
	};

	mainboard_set_regulator_voltage(MTK_REGULATOR_VIO18, 1800000);
	mtk_i2c_bus_init(PMIC_I2C_BUS, I2C_SPEED_FAST);

	if (is_pmic_aw37503(PMIC_I2C_BUS)) {
		printk(BIOS_DEBUG, "Initialize and power on PMIC AW37503\n");
		aw37503_init(PMIC_I2C_BUS);
		gpio_output(GPIO_EN_PP3300_DISP_X, 1);
		mdelay(2);
		gpio_output(GPIO_EN_PP3300_SDBRDG_X, 1);
	} else if (tps65132s_setup(&cfg) != CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to setup tps65132s\n");
	}

	/* DISP_RST_1V8_L */
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
	mdelay(1);
	gpio_output(GPIO_EDPBRDG_RST_L, 0);
	udelay(20);
	gpio_output(GPIO_EDPBRDG_RST_L, 1);
}

static struct panel_description starmie_panels[] = {
	/* K&D panel vendor and ILI9882T chip,
	   K&D and STA panel are identical except manufacturer_name. */
	[3] = {
		.power_on = mipi_panel_power_on,
		.name = "STA_ILI9882T",
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
	/* STA panel vendor and ILI9882T chip */
	[8] = {
		.power_on = mipi_panel_power_on,
		.name = "STA_ILI9882T",
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
	/* STA panel vendor and HIMAX83102_J02 chip */
	[10] = {
		.power_on = mipi_panel_power_on,
		.name = "STA_HIMAX83102_J02",
		.orientation = LB_FB_ORIENTATION_LEFT_UP,
	},
};

struct panel_description *get_panel_description(void)
{
	uint32_t id = panel_id() & 0xF;
	if (id >= ARRAY_SIZE(starmie_panels))
		return NULL;

	return get_panel_from_cbfs(&starmie_panels[id]);
}
