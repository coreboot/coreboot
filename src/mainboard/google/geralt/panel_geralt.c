/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <gpio.h>
#include <soc/gpio_common.h>
#include <string.h>

#include "gpio.h"
#include "panel.h"

static void configure_mipi_pwm_backlight(void)
{
	gpio_output(GPIO_AP_DISP_BKLTEN, 0);
	gpio_output(GPIO_MIPI_PANEL_BL_PWM, 0);
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
	},
	[11] = {
		.name = "MUTTO_B152731E1",
		.power_on = power_on_edp_mutto_b152731e1,
		.configure_panel_backlight = configure_edp_aux_backlight,
		.disp_path = DISP_PATH_EDP,
	},
};

struct panel_description *get_panel_description(uint32_t panel_id)
{
	if (panel_id >= ARRAY_SIZE(panels))
		return NULL;

	return &panels[panel_id];
}
