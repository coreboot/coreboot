/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

static struct sm_obj_form platform = {
	.ui_name = "Platform",
	.obj_list = (const struct sm_object *[]) {
		&me_state,
		&me_state_counter,
		&legacy_8254_timer,
		&vtd,
		NULL
	},
};

static struct sm_obj_form igpu = {
	.ui_name = "Integrated Graphics",
	.obj_list = (const struct sm_object *[]) {
		&igd_dvmt,
		&igd_aperture,
		NULL
	},
};

static struct sm_obj_form performance = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&hyper_threading,
		&pciexp_speed,
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
	&platform,
	&igpu,
	&performance,
	&power,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
