/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <southbridge/intel/lynxpoint/cfr.h>

static const struct sm_object touchscreen = SM_DECLARE_BOOL({
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen",
	.ui_helptext	= "Enable or disable the integrated touchscreen device",
	.default_value	= true,
#if !CONFIG(BOARD_GOOGLE_LULU)
	.flags		= CFR_OPTFLAG_SUPPRESS,
#endif
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&me_disable,
		NULL
	},
};

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&touchscreen,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&devices,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
