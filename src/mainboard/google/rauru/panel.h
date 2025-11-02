/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_RAURU_PANEL_H__
#define __MAINBOARD_GOOGLE_RAURU_PANEL_H__

#include <gpio.h>
#include <soc/display.h>
#include <soc/i2c.h>
#include <soc/tps65132s.h>

#define PMIC_TRS65132S_SLAVE	0x3e
#define PMIC_I2C_BUS		I2C7

struct tps65132s_config {
	uint32_t i2c_bus;
	gpio_t en;
};

uint32_t panel_id(void);
void configure_backlight(bool enable);
void tps65132s_power_on(struct tps65132s_cfg *config);

/* Return the MIPI panel description */
struct panel_description *get_panel_description(void);

#endif /* __MAINBOARD_GOOGLE_RAURU_PANEL_H__ */
