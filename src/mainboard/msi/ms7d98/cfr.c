/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

static const struct sm_object tdp_pl1_override = SM_DECLARE_NUMBER({
	.opt_name       = "tdp_pl1_override",
	.ui_name        = "CPU Power Limit 1",
	.ui_helptext    = "Long-duration CPU package power limit (in Watts)",
	.min            = 35,
	.max            = 400,
	.step           = 1,
	.display_flags  = 0,
});

static const struct sm_object tdp_pl2_override = SM_DECLARE_NUMBER({
	.opt_name       = "tdp_pl2_override",
	.ui_name        = "CPU Power Limit 2",
	.ui_helptext    = "Short-duration CPU package power limit (in Watts)",
	.min            = 35,
	.max            = 500,
	.step           = 1,
	.display_flags  = 0,
});

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

static struct sm_obj_form security = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		&intel_tme,
		NULL
	},
};

static struct sm_obj_form igpu = {
	.ui_name = "Integrated Graphics",
	.obj_list = (const struct sm_object *[]) {
		&igd_enabled,
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
		&tdp_pl1_override,
		&tdp_pl2_override,
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
	&security,
	&igpu,
	&performance,
	&power,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
