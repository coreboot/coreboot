/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_KUKUI_PANEL_H__
#define __MAINBOARD_GOOGLE_KUKUI_PANEL_H__

#include <mipi/panel.h>
#include <soc/dsi.h>

struct panel_description {
	const char *name;  /* Panel name for constructing CBFS file name */
	struct panel_serializable_data *s;
	void (*power_on)(void);  /* Callback to turn on panel */
	void (*post_power_on)(void);  /* Callback to run after panel is turned on */
	enum lb_fb_orientation orientation;
};

/* Returns the panel description from given ID. */
struct panel_description *get_panel_description(int panel_id);

/* Loads panel serializable data from CBFS. */
struct panel_description *get_panel_from_cbfs(struct panel_description *desc);

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
