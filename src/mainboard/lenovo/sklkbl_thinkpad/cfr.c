/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/lenovo/h8/cfr.h>
#include <ec/lenovo/pmh7/cfr.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>
#include <static.h>

/* Hide the dGPU CFR entry if dGPU not present */
static void update_dgpu(const struct sm_object *obj, struct sm_object *new)
{
	struct device *dgpu = DEV_PTR(dgpu);
	if (!dgpu || !dgpu->enabled) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
	}
}

/* dGPU */
static const struct sm_object dgpu = SM_DECLARE_ENUM({
	.opt_name	= "dgpu_enable",
	.ui_name	= "dGPU",
	.ui_helptext	= "Enable or disable the dGPU",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
}, WITH_CALLBACK(update_dgpu));

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&dgpu,
		&hyper_threading,
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&me_state,
		&me_state_counter,
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		&s0ix_enable,
		&vtd,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&backlight,
		&uwb,
		&usb_always_on,
		&volume,
		&wlan,
		&wwan,
		&pm_beeps,
		&battery_beep,
		&fn_ctrl_swap,
		&sticky_fn,
		&f1_to_f12_as_primary,
		&touchpad,
		&trackpoint,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&ec,
	&system,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
