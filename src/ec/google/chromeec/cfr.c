/* SPDX-License-Identifier: GPL-2.0-only */

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
	.opt_name	= "auto_fan_control",
	.ui_name	= "Automatic Fan Control",
	.ui_helptext	= "Enable or disable automatic fan control.",
	.default_value	= CONFIG(EC_GOOGLE_CHROMEEC_AUTO_FAN_CTRL),
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
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_enum.default_value = -1;
	}
}

static const struct sm_object ec_kb_backlight = SM_DECLARE_ENUM({
	.opt_name	= "ec_kb_backlight",
	.ui_name	= "Keyboard Backlight Level At Boot",
	.ui_helptext	= "Specify the level the keyboard backlight should be at boot."
			  " Set to 0% to disable the backlight.",
	.default_value	= 50,
	.values		= ec_backlight_values,
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
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_enum.default_value = GOOGLE_CHROMEEC_RGBKBD_COLOR_OFF;
	}
}

static const struct sm_object ec_rgb_kb_color = SM_DECLARE_ENUM({
	.opt_name	= "ec_rgb_kb_color",
	.ui_name	= "RGB Keyboard Color At Boot",
	.ui_helptext	= "Select the static color applied to the RGB keyboard at boot.",
	.default_value	= GOOGLE_CHROMEEC_RGBKBD_COLOR_OFF,
	.values		= ec_rgb_backlight_values,
}, WITH_CALLBACK(update_rgb_kb_backlight));

static void update_lid_shutdown(struct sm_object *new)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC_LID_SHUTDOWN)) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_bool.default_value = false;
	}
}

static const struct sm_object lid_shutdown = SM_DECLARE_BOOL({
	.opt_name	= "lid_shutdown",
	.ui_name	= "Power Off On Lid-Closed Boot",
	.ui_helptext	= "Shut down before loading the payload when the lid is closed"
			  " and no external display is attached.",
	.default_value	= CONFIG(EC_GOOGLE_CHROMEEC_LID_SHUTDOWN),
}, WITH_CALLBACK(update_lid_shutdown));

/*
 * Battery sustainer (EC_CMD_CHARGE_CONTROL v2+).
 * Master switch gates the min/max thresholds; all are hidden when the EC
 * does not support charge-control v2+.
 */
static void update_charge_limit_override(struct sm_object *new)
{
	if (!google_chromeec_charge_control_sustainer_supported())
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object ec_charge_limit_override = SM_DECLARE_BOOL({
	.opt_name	= "ec_charge_limit_override",
	.ui_name	= "Override Battery Charge Limits",
	.ui_helptext	= "When enabled, apply the custom min/max charge thresholds "
			  "below via the EC battery sustainer. When disabled, clear "
			  "any sustainer and leave charge behavior at the EC default.",
	.default_value	= false,
}, WITH_CALLBACK(update_charge_limit_override));

static void update_charge_control(struct sm_object *new)
{
	if (!google_chromeec_charge_control_sustainer_supported())
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_enum_value ec_charge_max_values[] = {
	{ "100%", 100 },
	{ "90%", 90 },
	{ "85%", 85 },
	{ "80%", 80 },
	{ "75%", 75 },
	{ "70%", 70 },
	{ "60%", 60 },
	{ "50%", 50 },
	{ "40%", 40 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object ec_max_charge = SM_DECLARE_ENUM({
	.opt_name	= "ec_max_charge",
	.ui_name	= "Battery Charge Limit (Max)",
	.ui_helptext	= "Stop charging at this battery level; the system then runs from AC.",
	.default_value	= 100,
	.values		= ec_charge_max_values,
}, WITH_DEP_VALUES(&ec_charge_limit_override, true),
WITH_CALLBACK(update_charge_control));

static const struct sm_enum_value ec_charge_min_values[] = {
	{ "0%", 0 },
	{ "10%", 10 },
	{ "20%", 20 },
	{ "30%", 30 },
	{ "40%", 40 },
	{ "50%", 50 },
	{ "60%", 60 },
	{ "70%", 70 },
	{ "75%", 75 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object ec_min_charge = SM_DECLARE_ENUM({
	.opt_name	= "ec_min_charge",
	.ui_name	= "Battery Charge Limit (Min)",
	.ui_helptext	= "Resume charging on AC once the battery drops below this level. "
			  "Must be less than the maximum charge limit.",
	.default_value	= 0,
	.values		= ec_charge_min_values,
}, WITH_DEP_VALUES(&ec_charge_limit_override, true),
WITH_CALLBACK(update_charge_control));

/*
 * Shared ChromeEC CFR menu. Options that do not apply to a given board are
 * hidden by the callbacks above (fan, keyboard backlight, RGB keyboard,
 * lid shutdown, charge limits).
 */
static const __cfr_form struct sm_obj_form chromeec = {
	.ui_name	= "ChromeEC Embedded Controller",
	.obj_list	= (const struct sm_object *[]) {
		&auto_fan_control,
		&ec_kb_backlight,
		&ec_rgb_kb_color,
		&lid_shutdown,
		&ec_charge_limit_override,
		&ec_max_charge,
		&ec_min_charge,
		NULL,
	},
};
