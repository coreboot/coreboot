/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>

struct sm_object wifi_slot_enable = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "wifi_slot_enable",
	.ui_name	= "Enable Wi-Fi card slot",
	.ui_helptext	= "Enable or disable detection of devices in the Wi-Fi card slot",
	.default_value	= true,
});

struct sm_object ssd_slot_enable = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "ssd_slot_enable",
	.ui_name	= "Enable SSD slot",
	.ui_helptext	= "Enable or disable detection of devices in the SSD slot",
	.default_value	= true,
});

struct sm_object hdd_slot_enable = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "hdd_slot_enable",
	.ui_name	= "Enable 2.5 inch disk slot",
	.ui_helptext	= "Enable or disable detection of devices in the 2.5 inch disk slot",
	.default_value	= true,
});

static struct sm_obj_form devices = {
	.ui_name	= "Devices",
	.obj_list	= (const struct sm_object *[]) {
		&wifi_slot_enable,
		&ssd_slot_enable,
		&hdd_slot_enable,
		NULL
	},
};

struct sm_object ps2_enable = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "ps2_enable",
	.ui_name	= "PS/2 controller",
	.ui_helptext	= "Enable or disable the PS/2 controller",
	.default_value	= true,
});

struct sm_object power_on_after_fail = SM_DECLARE_ENUM({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Power state after fail",
	.ui_helptext	= "State of the platform after external power is restored",
	.default_value	= CONFIG_MAINBOARD_POWER_FAILURE_STATE,
	.values		= (const struct sm_enum_value[]) {
				{ "Off",	0 },
				{ "On",		1 },
				{ "Previous",	2 },
				SM_ENUM_VALUE_END },
});

static struct sm_obj_form superio = {
	.ui_name	= "Super I/O",
	.obj_list	= (const struct sm_object *[]) {
		&ps2_enable,
		&power_on_after_fail,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&devices,
	&superio,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
