/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		&s0ix_enable,
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
