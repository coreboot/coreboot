/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

static const struct sm_object touchpad_wake = SM_DECLARE_BOOL({
	.opt_name	= "touchpad_wake",
	.ui_name	= "Touchpad Wake",
	.ui_helptext	= "Enable or disable touchpad wake from sleep.\n"
			  "Disabled by default to prevent random wakeups when\n"
			  "the system is moved while sleeping.",
	.default_value	= false,
});

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
		&touchpad_wake,
		&vtd,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
