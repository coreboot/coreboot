/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/option/cfr_frontend.h>
#include "h8.h"
#include "chip.h"

static void update_version(struct sm_object *new)
{
	char *ecfw = malloc(17);
	if (!ecfw)
		return;
	int len = h8_build_id_and_function_spec_version(ecfw, 16);
	ecfw[len] = 0;

	new->sm_varchar.default_value = ecfw;
}

static const struct sm_object fw_version = SM_DECLARE_VARCHAR({
	.opt_name	= "ec_version",
	.ui_name	= "Version",
	.ui_helptext	= "The EC firmware version",
	.default_value	= "Unknown",
}, WITH_CALLBACK(update_version));

/* Bluetooth */
static void update_bluetooth(struct sm_object *new)
{
	if (!(CONFIG(H8_SUPPORT_BT_ON_WIFI) || h8_has_bdc()))
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object bluetooth = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth",
	.ui_name	= "Bluetooth",
	.ui_helptext	= "Enable or disable the bluetooth module",
	.default_value	= true,
}, WITH_CALLBACK(update_bluetooth));

static const struct sm_enum_value kic_values_both[] = {
	{ "Both",		KIC_BOTH	},
	{ "Keyboard only",	KIC_KEYBOARD	},
	{ "ThinkLight only",	KIC_THINKLIGHT	},
	{ "None",		KIC_NONE	},
	SM_ENUM_VALUE_END,
};

static const struct sm_enum_value kic_values_thinklight_only[] = {
	{ "ThinkLight",		KIC_THINKLIGHT	},
	{ "None",		KIC_NONE	},
	SM_ENUM_VALUE_END,
};

static const struct sm_enum_value kic_values_keyboard_only[] = {
	{ "Keyboard backlight",	KIC_KEYBOARD	},
	{ "None",		KIC_NONE	},
	SM_ENUM_VALUE_END,
};

static void update_backlight(struct sm_object *new)
{
	const bool has_thinklight = h8_has_thinklight();
	const bool has_kb_backlight = h8_kb_backlight_supported();

	if (!has_thinklight && !has_kb_backlight) {
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
		return;
	}

	if (has_thinklight && has_kb_backlight) {
		new->sm_enum.values = kic_values_both;
		new->sm_enum.ui_helptext = "Select illumination device(s)";
	} else if (has_thinklight) {
		new->sm_enum.values = kic_values_thinklight_only;
		new->sm_enum.ui_helptext = "Enable or disable ThinkLight";
	} else {
		new->sm_enum.values = kic_values_keyboard_only;
		new->sm_enum.ui_helptext = "Enable or disable keyboard illumination";
	}

	new->sm_enum.default_value = h8_illumination_default();
}

/* Illumination Control (ThinkLight and/or keyboard backlight) */
static const struct sm_object backlight = SM_DECLARE_ENUM({
	.opt_name	= "backlight",
	.ui_name	= "Illumination Control",
	.ui_helptext	= "Select illumination device(s)",
	.values		= kic_values_both,
}, WITH_CALLBACK(update_backlight));

/* USB Always-On */
static const struct sm_object usb_always_on = SM_DECLARE_ENUM({
	.opt_name	= "usb_always_on",
	.ui_name	= "USB Always-on",
	.ui_helptext	= "Always keep USB ports powered",
	.default_value	= UAO_OFF,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		UAO_OFF			},
				{ "AC only",		UAO_AC_ONLY		},
				{ "AC and Battery",	UAO_AC_AND_BATTERY	},
				SM_ENUM_VALUE_END				},
});

/* Ultrawideband */
static void update_uwb(struct sm_object *new)
{
	if (!h8_has_uwb())
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object uwb = SM_DECLARE_BOOL({
	.opt_name	= "uwb",
	.ui_name	= "Ultrawideband",
	.ui_helptext	= "TBD",
	.default_value	= true,
}, WITH_CALLBACK(update_uwb));

enum {
	H8_SECONDARY_BATTERY,
	H8_PRIMARY_BATTERY,
};

/* Battery charge priority */
static const struct sm_object first_battery = SM_DECLARE_ENUM({
	.opt_name	= "first_battery",
	.ui_name	= "Battery Charge Priority",
	.ui_helptext	= "Select which battery to charge first.",
	.default_value	= H8_PRIMARY_BATTERY,
	.values		= (const struct sm_enum_value[]) {
				{ "Secondary", H8_SECONDARY_BATTERY	},
				{ "Primary",   H8_PRIMARY_BATTERY	},
				SM_ENUM_VALUE_END			},
});

/* Volume control */
static const struct sm_object volume = SM_DECLARE_NUMBER({
	.opt_name	= "volume",
	.ui_name	= "Volume",
	.ui_helptext	= "EC volume register value to restore at boot.",
	.default_value	= 3,
	.min		= 0,
	.max		= 0xff,
	.step		= 1,
	.display_flags	= CFR_NUM_OPT_DISPFLAG_HEX,
});

/* WLAN */
static const struct sm_object wlan = SM_DECLARE_BOOL({
	.opt_name	= "wlan",
	.ui_name	= "WLAN",
	.ui_helptext	= "Enable or disable the WLAN module",
	.default_value	= true,
});

/* WWAN */
static void update_wwan(struct sm_object *new)
{
	if (!h8_has_wwan())
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object wwan = SM_DECLARE_BOOL({
	.opt_name	= "wwan",
	.ui_name	= "WWAN",
	.ui_helptext	= "Enable or disable the WWAN module",
	.default_value	= true,
}, WITH_CALLBACK(update_wwan));

/* Power Management Beeps */
static void update_beep(struct sm_object *new)
{
	if (!h8_has_power_management_beeps())
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object pm_beeps = SM_DECLARE_BOOL({
	.opt_name	= "power_management_beeps",
	.ui_name	= "Power Management Beeps",
	.ui_helptext	= "Enable or disable power management beeps",
	.default_value	= true,
}, WITH_CALLBACK(update_beep));

/* Low Battery Beep */
static const struct sm_object battery_beep = SM_DECLARE_BOOL({
	.opt_name	= "low_battery_beep",
	.ui_name	= "Low Battery Beep",
	.ui_helptext	= "Enable or disable low battery beep",
	.default_value	= true,
}, WITH_CALLBACK(update_beep));

/* Fn-CTRL Swap */
static const struct sm_object fn_ctrl_swap = SM_DECLARE_BOOL({
	.opt_name	= "fn_ctrl_swap",
	.ui_name	= "Swap Fn and CTRL",
	.ui_helptext	= "Swap the left Fn and CTRL keys",
	.default_value	= CONFIG(H8_FN_CTRL_SWAP),
});

/* Fn Lock */
static const struct sm_object sticky_fn = SM_DECLARE_BOOL({
	.opt_name	= "sticky_fn",
	.ui_name	= "Sticky Fn key",
	.ui_helptext	= "Function key acts as a toggle",
	.default_value	= false,
});

/* Function keys primary */
static const struct sm_object f1_to_f12_as_primary = SM_DECLARE_BOOL({
	.opt_name	= "f1_to_f12_as_primary",
	.ui_name	= "Primary Function keys",
	.ui_helptext	= "F1-F12 default act as function keys",
	.default_value	= true,
	.flags		= CONFIG(H8_HAS_PRIMARY_FN_KEYS) ? 0 : CFR_OPTFLAG_SUPPRESS,
});

/* Touchpad - Controlled by PMH7 */
static const struct sm_object touchpad = SM_DECLARE_BOOL({
	.opt_name	= "touchpad",
	.ui_name	= "Touchpad",
	.ui_helptext	= "Enable or disable the touchpad",
	.default_value	= true,
	.flags		= CONFIG(EC_LENOVO_PMH7) ? 0 : CFR_OPTFLAG_SUPPRESS,
});

/* Trackpoint - Controlled by PMH7 */
static const struct sm_object trackpoint = SM_DECLARE_BOOL({
	.opt_name	= "trackpoint",
	.ui_name	= "Trackpoint",
	.ui_helptext	= "Enable or disable the trackpoint",
	.default_value	= true,
	.flags		= CONFIG(EC_LENOVO_PMH7) ? 0 : CFR_OPTFLAG_SUPPRESS,
});

__cfr_form static struct sm_obj_form cfr_power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&first_battery,
		&usb_always_on,
		NULL
	},
};

__cfr_form static struct sm_obj_form cfr_devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&fw_version,
		&bluetooth,
		&wlan,
		&wwan,
		&uwb,
		NULL
	},
};

__cfr_form static struct sm_obj_form cfr_hid = {
	.ui_name = "Keyboard/Mouse",
	.obj_list = (const struct sm_object *[]) {
		&backlight,
		&fn_ctrl_swap,
		&sticky_fn,
		&f1_to_f12_as_primary,
		&touchpad,
		&trackpoint,
		NULL
	},
};

__cfr_form static struct sm_obj_form cfr_misc = {
	.ui_name = "Other",
	.obj_list = (const struct sm_object *[]) {
		&volume,
		&pm_beeps,
		&battery_beep,
		NULL
	},
};
