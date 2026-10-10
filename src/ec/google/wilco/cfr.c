/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/option/cfr_frontend.h>

#include "commands.h"

static bool charge_mode_supported(void)
{
	static int supported = -1;
	uint8_t mode;

	if (supported < 0)
		supported = !wilco_ec_get_byte_property(PID_CHARGE_MODE, &mode) &&
			    mode != CHARGE_MODE_LONG_LIFE;

	return supported;
}

static void update_charge_mode(struct sm_object *new)
{
	if (!charge_mode_supported())
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_enum_value ec_charge_mode_values[] = {
	{ "EC default",		CHARGE_MODE_EC_DEFAULT },
	{ "Standard",		CHARGE_MODE_STANDARD },
	{ "Custom",		CHARGE_MODE_CUSTOM },
	SM_ENUM_VALUE_END,
};

static const struct sm_object ec_charge_mode = SM_DECLARE_ENUM({
	.opt_name	= "ec_charge_mode",
	.ui_name	= "Battery Charge Mode",
	.ui_helptext	= "Charge mode applied to the EC at each boot. EC default leaves "
			  "the mode stored in the EC alone. Standard charges the "
			  "battery to 100%. Custom starts and stops charging at the "
			  "levels below.",
	.default_value	= CHARGE_MODE_EC_DEFAULT,
	.values		= ec_charge_mode_values,
}, WITH_CALLBACK(update_charge_mode));

static const struct sm_enum_value ec_charge_start_values[] = {
	{ "50%", 50 },
	{ "55%", 55 },
	{ "60%", 60 },
	{ "65%", 65 },
	{ "70%", 70 },
	{ "75%", 75 },
	{ "80%", 80 },
	{ "85%", 85 },
	{ "90%", 90 },
	{ "95%", 95 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object ec_charge_start = SM_DECLARE_ENUM({
	.opt_name	= "ec_charge_start",
	.ui_name	= "Battery Charge Start Level",
	.ui_helptext	= "Start charging on AC when the battery drops below this level. "
			  "Must be at least 5% below the stop level.",
	.default_value	= 60,
	.values		= ec_charge_start_values,
}, WITH_DEP_VALUES(&ec_charge_mode, CHARGE_MODE_CUSTOM),
WITH_CALLBACK(update_charge_mode));

static const struct sm_enum_value ec_charge_stop_values[] = {
	{ "100%", 100 },
	{ "95%", 95 },
	{ "90%", 90 },
	{ "85%", 85 },
	{ "80%", 80 },
	{ "75%", 75 },
	{ "70%", 70 },
	{ "65%", 65 },
	{ "60%", 60 },
	{ "55%", 55 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object ec_charge_stop = SM_DECLARE_ENUM({
	.opt_name	= "ec_charge_stop",
	.ui_name	= "Battery Charge Stop Level",
	.ui_helptext	= "Stop charging at this battery level; the system then runs from AC.",
	.default_value	= 90,
	.values		= ec_charge_stop_values,
}, WITH_DEP_VALUES(&ec_charge_mode, CHARGE_MODE_CUSTOM),
WITH_CALLBACK(update_charge_mode));

static const __cfr_form struct sm_obj_form wilco = {
	.ui_name	= "Embedded Controller",
	.obj_list	= (const struct sm_object *[]) {
		&ec_charge_mode,
		&ec_charge_start,
		&ec_charge_stop,
		NULL,
	},
};
