/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <fw_config.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dsi.h>
#include <variants.h>

#include "gpio.h"
#include "panel.h"

/* Set up backlight control pins as output pin and power-off by default */
static void configure_backlight(void)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
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
	if (CONFIG(BOARD_GOOGLE_RAURU))
		return &rauru_panel;

	fw_config_panel_override(&edp_panel);

	return &edp_panel;
}
