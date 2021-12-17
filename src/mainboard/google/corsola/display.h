/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__
#define __MAINBOARD_GOOGLE_CORSOLA_DISPLAY_H__

#include <edid.h>

struct edp_bridge {
	void (*power_on)(void);
	int (*get_edid)(u8 i2c_bus, struct edid *edid);
	int (*post_power_on)(u8 i2c_bus, struct edid *edid);
};

int configure_display(void);

#endif
