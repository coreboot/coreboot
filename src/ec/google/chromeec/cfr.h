/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs which are used to control EC settings.
 */

#ifndef _CHROMEEC_CFR_H_
#define _CHROMEEC_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include "ec.h"

static void update_fan_control(struct sm_object *new)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC_AUTO_FAN_CTRL) && !google_chromeec_has_fan()) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_bool.default_value = false;
	}
}

static const struct sm_object auto_fan_control = SM_DECLARE_BOOL({
	.opt_name       = "auto_fan_control",
	.ui_name        = "Automatic Fan Control",
	.ui_helptext    = "Enable or disable automatic fan control.",
	.default_value  = CONFIG(EC_GOOGLE_CHROMEEC_AUTO_FAN_CTRL),
}, WITH_CALLBACK(update_fan_control));

static const struct sm_enum_value ec_backlight_values[] = {
	{ "0%", 0 },
	{ "25%", 25 },
	{ "50%", 50 },
	{ "100%", 100 },
	SM_ENUM_VALUE_END,
};

static void update_kb_backlight(struct sm_object *new)
{
	if (!google_chromeec_has_kbbacklight() || google_chromeec_has_rgbkbd()) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_bool.default_value = -1;
	}
}

static const struct sm_object ec_kb_backlight = SM_DECLARE_ENUM({
	.opt_name       = "ec_kb_backlight",
	.ui_name        = "Keyboard Backlight Level At Boot",
	.ui_helptext    = "Specify the level the keyboard backlight should be at boot."
			" Set to 0% to disable the backlight.",
	.default_value  = 50,
	.values         = ec_backlight_values,
}, WITH_CALLBACK(update_kb_backlight));

static const struct sm_enum_value ec_rgb_backlight_values[] = {
	{ "Off",	GOOGLE_CHROMEEC_RGBKBD_COLOR_OFF },
	{ "Red",	GOOGLE_CHROMEEC_RGBKBD_COLOR_RED },
	{ "Green",	GOOGLE_CHROMEEC_RGBKBD_COLOR_GREEN },
	{ "Blue",	GOOGLE_CHROMEEC_RGBKBD_COLOR_BLUE },
	{ "Yellow",	GOOGLE_CHROMEEC_RGBKBD_COLOR_YELLOW },
	{ "White",	GOOGLE_CHROMEEC_RGBKBD_COLOR_WHITE },
	SM_ENUM_VALUE_END,
};

static void update_rgb_kb_backlight(struct sm_object *new)
{
	if (!google_chromeec_has_rgbkbd()) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_bool.default_value = GOOGLE_CHROMEEC_RGBKBD_COLOR_OFF;
	}
}

static const struct sm_object ec_rgb_kb_color = SM_DECLARE_ENUM({
	.opt_name       = "ec_rgb_kb_color",
	.ui_name        = "RGB Keyboard Color At Boot",
	.ui_helptext    = "Select the static color applied to the RGB keyboard at boot.",
	.default_value  = GOOGLE_CHROMEEC_RGBKBD_COLOR_OFF,
	.values         = ec_rgb_backlight_values,
}, WITH_CALLBACK(update_rgb_kb_backlight));

#endif /* _CHROMEEC_CFR_H_ */
