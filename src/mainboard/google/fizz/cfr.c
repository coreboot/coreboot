/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/intel/oc_mailbox/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>
#include "board.h"

static const struct sm_object tdp_pl1_override = SM_DECLARE_NUMBER({
	.opt_name	= "tdp_pl1_override",
	.ui_name	= "CPU PL1 power limit (W)",
	.ui_helptext	= "Long-duration CPU package power limit.\n"
			  "Default: 15 W. Range: 15-35 W.",
	.default_value	= 15,
	.min		= 15,
	.max		= 35,
	.step		= 1,
	.display_flags	= 0,
});

static void update_tdp_pl2_default(struct sm_object *new)
{
	new->sm_number.default_value = fizz_default_pl2_watts();
}

static const struct sm_object tdp_pl2_override = SM_DECLARE_NUMBER({
	.opt_name	= "tdp_pl2_override",
	.ui_name	= "CPU PL2 power limit (W)",
	.ui_helptext	= "Short-duration CPU package power limit.\n"
			  "Default: 29 W (U22 SKUs) or 44 W (U42 SKUs). Range: 29-51 W.\n"
			  "Must be >= PL1 (enforced at boot).",
	.default_value	= 29,
	.min		= 29,
	.max		= 51,
	.step		= 1,
	.display_flags	= 0,
}, WITH_CALLBACK(update_tdp_pl2_default));

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&hyper_threading,
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&me_state,
		&me_state_counter,
		&disable_heci1_at_pre_boot,
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		&s0ix_enable,
		&vtd,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		&tdp_pl1_override,
		&tdp_pl2_override,
		&pkg_power_limit_lock,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&power,
#if CONFIG(INTEL_OC_MAILBOX_ENABLE_UNDERVOLTING)
	&cpu_voltage,
#endif
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
