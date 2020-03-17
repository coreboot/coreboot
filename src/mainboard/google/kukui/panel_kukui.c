/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <boardid.h>
#include <delay.h>
#include <gpio.h>

#include "panel.h"

static void power_on_ssd2858(void)
{
	gpio_output(GPIO_MIPIBRDG_PWRDN_L_1V8, 0);
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 0);
	gpio_output(GPIO_PPVARP_LCD_EN, 1);
	gpio_output(GPIO_PPVARN_LCD_EN, 1);
	gpio_output(GPIO_PP1800_LCM_EN, 1);
	gpio_output(GPIO_PP3300_LCM_EN, 1);
	gpio_output(GPIO_PP1200_MIPIBRDG_EN, 1);
	gpio_output(GPIO_VDDIO_MIPIBRDG_EN, 1);
	mdelay(20);
	gpio_output(GPIO_MIPIBRDG_PWRDN_L_1V8, 1);
	mdelay(20);
	gpio_output(GPIO_MIPIBRDG_RST_L_1V8, 1);
	mdelay(20);
}

static struct panel_description kukui_panel = {
	.name = "CMN_P097PFG_SSD2858",
	.power_on = power_on_ssd2858,
};

struct panel_description *get_panel_description(int panel_id)
{
	/* The Innolux panel before Rev2 is no longer supported. */
	if (board_id() < 2)
		return NULL;

	/* Only one panel no matter what panel_id was provided. */
	return get_panel_from_cbfs(&kukui_panel);
}
