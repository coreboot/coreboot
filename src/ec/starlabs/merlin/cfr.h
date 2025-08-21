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
	.default_value	= 0,
	.values		= (struct sm_enum_value[]) {
				{ "30 seconds",		0	},
				{ "1 minute",		1	},
				{ "3 minutes",		2	},
				{ "5 minutes",		3	},
				{ "Never",		4	},
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
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "100%",		0	},
				{ "80%",		1	},
				{ "60%",		2	},
				SM_ENUM_VALUE_END		},
});

/*
 * Fan Mode
 */
static const struct sm_object fan_mode = SM_DECLARE_ENUM({
	.opt_name	= "fan_mode",
	.ui_name	= "Fan Mode",
	.ui_helptext	= "Adjust the fan curve to prioritize performance or noise levels.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Normal",		0		},
				{ "Aggressive",		1		},
				{ "Quiet",		2		},
				{ "Disabled",		3		},
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
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "1.0C",		0	},
				{ "0.5C",		1	},
				{ "0.2C",		2	},
				SM_ENUM_VALUE_END		},
});

/*
 * Lid Switch
 */
static const struct sm_object lid_switch = SM_DECLARE_ENUM({
	.opt_name	= "lid_switch",
	.ui_name	= "Lid Switch",
	.ui_helptext	= "Configure what opening or closing the lid will do.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Normal",		0	},
				{ "Sleep Only",		1	},
				{ "Disabled",		2	},
				SM_ENUM_VALUE_END		},
});

/*
 * Power LED Brightness
 */
static const struct sm_object power_led = SM_DECLARE_ENUM({
	.opt_name	= "power_led",
	.ui_name	= "Power LED Brightness",
	.ui_helptext	= "Control the maximum brightness of the power LED",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Normal",		0		},
				{ "Reduced",		1		},
				{ "Off",		2		},
				SM_ENUM_VALUE_END,
	},
});
