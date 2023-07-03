/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__
#define __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__

#include <edid.h>
#include <mipi/panel.h>
#include <soc/i2c.h>

#define BRIDGE_I2C		I2C0
#define PMIC_AW37503_SLAVE	0x3E
#define PMIC_I2C_BUS		I2C6

struct panel_description {
	void (*power_on)(void);	/* Callback to turn on panel */
	int (*post_power_on)(u8 i2c_bus, struct edid *edid);
	const char *name;	/* Panel name in CBFS */
	struct panel_serializable_data *s;
	enum lb_fb_orientation orientation;
};

void aw37503_init(unsigned int bus);
bool is_pmic_aw37503(unsigned int bus);
int configure_display(void);
uint32_t panel_id(void);

/* Return the mipi panel description from given panel id */
struct panel_description *get_panel_description(void);

/* Return the ANX7625 bridge description */
struct panel_description *get_anx7625_description(void);

/* Return the PS8640 bridge description */
struct panel_description *get_ps8640_description(void);

/* Load panel serializable data from CBFS */
struct panel_description *get_panel_from_cbfs(struct panel_description *desc);

#endif
