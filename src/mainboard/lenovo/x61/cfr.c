/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/lenovo/h8/cfr.h>
#include <ec/lenovo/pmh7/cfr.h>
#include <northbridge/intel/gm965/cfr.h>
#include <southbridge/intel/i82801hx/cfr.h>

static const struct sm_object boot_option = SM_DECLARE_ENUM({
	.opt_name	= "boot_option",
	.ui_name	= "Boot Option",
	.ui_helptext	= "Select the firmware slot to boot.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Fallback", 0		},
				{ "Normal",   1		},
				SM_ENUM_VALUE_END	},
});

static struct sm_obj_form graphics = {
	.ui_name = "Graphics",
	.obj_list = (const struct sm_object *[]) {
		&gfx_uma_size,
		NULL
	},
};

static struct sm_obj_form storage = {
	.ui_name = "Storage",
	.obj_list = (const struct sm_object *[]) {
		&sata_mode,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&first_battery,
		&bluetooth,
		&wlan,
		&wwan,
		&pm_beeps,
		&battery_beep,
		&sticky_fn,
		&volume,
		&trackpoint,
		NULL
	},
};

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&boot_option,
		&debug_level,
		&nmi,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&power,
	&graphics,
	&storage,
	&ec,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
