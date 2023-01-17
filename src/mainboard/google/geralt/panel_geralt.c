/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <gpio.h>
#include <soc/gpio_common.h>
#include <soc/i2c.h>
#include <soc/regulator.h>
#include <soc/pmif.h>
#include <string.h>

#include "gpio.h"
#include "panel.h"

#define PMIC_TPS65132_I2C	I2C3
#define PMIC_TPS65132_SLAVE	0x3E

static void configure_mipi_pwm_backlight(void)
{
	gpio_output(GPIO_AP_DISP_BKLTEN, 0);
	gpio_output(GPIO_MIPI_BL_PWM_1V8, 0);
}

static void configure_edp_aux_backlight(void)
{
	/* TODO: Add edp aux backlight for MUTTO_B152731E1 when we get MUTTO_B152731E1 */
}

static void power_on_mipi_boe_tv110c9m_ll0(void)
{
	/* Enable VM18V */
	mainboard_enable_regulator(MTK_REGULATOR_VDD18, true);

	/* Initialize I2C3 for PMIC TPS65132 */
	mtk_i2c_bus_init(PMIC_TPS65132_I2C, I2C_SPEED_FAST);
	mdelay(10);

	gpio_output(GPIO_DISP_RST_1V8_L, 0);
	mdelay(1);

	gpio_output(GPIO_EN_PPVAR_MIPI_DISP, 1);
	gpio_output(GPIO_EN_PPVAR_MIPI_DISP_150MA, 1);
	mdelay(10);

	/* Set AVDD = 5.7V */
	if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x00, 0x11, 0x1F) < 0)
		return;

	/* Set AVEE = -5.7V */
	if (panel_pmic_reg_mask(PMIC_TPS65132_I2C, PMIC_TPS65132_SLAVE, 0x01, 0x11, 0x1F) < 0)
		return;

	gpio_output(GPIO_DISP_RST_1V8_L, 1);
	mdelay(1);
	gpio_output(GPIO_DISP_RST_1V8_L, 0);
	mdelay(1);
	gpio_output(GPIO_DISP_RST_1V8_L, 1);
	mdelay(6);
}

static void power_on_edp_mutto_b152731e1(void)
{
	/* TODO: Add the poweron for MUTTO_B152731E1 when we get MUTTO_B152731E1 */
}

static struct panel_description panels[] = {
	[1] = {
		.name = "BOE_TV110C9M_LL0",
		.power_on = power_on_mipi_boe_tv110c9m_ll0,
		.configure_panel_backlight = configure_mipi_pwm_backlight,
		.disp_path = DISP_PATH_MIPI,
		.pwm_ctrl_gpio = true,
	},
	[11] = {
		.name = "MUTTO_B152731E1",
		.power_on = power_on_edp_mutto_b152731e1,
		.configure_panel_backlight = configure_edp_aux_backlight,
		.disp_path = DISP_PATH_EDP,
		.pwm_ctrl_gpio = false,
	},
};

struct panel_description *get_panel_description(uint32_t panel_id)
{
	if (panel_id >= ARRAY_SIZE(panels))
		return NULL;

	return &panels[panel_id];
}

void fill_lp_backlight_gpios(struct lb_gpios *gpios)
{
	struct panel_description *panel = get_active_panel();
	if (!panel || panel->disp_path == DISP_PATH_NONE)
		return;

	struct lb_gpio mipi_pwm_gpios[] = {
		{GPIO_MIPI_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
	};

	struct lb_gpio edp_pwm_gpios[] = {
		{GPIO_EDP_BL_PWM_1V8.id, ACTIVE_HIGH, -1, "PWM control"},
	};

	if (panel->pwm_ctrl_gpio) {
		/* PWM control for typical eDP and MIPI panels */
		if (panel->disp_path == DISP_PATH_MIPI)
			lb_add_gpios(gpios, mipi_pwm_gpios, ARRAY_SIZE(mipi_pwm_gpios));
		else
			lb_add_gpios(gpios, edp_pwm_gpios, ARRAY_SIZE(edp_pwm_gpios));
	}

	struct lb_gpio backlight_gpios[] = {
		{GPIO_AP_DISP_BKLTEN.id, ACTIVE_HIGH, -1, "backlight enable"},
	};

	lb_add_gpios(gpios, backlight_gpios, ARRAY_SIZE(backlight_gpios));
}
