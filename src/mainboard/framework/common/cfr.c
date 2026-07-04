/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <console/console.h>
#include <drivers/option/cfr_frontend.h>
#include <mainboard/framework/common/board_host_command.h>
#include <mainboard/framework/common/ec.h>

static const struct sm_object ps2_emulation = SM_DECLARE_BOOL({
	.opt_name	= PS2_EMULATION_OPTION_NAME,
	.ui_name	= "PS/2 Touchpad Emulation",
	.ui_helptext	= "Let the EC emulate the touchpad as PS/2 if no "
			  "Operating System driver is detected (e.g. Windows Installer).",
	.default_value	= true,
});

static const struct sm_object standalone_mode = SM_DECLARE_BOOL({
	.opt_name	= STANDALONE_MODE_OPTION_NAME,
	.ui_name	= "EC Standalone Mode",
	.ui_helptext	= "Enable EC standalone mode, for running the mainboard "
			  "without the rest of the laptop. Skips hardware checks "
			  "for chassis open, display, battery and Input Cover",
	.default_value	= false,
});

static const struct sm_enum_value fp_led_level_values[] = {
	{ "EC default",	FP_LED_LEVEL_EC_DEFAULT },
#if !CONFIG(BOARD_FRAMEWORK_SUNFLOWER)
	{ "Auto",	FP_LED_BRIGHTNESS_AUTO },
#endif
	{ "High",	FP_LED_BRIGHTNESS_HIGH },
	{ "Medium",	FP_LED_BRIGHTNESS_MEDIUM },
	{ "Low",	FP_LED_BRIGHTNESS_LOW },
	{ "Ultra low",	FP_LED_BRIGHTNESS_ULTRA_LOW },
	SM_ENUM_VALUE_END,
};

static const struct sm_object fp_led_level = SM_DECLARE_ENUM({
	.opt_name	= FP_LED_LEVEL_OPTION_NAME,
	.ui_name	= "Power Button Brightness",
	.ui_helptext	= "Set the brightness of the power button LED."
#if !CONFIG(BOARD_FRAMEWORK_SUNFLOWER)
			  "\"Auto\" follows the ambient light sensor.\n"
#endif
			  "\"EC default\" leaves the level as last configured in the EC.",
	.default_value	= FP_LED_LEVEL_EC_DEFAULT,
	.values		= fp_led_level_values,
});

static struct sm_obj_form debug = {
	.ui_name = "Debug",
	.obj_list = (const struct sm_object *[]) {
		&debug_level,
		NULL
	},
};

static const struct sm_object battery_charge_limit = SM_DECLARE_NUMBER({
	.opt_name	= BATTERY_CHARGE_LIMIT_OPTION_NAME,
	.ui_name	= "Battery Charge Limit (%)",
	.ui_helptext	= "Maximum battery charge level, to extend battery lifespan.\n"
			  "Range: 50-100%. Charging stops at the selected percentage; "
			  "100 disables the limit and charges the battery fully.\n"
			  "0 keeps the limit as last configured in the EC, e.g. one set "
			  "at runtime via the OS, so a reboot doesn't override it.",
	.default_value	= BATTERY_CHARGE_LIMIT_EC_DEFAULT,
	.min		= 0,
	.max		= 100,
	.step		= 5,
});

#if CONFIG(FRAMEWORK_INPUT_DECK)
static const struct sm_enum_value input_deck_mode_values[] = {
	{ "Require Modules",	INPUT_DECK_MODE_AUTO },
	{ "Force off",		INPUT_DECK_MODE_FORCE_OFF },
	{ "Force on",		INPUT_DECK_MODE_FORCE_ON },
	SM_ENUM_VALUE_END,
};

static const struct sm_object input_deck_mode = SM_DECLARE_ENUM({
	.opt_name	= INPUT_DECK_MODE_OPTION_NAME,
	.ui_name	= "Input Deck Mode",
	.ui_helptext	= "Require Modules will check if all input modules are correctly "
			  "installed before enabling power.",
	.default_value	= INPUT_DECK_MODE_AUTO,
	.values		= input_deck_mode_values,
});
#endif

#if CONFIG(FRAMEWORK_TOUCHSCREEN_STYLUS)
static const struct sm_enum_value stylus_protocol_values[] = {
	{ "MPP 2.0",	STYLUS_PROTOCOL_MPP },
	{ "USI 2.0",	STYLUS_PROTOCOL_USI },
	SM_ENUM_VALUE_END,
};

static const struct sm_object stylus_protocol = SM_DECLARE_ENUM({
	.opt_name	= STYLUS_PROTOCOL_OPTION_NAME,
	.ui_name	= "Touchscreen Stylus Protocol",
	.ui_helptext	= "Use this setting to change the touchscreen stylus protocol "
			  "to match what your stylus supports.\n\n"
			  "Framework Stylus supports both protocols.\n\n"
			  "Modern Microsoft Surface compatible styluses support MPP.\n"
			  "Modern Chromebook compatible styluses support USI.",
	.default_value	= STYLUS_PROTOCOL_MPP,
	.values		= stylus_protocol_values,
});
#endif

static struct sm_obj_form ec = {
	.ui_name = "Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&ps2_emulation,
		&standalone_mode,
		&fp_led_level,
		&battery_charge_limit,
#if CONFIG(FRAMEWORK_INPUT_DECK)
		&input_deck_mode,
#endif
#if CONFIG(FRAMEWORK_TOUCHSCREEN_STYLUS)
		&stylus_protocol,
#endif
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&debug,
	&ec,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
