/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_GERALT_PANEL_H__
#define __MAINBOARD_GOOGLE_GERALT_PANEL_H__

#include <soc/display.h>
#include <soc/tps65132s.h>
#include <stdbool.h>

void configure_mipi_pwm_backlight(bool enable);
void fill_lp_backlight_gpios(struct lb_gpios *gpios);
void power_on_mipi_panel(const struct tps65132s_cfg *cfg);
uint32_t panel_id(void);
struct panel_description *get_panel_description(uint32_t panel_id);

#endif
