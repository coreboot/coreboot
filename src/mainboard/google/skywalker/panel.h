/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_SKYWALKER_PANEL_H__
#define __MAINBOARD_GOOGLE_SKYWALKER_PANEL_H__

#include <gpio.h>
#include <soc/display.h>
#include <soc/i2c.h>
#include <stdbool.h>

#define PMIC_AW37503_SLAVE	0x3E
#define PMIC_I2C_BUS		I2C6

struct aw37503_config {
	uint32_t i2c_bus;
	gpio_t en;
};

uint8_t panel_id(void);

void panel_configure_backlight(bool enable);
void mipi_panel_power_on(void);

/* Return the mipi panel description */
struct panel_description *get_panel_description(void);

#endif
