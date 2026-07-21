/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/aspm.h>
#include <intelblocks/cfr.h>
#include <variants.h>
#include <common/cfr.h>

static const struct pcie_pm_option_names pciexp_wifi_names = {
	.clk_pm = "pciexp_wifi_clk_pm",
	.aspm = "pciexp_wifi_aspm",
	.l1ss = "pciexp_wifi_l1ss",
	.speed = "pciexp_speed",
};

static const struct pcie_pm_option_names pciexp_ssd_names = {
	.clk_pm = "pciexp_ssd_clk_pm",
	.aspm = "pciexp_ssd_aspm",
	.l1ss = "pciexp_ssd_l1ss",
	.speed = "pciexp_speed",
};

static const struct pcie_pm_option_names pciexp_ssd2_names = {
	.clk_pm = "pciexp_ssd2_clk_pm",
	.aspm = "pciexp_ssd2_aspm",
	.l1ss = "pciexp_ssd2_l1ss",
	.speed = "pciexp_speed",
};

void mainboard_get_pcie_pm_options(const struct pcie_rp_config *rp_cfg,
				   unsigned int index,
				   bool is_cpu_rp,
				   struct pcie_pm_option_names *names)
{
	(void)rp_cfg;

	if (!names)
		return;

#if CONFIG(BOARD_STARLABS_STARFIGHTER_RPL)
	if (is_cpu_rp) {
		if (index == CPU_RP(1))
			*names = pciexp_ssd_names;
		return;
	}
#else
	if (is_cpu_rp)
		return;
#endif

#if CONFIG(BOARD_STARLABS_STARFIGHTER_RPL)
	switch (index) {
	case PCH_RP(5):
		*names = pciexp_wifi_names;
		return;
	case PCH_RP(9):
		*names = pciexp_ssd2_names;
		return;
	}
#elif CONFIG(BOARD_STARLABS_STARFIGHTER_MTL)
	switch (index) {
	case PCH_RP(9):
		*names = pciexp_wifi_names;
		return;
	case PCH_RP(10):
		*names = pciexp_ssd_names;
		return;
	case PCH_RP(1):
		*names = pciexp_ssd2_names;
		return;
	}
#endif
}

#if CONFIG(BOARD_STARLABS_STARFIGHTER_MTL)
static const struct sm_object firmware_enable_amp = SM_DECLARE_BOOL({
	.opt_name	= "firmware_enable_amp",
	.ui_name	= "Firmware Enables AMP",
	.ui_helptext	= "Enabled: assert LINE2 EAPD and drive GPIO2 high "
			  "during HDA initialization.\n"
			  "Disabled: leave the speaker amp off for OS runtime "
			  "sequencing.",
	.default_value	= true,
});
#endif

static struct sm_obj_form audio_group = {
	.ui_name = "Audio",
	.obj_list = (const struct sm_object *[]) {
		&hda_dsp,
		#if CONFIG(BOARD_STARLABS_STARFIGHTER_MTL)
		&firmware_enable_amp,
		#endif
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
		#if CONFIG(EC_STARLABS_ADAPTER_AUTO_POWER_ON)
		&power_on_ac,
		#endif
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

#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
static struct sm_obj_form io_expansion_group = {
	.ui_name = "I/O / Expansion",
	.obj_list = (const struct sm_object *[]) {
		&card_reader,
		&thunderbolt,
		NULL
	},
};
#endif

static struct sm_obj_form keyboard_group = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]) {
		&fn_ctrl_swap,
		&kbl_timeout,
		NULL
	},
};

static struct sm_obj_form trackpad_group = {
	.ui_name = "Trackpad",
	.obj_list = (const struct sm_object *[]) {
		&touchpad_haptics,
		&touchpad_force_press,
		&touchpad_force_release,
		&touchpad_report_rate,
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
		&pciexp_wifi_clk_pm,
		&pciexp_wifi_aspm,
		&pciexp_wifi_l1ss,
		&pciexp_ssd_clk_pm,
		&pciexp_ssd_aspm,
		&pciexp_ssd_l1ss,
		&pciexp_ssd2_clk_pm,
		&pciexp_ssd2_aspm,
		&pciexp_ssd2_l1ss,
		#endif
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_RAPTORLAKE)
		&gna,
		#endif
		&hyper_threading,
		&memory_speed,
		&power_profile,
		&pl1_override,
		&pl2_override,
		#if CONFIG(STARLABS_LEGACY_PL4)
		&pl4_override,
		#endif
		&tcc_temp,
		#if CONFIG(SOC_INTEL_METEORLAKE)
		&vpu,
		#endif
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
		#if CONFIG(SOC_INTEL_RAPTORLAKE) || CONFIG(SOC_INTEL_METEORLAKE)
		&ibecc,
		#endif
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
		&wifi,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&audio_group,
	&battery_group,
	&debug_group,
	#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
	&io_expansion_group,
	#endif
	&keyboard_group,
	&trackpad_group,
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
