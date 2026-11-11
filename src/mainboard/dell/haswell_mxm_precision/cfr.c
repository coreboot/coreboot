/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <cpu/intel/haswell/cfr.h>
#include <drivers/intel/oc_mailbox/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <southbridge/intel/lynxpoint/cfr.h>

static const struct sm_object tdp_pl1_override = SM_DECLARE_NUMBER({
	.opt_name	= "tdp_pl1_override",
	.ui_name	= "CPU PL1 power limit (W)",
	.ui_helptext	= "Long-duration CPU package power limit.\n"
			  "Info: Intel recommends setting PL1 to match the CPU's TDP.\n"
			  "Default: 47 W.\n"
			  "Range: 37-57 W.",
	.default_value	= 47,
	.min		= 37,
	.max		= 57,
	.step		= 1,
	.display_flags	= 0,
});

static const struct sm_object tdp_pl2_override = SM_DECLARE_NUMBER({
	.opt_name	= "tdp_pl2_override",
	.ui_name	= "CPU PL2 power limit (W)",
	.ui_helptext	= "Short-duration CPU package power limit.\n"
			  "Info: Intel recommends setting PL2 to 1.25 x PL1.\n"
			  "Some OEMs disable this limitation by setting PL2 to 4096 W.\n"
			  "In that case, maximum power draw is only limited by IccMax and thermal budget.\n"
			  "Default: 59 W.\n"
			  "Range: 37-4096 W.\n"
			  "Must be >= PL1 (enforced at boot).",
	.default_value	= 59,
	.min		= 37,
	.max		= 4096,
	.step		= 1,
	.display_flags	= 0,
});

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&me_disable,
		&nmi,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		&tdp_pl1_override,
		&tdp_pl2_override,
		&cpu_power_limit_lock,
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
