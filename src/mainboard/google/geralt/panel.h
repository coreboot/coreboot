/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_GERALT_PANEL_H__
#define __MAINBOARD_GOOGLE_GERALT_PANEL_H__

#include <boot/coreboot_tables.h>
#include <mipi/panel.h>
#include <soc/ddp.h>

struct panel_description {
	const char *name;
	struct panel_serializable_data *s;
	void (*power_on)(void);
	void (*configure_panel_backlight)(void);
	enum disp_path_sel disp_path;
	bool pwm_ctrl_gpio;
};

void fill_lp_backlight_gpios(struct lb_gpios *gpios);
uint32_t panel_id(void);
struct panel_description *get_panel_description(uint32_t panel_id);
struct panel_description *get_active_panel(void);

#endif
