/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include <common/cfr.h>

static struct sm_obj_form battery_group = {
	.ui_name = "Battery",
	.obj_list = (const struct sm_object *[]) {
#if CONFIG(SYSTEM_TYPE_LAPTOP)
		&charging_speed,
		&max_charge,
#endif
		&power_on_after_fail_bool,
		NULL
	},
};

static struct sm_obj_form debug_group = {
	.ui_name = "Debug",
	.obj_list = (const struct sm_object *[]) {
		&debug_level,
		NULL
	},
};

#if CONFIG(SYSTEM_TYPE_LAPTOP)
static struct sm_obj_form leds_group = {
	.ui_name = "LEDs",
	.obj_list = (const struct sm_object *[]) {
		&charge_led,
		&power_led,
		NULL
	},
};

static struct sm_obj_form keyboard_group = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]) {
		&fn_ctrl_swap,
		&kbl_timeout,
		NULL
	},
};

static struct sm_obj_form audio_video_group = {
	.ui_name = "Audio/Video",
	.obj_list = (const struct sm_object *[]) {
		&microphone,
		&webcam,
		NULL
	},
};

static struct sm_obj_form display_group = {
	.ui_name = "Display",
	.obj_list = (const struct sm_object *[]) {
		&display_native_res,
		NULL
	},
};
#endif

static struct sm_obj_form pcie_power_management_group = {
	.ui_name = "PCIe Power Management",
	.obj_list = (const struct sm_object *[]) {
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		&gna,
#if CONFIG(SYSTEM_TYPE_LAPTOP)
		&memory_speed,
#endif
		&power_profile,
		NULL
	},
};

static struct sm_obj_form security_group = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		&bios_lock,
		&intel_tme,
		&me_state,
		&me_state_counter,
		NULL
	},
};

static struct sm_obj_form suspend_lid_group = {
	.ui_name = "Suspend & Lid",
	.obj_list = (const struct sm_object *[]) {
#if CONFIG(SYSTEM_TYPE_LAPTOP)
		&lid_switch,
#endif
		&s0ix_enable,
		NULL
	},
};

static struct sm_obj_form virtualization_group = {
	.ui_name = "Virtualization",
	.obj_list = (const struct sm_object *[]) {
		&vtd,
		NULL
	},
};

static struct sm_obj_form wireless_group = {
	.ui_name = "Wireless",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&bluetooth_rtd3,
		&wifi,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
#if CONFIG(SYSTEM_TYPE_LAPTOP)
	&audio_video_group,
#endif
	&battery_group,
	&debug_group,
#if CONFIG(SYSTEM_TYPE_LAPTOP)
	&display_group,
	&keyboard_group,
	&leds_group,
#endif
	&pcie_power_management_group,
	&performance_group,
	&security_group,
	&suspend_lid_group,
	&virtualization_group,
	&wireless_group,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	starlabs_cfr_register_overrides();
	cfr_write_setup_menu(cfr_root, sm_root);
}
