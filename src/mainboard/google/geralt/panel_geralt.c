/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <soc/gpio_common.h>
#include <string.h>

#include "gpio.h"
#include "panel.h"

static void configure_mipi_pwm_backlight(void)
{
	gpio_output(GPIO_AP_DISP_BKLTEN, 0);
	gpio_output(GPIO_MIPI_BL_PWM_1V8, 0);
}

static void configure_edp_aux_backlight(void)
{
	/* TODO: Add edp aux backlight for MUTTO_B152731E1 when we get MUTTO_B152731E1 */
}

static void power_on_mipi_boe_nv110c9m_l60(void)
{
	/* TODO: Add the poweron for BOE_NV110C9M_L60 when we get BOE_NV110C9M_L60 */
}

static void power_on_edp_mutto_b152731e1(void)
{
	/* TODO: Add the poweron for MUTTO_B152731E1 when we get MUTTO_B152731E1 */
}

static struct panel_description panels[] = {
	[1] = {
		.name = "BOE_NV110C9M_L60",
		.power_on = power_on_mipi_boe_nv110c9m_l60,
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
