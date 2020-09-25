/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_KUKUI_PANEL_H__
#define __MAINBOARD_GOOGLE_KUKUI_PANEL_H__

#include <edid.h>
#include <soc/dsi.h>

/*
 * The data that to be serialized and put into CBFS.
 * Note some fields, for example edid.mode.name, were actually pointers and
 * cannot be really serialized.
 */
struct panel_serializable_data {
	struct edid edid;  /* edid info of this panel */
	enum lb_fb_orientation orientation;  /* Panel orientation */
	u8 init[]; /* A packed array of lcm_init_command */
};

struct panel_description {
	const char *name;  /* Panel name for constructing CBFS file name */
	struct panel_serializable_data *s;
	void (*power_on)(void);  /* Callback to turn on panel */
	void (*post_power_on)(void);  /* Callback to run after panel is turned on */
};

/* Returns the panel description from given ID. */
struct panel_description *get_panel_description(int panel_id);

/* Loads panel serializable data from CBFS. */
struct panel_description *get_panel_from_cbfs(struct panel_description *desc);

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
