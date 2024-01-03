/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <edid.h>
#include <gpio.h>
#include <identity.h>
#include <soc/gpio_common.h>
#include <string.h>

#include "gpio.h"
#include "panel.h"

struct panel_description __weak *get_panel_description(uint32_t panel_id)
{
	printk(BIOS_WARNING, "%s: %s: the panel configuration is not ready\n",
	       __func__, mainboard_part_number);
	return NULL;
}

struct panel_description *get_active_panel(void)
{
	uint32_t active_panel_id = panel_id();
	return get_panel_description(active_panel_id);
}

void configure_mipi_pwm_backlight(void)
{
	gpio_output(GPIO_AP_DISP_BKLTEN, 0);
	gpio_output(GPIO_MIPI_BL_PWM_1V8, 0);
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
