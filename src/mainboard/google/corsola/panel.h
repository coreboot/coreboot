/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__
#define __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__

#include <soc/display.h>
#include <soc/i2c.h>
#include <soc/tps65132s.h>

#define BRIDGE_I2C		I2C0
#define PMIC_AW37503_SLAVE	0x3E
#define PMIC_I2C_BUS		I2C6

void aw37503_init(unsigned int bus);
bool is_pmic_aw37503(unsigned int bus);
uint32_t panel_id(void);
void backlight_control(void);
void tps65132s_power_on(struct tps65132s_cfg *config);

/* Return the mipi panel description from given panel id */
struct panel_description *get_panel_description(void);

/* Return the ANX7625 bridge description */
struct panel_description *get_anx7625_description(void);

/* Return the PS8640 bridge description */
struct panel_description *get_ps8640_description(void);

#endif
