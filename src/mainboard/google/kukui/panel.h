/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Huaqin Telecom Inc.
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

#ifndef __MAINBOARD_GOOGLE_KUKUI_PANEL_H__
#define __MAINBOARD_GOOGLE_KUKUI_PANEL_H__

#include <edid.h>
#include <soc/dsi.h>

struct panel_description {
	struct edid edid;  /* edid info of this panel */
	enum lb_fb_orientation orientation;  /* panel orientation */
	void (*power_on)(void);  /* Callback to turn on panel */
	u8 init[]; /* a packed array of lcm_init_command */
};

/* Returns the panel description from given ID. */
extern struct panel_description *get_panel_description(int panel_id);

#define INIT_DCS_CMD(...) \
	LCM_DCS_CMD, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define INIT_GENERIC_CMD(...) \
	LCM_GENERIC_CMD, \
	sizeof((u8[]){__VA_ARGS__}), \
	__VA_ARGS__

#define INIT_DELAY_CMD(delay) \
	LCM_DELAY_CMD, \
	delay

#define INIT_END_CMD \
	LCM_END_CMD

/* GPIO names */
#define GPIO_LCM_RST_1V8		GPIO(LCM_RST)		/* 45 */
#define GPIO_MIPIBRDG_PWRDN_L_1V8	GPIO(LCM_RST)		/* 45 */
#define GPIO_MIPIBRDG_RST_L_1V8		GPIO(BPI_BUS3)		/* 73 */
#define GPIO_PP1200_MIPIBRDG_EN		GPIO(BPI_OLAT1)		/* 54 */
#define GPIO_PP1800_LCM_EN		GPIO(SIM2_SRST)		/* 36 */
#define GPIO_PP3300_LCM_EN		GPIO(SIM2_SIO)		/* 35 */
#define GPIO_PPVARN_LCD_EN		GPIO(PERIPHERAL_EN9)	/* 166 */
#define GPIO_PPVARP_LCD_EN		GPIO(MISC_BSI_CK_3)	/* 66 */
#define GPIO_VDDIO_MIPIBRDG_EN		GPIO(SIM2_SCLK)		/* 37 */

#endif /* __MAINBOARD_GOOGLE_KUKUI_PANEL_H__ */
