/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include <device/i2c_bus.h>
#include <device/i2c_simple.h>
#include <option.h>
#include <static.h>
#include <variants.h>
#include <common/cfr.h>

void cfr_card_reader_update(struct sm_object *new_obj)
{
	struct device *mxc_accel = DEV_PTR(mxc6655);

	if (!i2c_dev_detect(i2c_busdev(mxc_accel), mxc_accel->path.i2c.device))
		new_obj->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

void cfr_touchscreen_update(struct sm_object *new_obj)
{
	if (get_uint_option("accelerometer", 1) == 0)
		new_obj->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
}

static struct sm_obj_form audio_video_group = {
	.ui_name = "Audio/Video",
	.obj_list = (const struct sm_object *[]) {
		&microphone,
		&webcam,
		NULL
	},
};

static struct sm_obj_form battery_group = {
	.ui_name = "Battery",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_CHARGING_SPEED)
		&charging_speed,
		#endif
		&max_charge,
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

static struct sm_obj_form display_group = {
	.ui_name = "Display",
	.obj_list = (const struct sm_object *[]) {
		&accelerometer,
		&display_native_res,
		&touchscreen,
		NULL
	},
};

static struct sm_obj_form io_expansion_group = {
	.ui_name = "I/O / Expansion",
	.obj_list = (const struct sm_object *[]) {
		&card_reader,
		NULL
	},
};

static struct sm_obj_form leds_group = {
	.ui_name = "LEDs",
	.obj_list = (const struct sm_object *[]) {
		&charge_led,
		&power_led,
		NULL
	},
};

static struct sm_obj_form pcie_power_management_group = {
	.ui_name = "PCIe Power Management",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM)
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		#endif
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_RAPTORLAKE)
		&gna,
		#endif
		&memory_speed,
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
		#if CONFIG(EC_STARLABS_LID_SWITCH)
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
	&audio_video_group,
	&battery_group,
	&debug_group,
	&display_group,
	&io_expansion_group,
	&leds_group,
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
