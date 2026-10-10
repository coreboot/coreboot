/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <baseboard/gpio.h>
#include <baseboard/panel.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <soc/pwm.h>
#include <variants.h>

void configure_backlight(struct panel_description *panel, bool enable)
{
	if (!enable) {
		gpio_output(GPIO_AP_EDP_BKLTEN, 0);
		gpio_output(GPIO_BL_PWM_1V8, 0);
		mtk_disp_pwm_set_en(false);
		return;
	}

	/*
	 * Switch BL_PWM_1V8 to DISP_PWM before asserting BKLTEN; it defaults
	 * to GPIO-high (100%) and would drop to the PWM duty at kernel
	 * pinmux time, causing a visible flash.
	 *
	 * Panels without kernel backlight data keep BL_PWM_1V8 as a plain GPIO;
	 * DISP_PWM is only flagged active so the kernel can detect it.
	 */
	if (panel->backlight.max &&
	    !mtk_disp_pwm_config(panel->backlight.period_ns, panel->backlight.level,
				 panel->backlight.max))
		gpio_set_mode(GPIO_BL_PWM_1V8, GPIO_FUNC(DISP_PWM, DISP_PWM));
	else
		gpio_output(GPIO_BL_PWM_1V8, 1);

	mtk_disp_pwm_set_en(true);
	gpio_output(GPIO_AP_EDP_BKLTEN, 1);
}

static void power_on_panel(void)
{
	gpio_set_mode(GPIO_EDP_HPD_1V8, GPIO_FUNC(EINT13, EDP_TX_HPD));
}

static struct panel_description rauru_panel = {
	.configure_backlight = configure_backlight,
	.power_on = power_on_panel,
	.disp_path = DISP_PATH_EDP,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

static void power_on_load_switch(void)
{
	gpio_output(GPIO_EN_PP3300_EDP_X, 1);
	gpio_set_mode(GPIO_EDP_HPD_1V8, GPIO_FUNC(EINT13, EDP_TX_HPD));
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_ENABLE, GPIO_PULL_UP);
}

static struct panel_description edp_panel = {
	.configure_backlight = configure_backlight,
	.power_on = power_on_load_switch,
	.disp_path = DISP_PATH_EDP,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

__weak void fw_config_panel_override(struct panel_description *panel)
{
}

struct panel_description *get_active_panel(void)
{
	if (CONFIG(BOARD_GOOGLE_SAPPHIRE))
		return get_panel_description();

	if (CONFIG(BOARD_GOOGLE_RAURU))
		return &rauru_panel;

	fw_config_panel_override(&edp_panel);

	return &edp_panel;
}
