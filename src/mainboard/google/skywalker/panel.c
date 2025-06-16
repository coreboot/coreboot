/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <gpio.h>
#include <soc/display.h>

#include "gpio.h"

/* Set up backlight control pins as output pins, and set them to power off by default */
static void configure_backlight(void)
{
	gpio_output(GPIO_AP_EDP_BKLTEN, 0);
	gpio_output(GPIO_BL_PWM_1V8, 0);
}

static void power_on_panel(void)
{
	gpio_output(GPIO_EN_PP3300_EDP_X, 1);
	gpio_set_mode(GPIO_EDP_HPD_1V8, GPIO_FUNC(EDP_TX_HPD, EDP_TX_HPD));
	gpio_set_pull(GPIO_EDP_HPD_1V8, GPIO_PULL_DISABLE, GPIO_PULL_UP);
}

static struct panel_description panel = {
	.configure_backlight = configure_backlight,
	.power_on = power_on_panel,
	.disp_path = DISP_PATH_EDP,
	.orientation = LB_FB_ORIENTATION_NORMAL,
};

struct panel_description *get_active_panel(void)
{
	return &panel;
}
