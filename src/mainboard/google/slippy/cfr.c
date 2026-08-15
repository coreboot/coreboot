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
	.flags		= !CONFIG(BOARD_GOOGLE_PEPPY) ? CFR_OPTFLAG_SUPPRESS : 0,
});

static const struct sm_object lte_ngff = SM_DECLARE_BOOL({
	.opt_name	= "lte_ngff",
	.ui_name	= "LTE / NGFF slot",
	.ui_helptext	= "Enable PCIe RP2 and ACPI for the LTE/NGFF slot.\n"
			  "Only for boards with the NGFF connector (or equivalent) fitted.",
	.default_value	= false,
	.flags		= !CONFIG(BOARD_GOOGLE_PEPPY) ? CFR_OPTFLAG_SUPPRESS : 0,
});

static const struct sm_object ambient_light = SM_DECLARE_BOOL({
	.opt_name	= "ambient_light",
	.ui_name	= "Ambient light sensor",
	.ui_helptext	= "Enable ACPI/SMBIOS for the ISL29018 on I2C1.\n"
			  "Only for boards that have the sensor fitted.",
	.default_value	= false,
	.flags		= !CONFIG(BOARD_GOOGLE_PEPPY) ? CFR_OPTFLAG_SUPPRESS : 0,
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
		&lte_ngff,
		&ambient_light,
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
