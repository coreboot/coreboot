/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_RAURU_PANEL_H__
#define __MAINBOARD_GOOGLE_RAURU_PANEL_H__

#include <soc/display.h>

uint32_t panel_id(void);
void configure_backlight(bool enable);

/* Return the MIPI panel description */
struct panel_description *get_panel_description(void);

#endif /* __MAINBOARD_GOOGLE_RAURU_PANEL_H__ */
