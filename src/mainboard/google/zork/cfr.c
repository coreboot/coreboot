/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/google/chromeec/cfr.h>

static struct sm_obj_form ec = {
	.ui_name = "ChromeEC Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&auto_fan_control
		&ec_kb_backlight,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&ec,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
