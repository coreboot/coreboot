/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs which are used to control EC settings.
 */

#include <drivers/option/cfr_frontend.h>
#include "ec.h"

/*
 * Keyboard Backlight Timeout
 */
static const struct sm_object kbl_timeout = SM_DECLARE_ENUM({
	.opt_name	= "kbl_timeout",
	.ui_name	= "Keyboard Backlight Timeout",
	.ui_helptext	= "Set the amount of time before the keyboard backlight turns off"
			  " when un-used",
	.default_value	= SEC_30,
	.values		= (struct sm_enum_value[]) {
			{ "30 seconds",		SEC_30	},
			{ "1 minute",		MIN_1	},
			{ "3 minutes",		MIN_3	},
			{ "5 minutes",		MIN_5	},
			{ "Never",		NEVER	},
			SM_ENUM_VALUE_END		},
});


/*
 * Function-Control Swap
 */
static const struct sm_object fn_ctrl_swap = SM_DECLARE_BOOL({
	.opt_name	= "fn_ctrl_swap",
	.ui_name	= "Fn Ctrl Reverse",
	.ui_helptext	= "Swap the functions of the [Fn] and [Ctrl] keys",
	.default_value	= false,
});

/*
 * Maximum Battery Charge Level
 */
static const struct sm_object max_charge = SM_DECLARE_ENUM({
	.opt_name	= "max_charge",
	.ui_name	= "Maximum Charge Level",
	.ui_helptext	= "Set the maximum level the battery will charge to.",
	.default_value	= CHARGE_100,
	.values		= (const struct sm_enum_value[]) {
			{ "100%",		CHARGE_100	},
			{ "80%",		CHARGE_80	},
			{ "60%",		CHARGE_60	},
			SM_ENUM_VALUE_END			},
});

/*
 * Fan Mode
 */
static const struct sm_object fan_mode = SM_DECLARE_ENUM({
	.opt_name	= "fan_mode",
	.ui_name	= "Fan Mode",
	.ui_helptext	= "Adjust the fan curve to prioritize performance or noise levels.",
	.default_value	= FAN_NORMAL,
	.values		= (const struct sm_enum_value[]) {
			{ "Normal",		FAN_NORMAL	},
			{ "Aggressive",		FAN_AGGRESSIVE	},
			{ "Quiet",		FAN_QUIET	},
			{ "Disabled",		FAN_DISABLED	},
			SM_ENUM_VALUE_END			},
});

/*
 * Charging Speed
 */
static const struct sm_object charging_speed = SM_DECLARE_ENUM({
	.opt_name	= "charging_speed",
	.ui_name	= "Charging Speed",
	.ui_helptext	= "Set the maximum speed to charge the battery. Charging faster"
		  " will increase heat and battery wear.",
	.default_value	= SPEED_0_5C,
	.values		= (const struct sm_enum_value[]) {
			{ "1.0C",		SPEED_1_0C	},
			{ "0.5C",		SPEED_0_5C	},
			{ "0.2C",		SPEED_0_2C	},
			SM_ENUM_VALUE_END			},
});

/*
 * Lid Switch
 */
static const struct sm_object lid_switch = SM_DECLARE_ENUM({
	.opt_name	= "lid_switch",
	.ui_name	= "Lid Switch",
	.ui_helptext	= "Configure what opening or closing the lid will do.",
	.default_value	= SWITCH_NORMAL,
	.values		= (const struct sm_enum_value[]) {
			{ "Normal",		SWITCH_NORMAL		},
			{ "Sleep Only",		SWITCH_SLEEP_ONLY	},
			{ "Disabled",		SWITCH_DISABLED		},
			SM_ENUM_VALUE_END				},
});

/*
 * Power LED Brightness
 */
static const struct sm_object power_led = SM_DECLARE_ENUM({
	.opt_name	= "power_led",
	.ui_name	= "Power LED Brightness",
	.ui_helptext	= "Control the maximum brightness of the power LED",
	.default_value	= LED_NORMAL,
	.values		= (const struct sm_enum_value[]) {
			{ "Normal",		LED_NORMAL	},
			{ "Reduced",		LED_REDUCED	},
			{ "Off",		LED_OFF		},
			SM_ENUM_VALUE_END			},
});
