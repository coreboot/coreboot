/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#if CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM)
#include <intelblocks/aspm.h>
#endif
#include <intelblocks/cfr.h>
#include <common/cfr.h>

#if CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM)
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

void mainboard_get_pcie_pm_options(const struct pcie_rp_config *rp_cfg,
				   unsigned int index,
				   bool is_cpu_rp,
				   struct pcie_pm_option_names *names)
{
	(void)rp_cfg;

	if (!names)
		return;

#if CONFIG(BOARD_STARLABS_STARBOOK_RPL)
	if (is_cpu_rp) {
		if (index == CPU_RP(1))
			*names = pciexp_ssd_names;
		return;
	}
#else
	if (is_cpu_rp)
		return;
#endif

#if CONFIG(BOARD_STARLABS_STARBOOK_ADL)
	switch (index) {
	case PCH_RP(5):
		*names = pciexp_wifi_names;
		return;
	case PCH_RP(9):
		*names = pciexp_ssd_names;
		return;
	}
#elif CONFIG(BOARD_STARLABS_STARBOOK_ADL_N)
	switch (index) {
	case PCH_RP(7):
		*names = pciexp_wifi_names;
		return;
	case PCH_RP(9):
		*names = pciexp_ssd_names;
		return;
	}
#elif CONFIG(BOARD_STARLABS_STARBOOK_MTL)
	switch (index) {
	case PCH_RP(9):
		*names = pciexp_wifi_names;
		return;
	case PCH_RP(10):
		*names = pciexp_ssd_names;
		return;
	}
#elif CONFIG(BOARD_STARLABS_STARBOOK_RPL)
	if (index == PCH_RP(5))
		*names = pciexp_wifi_names;
#endif
}
#endif

static struct sm_obj_form audio_video_group = {
	.ui_name = "Audio/Video",
	.obj_list = (const struct sm_object *[]) {
		&microphone,
		&hda_dsp,
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

static struct sm_obj_form io_expansion_group = {
	.ui_name = "I/O / Expansion",
	.obj_list = (const struct sm_object *[]) {
		&card_reader,
		#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
		&thunderbolt,
		#endif
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
		#if CONFIG(BOARD_STARLABS_STARBOOK_ADL) || CONFIG(BOARD_STARLABS_STARBOOK_ADL_N) || \
		    CONFIG(BOARD_STARLABS_STARBOOK_MTL) || CONFIG(BOARD_STARLABS_STARBOOK_RPL)
		&pciexp_wifi_clk_pm,
		&pciexp_wifi_aspm,
		&pciexp_wifi_l1ss,
		&pciexp_ssd_clk_pm,
		&pciexp_ssd_aspm,
		&pciexp_ssd_l1ss,
		#else
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		#endif
		#endif
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		#if CONFIG(BOARD_HAS_GNA)
		&gna,
		#endif
		#if CONFIG(BOARD_SUPPORTS_HT)
		&hyper_threading,
		#endif
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
		#if CONFIG(BOARD_HAS_FPR)
		&fingerprint_reader,
		#endif
		&intel_tme,
		&me_state,
		&me_state_counter,
		#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || \
		    CONFIG(SOC_INTEL_RAPTORLAKE) || CONFIG(SOC_INTEL_METEORLAKE)
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
		&bluetooth_rtd3,
		&wifi,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&audio_video_group,
	&battery_group,
	&debug_group,
	&io_expansion_group,
	&keyboard_group,
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
