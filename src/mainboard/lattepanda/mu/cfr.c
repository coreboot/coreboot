/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>
#include <static.h>

WEAK_DEV_PTR(touchscreen);

static void touchscreen_cfr_update(struct sm_object *new_obj)
{
	if (!DEV_PTR(touchscreen))
		new_obj->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object touchscreen = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen",
	.ui_helptext	= "Enable or disable the Goodix touch panel on I2C0",
	.default_value	= true,
}, WITH_CALLBACK(touchscreen_cfr_update));

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&igd_enabled,
		&igd_dvmt,
		&igd_aperture,
		&touchscreen,
		NULL
	},
};

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&hyper_threading,
		&legacy_8254_timer,
		&me_state,
		&disable_heci1_at_pre_boot,
		&pciexp_clk_pm,
		&pciexp_aspm,
		&pciexp_aspm_cpu,
		&pciexp_l1ss,
		&pciexp_speed,
		&intel_tme,
		&bios_lock,
		&s0ix_enable,
		&vtd,
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
	&devices,
	&power,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
