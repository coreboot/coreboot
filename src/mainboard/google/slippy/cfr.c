/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/google/chromeec/cfr.h>
#include <southbridge/intel/lynxpoint/cfr.h>

static const struct sm_object touchpad_type = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_type",
	.ui_name	= "Touchpad Type",
	.ui_helptext	= "Select the installed touchpad type",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
		{ "Auto-detect",	0	},
		{ "Elan",		1	},
		{ "Cypress",		2	},
		SM_ENUM_VALUE_END		},
#if !CONFIG(BOARD_GOOGLE_PEPPY)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&me_disable,
		&nmi,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "ChromeEC Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&auto_fan_control,
		NULL
	},
};

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&touchpad_type,
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
