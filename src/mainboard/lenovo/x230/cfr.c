/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/lenovo/h8/cfr.h>
#include <ec/lenovo/pmh7/cfr.h>
#include <northbridge/intel/sandybridge/cfr.h>
#include <southbridge/intel/bd82x6x/cfr.h>

static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading",
	.ui_helptext	= "Enable or disable Hyper-Threading",
	.default_value	= true,
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&sata_mode,
		&gfx_uma_size,
		&hyper_threading,
		&me_state,
		&nmi,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		&first_battery,
		&usb_always_on,
		NULL
	},
};

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&wlan,
		&wwan,
		NULL
	},
};

static struct sm_obj_form hid = {
	.ui_name = "Keyboard/Mouse",
	.obj_list = (const struct sm_object *[]) {
		&trackpoint,
		&touchpad,
		&backlight,
		&fn_ctrl_swap,
		&sticky_fn,
		&f1_to_f12_as_primary,
		NULL
	},
};

static struct sm_obj_form misc = {
	.ui_name = "Other",
	.obj_list = (const struct sm_object *[]) {
		&volume,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&power,
	&devices,
	&hid,
	&misc,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
