/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include <common/cfr.h>

#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
static struct sm_obj_form audio_video_group = {
	.ui_name = "Audio/Video",
	.obj_list = (const struct sm_object *[]){
		&microphone,
		&webcam,
		NULL,
	},
};
#endif

static struct sm_obj_form battery_group = {
	.ui_name = "Battery",
	.obj_list =
		(const struct sm_object *[]){
#if CONFIG(EC_STARLABS_CHARGING_SPEED)
					     &charging_speed,
#endif
#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
					     &max_charge,
#endif
					     &power_on_after_fail_bool, NULL},
};

static struct sm_obj_form debug_group = {
	.ui_name = "Debug",
	.obj_list = (const struct sm_object *[]){&debug_level, NULL},
};

#if CONFIG(EC_STARLABS_POWER_LED) || CONFIG(EC_STARLABS_CHARGE_LED)
static struct sm_obj_form leds_group = {
	.ui_name = "LEDs",
	.obj_list =
		(const struct sm_object *[]){
#if CONFIG(EC_STARLABS_CHARGE_LED)
					     &charge_led,
#endif
#if CONFIG(EC_STARLABS_POWER_LED)
					     &power_led,
#endif
					     NULL, },
};
#endif

#if CONFIG(SYSTEM_TYPE_LAPTOP)
static struct sm_obj_form keyboard_group = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]){&fn_ctrl_swap, &kbl_timeout, NULL},
};
#endif

#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
static struct sm_obj_form display_group = {
	.ui_name = "Display",
	.obj_list =
		(const struct sm_object *[]){
#if CONFIG(BOARD_STARLABS_LITE_ADL)
					     &accelerometer,
#endif
					     &display_native_res,
#if CONFIG(BOARD_STARLABS_LITE_ADL)
					     &touchscreen,
#endif
					     NULL, },
};
#endif

#if CONFIG(BOARD_STARLABS_LITE_ADL)
static struct sm_obj_form io_expansion_group = {
	.ui_name = "I/O / Expansion",
	.obj_list =
		(const struct sm_object *[]){
					     &card_reader,
					     NULL, },
};
#endif

static struct sm_obj_form pcie_power_management_group = {
	.ui_name = "PCIe Power Management",
	.obj_list =
		(const struct sm_object *[]){
					     &pciexp_aspm,
					     &pciexp_clk_pm,
					     &pciexp_l1ss,
					     NULL, },
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list =
		(const struct sm_object *[]){
#if CONFIG(EC_STARLABS_FAN)
					     &fan_mode,
#endif
					     &gna,
#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
					     &memory_speed,
#endif
					     &power_profile, NULL},
};

static struct sm_obj_form security_group = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]){&bios_lock, &intel_tme, &me_state,
						 &me_state_counter, NULL},
};

static struct sm_obj_form suspend_lid_group = {
	.ui_name = "Suspend & Lid",
	.obj_list =
		(const struct sm_object *[]){
#if CONFIG(EC_STARLABS_LID_SWITCH)
					     &lid_switch,
#endif
					     &s0ix_enable, NULL},
};

static struct sm_obj_form virtualization_group = {
	.ui_name = "Virtualization",
	.obj_list = (const struct sm_object *[]){&vtd, NULL},
};

static struct sm_obj_form wireless_group = {
	.ui_name = "Wireless",
	.obj_list = (const struct sm_object *[]){&bluetooth, &bluetooth_rtd3, &wifi, NULL},
};

static struct sm_obj_form *sm_root[] = {
#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
	&audio_video_group,
#endif
	&battery_group,
	&debug_group,
#if CONFIG(SYSTEM_TYPE_LAPTOP) || CONFIG(SYSTEM_TYPE_DETACHABLE)
	&display_group,
#endif
#if CONFIG(BOARD_STARLABS_LITE_ADL)
	&io_expansion_group,
#endif
#if CONFIG(SYSTEM_TYPE_LAPTOP)
	&keyboard_group,
#endif
#if CONFIG(EC_STARLABS_POWER_LED) || CONFIG(EC_STARLABS_CHARGE_LED)
	&leds_group,
#endif
	&pcie_power_management_group,
	&performance_group,
	&security_group,
	&suspend_lid_group,
	&virtualization_group,
	&wireless_group,
	NULL};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	starlabs_cfr_register_overrides();
	cfr_write_setup_menu(cfr_root, sm_root);
}
