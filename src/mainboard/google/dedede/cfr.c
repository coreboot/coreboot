/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/google/chromeec/cfr.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

static const struct sm_object touchscreen = SM_DECLARE_ENUM({
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen Type",
	.ui_helptext	= "Select the model of the integrated touchscreen device",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
		{ "Auto-select",	0		},
		{ "ELAN0001",		1		},
		{ "GTCH7503",		2		},
		{ "GDIX0000",		3		},
		{ "ELAN2513",		4		},
		{ "WDHT0002",		5		},
		SM_ENUM_VALUE_END			},
#if !CONFIG(BOARD_GOOGLE_DRAWCIA)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
});

static const struct sm_object touchpad = SM_DECLARE_ENUM({
	.opt_name	= "touchpad",
	.ui_name	= "Touchpad Type",
	.ui_helptext	= "Select the model of the integrated touchpad device",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
		{ "Auto-select",	0		},
		{ "ELAN0000",		1		},
		{ "ELAN2702",		2		},
		SM_ENUM_VALUE_END			},
#if !CONFIG(BOARD_GOOGLE_GALTIC)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&me_state,
		&me_state_counter,
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		&s0ix_enable,
		&vtd,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "ChromeEC Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&ec_kb_backlight,
		NULL
	},
};

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&touchscreen,
		&touchpad,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&devices,
	&ec,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
