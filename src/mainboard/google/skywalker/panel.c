/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>

#include <gpio.h>
#include <soc/display.h>
#include <soc/regulator.h>

#include "gpio.h"
#include "panel.h"

static void aw37503_init(unsigned int bus)
{
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x00, 0x13, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x01, 0x13, 0x1F, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x4C, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x03, 0x43, 0xFF, 0);
	i2c_write_field(bus, PMIC_AW37503_SLAVE, 0x21, 0x00, 0xFF, 0);
}

static bool is_pmic_aw37503(unsigned int bus)
{
	u8 vendor_id;
	return (!i2c_read_field(bus, PMIC_AW37503_SLAVE,
				0x04, &vendor_id, 0x0F, 0) && vendor_id == 0x01);
}

/* Set up backlight control pins as output pins, and set them to power off by default */
void panel_configure_backlight(void)
{
	gpio_output(GPIO_EDP_BL_EN_1V8, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
}

static void power_on_panel(void)
{
	gpio_output(GPIO_EN_PP3300_EDP_X, 1);
	gpio_set_mode(GPIO_EDP_HPD_1V8, GPIO_FUNC(EDP_TX_HPD, EDP_TX_HPD));
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_DISABLE, GPIO_PULL_UP);
}

void mipi_panel_power_on(void)
{
	struct aw37503_config config = {
		.i2c_bus = PMIC_I2C_BUS,
		.en = GPIO_TCHSCR_REPORT_DISABLE,
	};
	gpio_output(config.en, 0);
	mdelay(1);

	mainboard_set_regulator_voltage(MTK_REGULATOR_VCN18, 1800000);
	mtk_i2c_bus_init(config.i2c_bus, I2C_SPEED_FAST);

	mainboard_enable_regulator(MTK_REGULATOR_VCN18, true);
	mdelay(5);

	if (is_pmic_aw37503(config.i2c_bus)) {
		printk(BIOS_DEBUG, "Initialize and power on PMIC AW37503\n");
		aw37503_init(config.i2c_bus);
		gpio_output(config.en, 1);
		mdelay(10);
	}
	gpio_output(GPIO_EN_PP3300_EDP_X, 0);
	mdelay(1);
	gpio_output(GPIO_EN_PP3300_EDP_X, 1);
	mdelay(1);
	gpio_output(GPIO_TCHSCR_RST_1V8_L, 0);
	mdelay(1);
	gpio_output(GPIO_TCHSCR_RST_1V8_L, 1);
	mdelay(6);
}

static struct panel_description panel = {
	.configure_backlight = panel_configure_backlight,
	.power_on = power_on_panel,
	.disp_path = DISP_PATH_EDP,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

struct panel_description *get_active_panel(void)
{
	if (CONFIG(BOARD_GOOGLE_PADME))
		return get_panel_description();

	return &panel;
}
