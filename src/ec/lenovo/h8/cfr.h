/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for Lenovo H8 EC
 */

#ifndef _LENOVO_H8_CFR_H_
#define _LENOVO_H8_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include "h8.h"

/* Bluetooth */
static const struct sm_object bluetooth = SM_DECLARE_ENUM({
	.opt_name	= "bluetooth",
	.ui_name	= "Bluetooth",
	.ui_helptext	= "Enable or disable the bluetooth module",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Keyboard Backlight */
static const struct sm_object backlight = SM_DECLARE_ENUM({
	.opt_name	= "backlight",
	.ui_name	= "Keyboard Backlight",
	.ui_helptext	= "Enable or disable the keyboard backlight",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

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
static const struct sm_object uwb = SM_DECLARE_ENUM({
	.opt_name	= "uwb",
	.ui_name	= "Ultrawideband",
	.ui_helptext	= "TBD",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Volume control */
static const struct sm_object volume = SM_DECLARE_ENUM({
	.opt_name	= "volume",
	.ui_name	= "Volume",
	.ui_helptext	= "TBD",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* WLAN */
static const struct sm_object wlan = SM_DECLARE_ENUM({
	.opt_name	= "wlan",
	.ui_name	= "WLAN",
	.ui_helptext	= "Enable or disable the WLAN module",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* WWAN */
static const struct sm_object wwan = SM_DECLARE_ENUM({
	.opt_name	= "wwan",
	.ui_name	= "WWAN",
	.ui_helptext	= "Enable or disable the WWAN module",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Power Management Beeps */
static const struct sm_object pm_beeps = SM_DECLARE_ENUM({
	.opt_name	= "power_management_beeps",
	.ui_name	= "Power Management Beeps",
	.ui_helptext	= "Enable or disable power management beeps",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Low Battery Beep */
static const struct sm_object battery_beep = SM_DECLARE_ENUM({
	.opt_name	= "low_battery_beep",
	.ui_name	= "Low Battery Beep",
	.ui_helptext	= "Enable or disable low battery beep",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Fn-CTRL Swap */
static const struct sm_object fn_ctrl_swap = SM_DECLARE_ENUM({
	.opt_name	= "fn_ctrl_swap",
	.ui_name	= "Swap Fn and CTRL",
	.ui_helptext	= "Swap the left Fn and CTRL keys",
	.default_value	= CONFIG(H8_FN_CTRL_SWAP),
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Fn Lock */
static const struct sm_object sticky_fn = SM_DECLARE_ENUM({
	.opt_name	= "sticky_fn",
	.ui_name	= "Sticky Fn key",
	.ui_helptext	= "Function key acts as a toggle",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Function keys primary */
static const struct sm_object f1_to_f12_as_primary = SM_DECLARE_ENUM({
	.opt_name	= "f1_to_f12_as_primary",
	.ui_name	= "Primary Function keys",
	.ui_helptext	= "F1-F12 default act as function keys",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

#endif /* _LENOVO_H8_CFR_H_ */
