/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include <common/cfr.h>

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		&memory_speed,
		&power_profile,
		NULL
	},
};

static struct sm_obj_form processor_group = {
	.ui_name = "Processor",
	.obj_list = (const struct sm_object *[]) {
		&me_state,
		&me_state_counter,
		&s0ix_enable,
		&vtd,
		NULL
	},
};

static struct sm_obj_form power_group = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&max_charge,
		&charging_speed,
		&power_led,
		&charge_led,
		&power_on_after_fail_bool,
		NULL
	},
};

static struct sm_obj_form keyboard_group = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]) {
		&kbl_timeout,
		&fn_ctrl_swap,
		NULL
	},
};

static struct sm_obj_form devices_group = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&display_native_res,
		&gna,
		&lid_switch,
		&microphone,
		&webcam,
		&wifi,
		NULL
	},
};

static struct sm_obj_form security_group = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		&bios_lock,
		&intel_tme,
		NULL
	},
};

static struct sm_obj_form pci_group = {
	.ui_name = "PCI",
	.obj_list = (const struct sm_object *[]) {
		&pciexp_clk_pm,
		&pciexp_aspm,
		&pciexp_l1ss,
		NULL
	},
};

static struct sm_obj_form coreboot_group = {
	.ui_name = "coreboot",
	.obj_list = (const struct sm_object *[]) {
		&debug_level,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&performance_group,
	&processor_group,
	&power_group,
	&keyboard_group,
	&devices_group,
	&security_group,
	&pci_group,
	&coreboot_group,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	starlabs_cfr_register_overrides();
	cfr_write_setup_menu(cfr_root, sm_root);
}
