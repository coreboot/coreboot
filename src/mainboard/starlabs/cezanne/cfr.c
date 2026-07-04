/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <variants.h>

enum {
	STARLABS_CFR_ASPM_DISABLE = 1,
	STARLABS_CFR_ASPM_L0S,
	STARLABS_CFR_ASPM_L1,
	STARLABS_CFR_ASPM_L0S_L1,
	STARLABS_CFR_ASPM_AUTO,
};

enum {
	STARLABS_CFR_L1SS_DISABLED = 1,
	STARLABS_CFR_L1SS_L1_1,
	STARLABS_CFR_L1SS_L1_2,
};

static void cezanne_update_pcie_clk_pm(struct sm_object *new_obj)
{
	if (!CONFIG(PCIEXP_CLK_PM))
		new_obj->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void cezanne_update_pcie_aspm(struct sm_object *new_obj)
{
	if (!CONFIG(PCIEXP_ASPM))
		new_obj->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void cezanne_update_pcie_l1ss(struct sm_object *new_obj)
{
	if (!CONFIG(PCIEXP_ASPM) || !CONFIG(PCIEXP_L1_SUB_STATE))
		new_obj->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

static const struct cfr_default_override cezanne_cfr_overrides[] = {
	CFR_OVERRIDE_ENUM("pciexp_wifi_aspm", STARLABS_CFR_ASPM_L1),
	CFR_OVERRIDE_ENUM("pciexp_ssd_aspm", STARLABS_CFR_ASPM_L1),
	CFR_OVERRIDE_END
};

static const struct sm_object microphone = SM_DECLARE_BOOL({
	.opt_name	= "microphone",
	.ui_name	= "Microphone",
	.ui_helptext	= "Enable or disable the built-in microphone",
	.default_value	= true,
});

static const struct sm_object power_profile = SM_DECLARE_ENUM({
	.opt_name	= "power_profile",
	.ui_name	= "Power Profile",
	.ui_helptext	= "Select whether to maximize performance, battery life or both.",
	.default_value	= PP_BALANCED,
	.values		= (const struct sm_enum_value[]) {
				{ "Power Saver",	PP_POWER_SAVER	},
				{ "Balanced",		PP_BALANCED	},
				{ "Performance",	PP_PERFORMANCE	},
				SM_ENUM_VALUE_END			},
});

#define STARBOOK_CEZANNE_DECLARE_PCIE_PM_OBJECTS(_suffix, _label)			\
static const struct sm_object pciexp_##_suffix##_clk_pm = SM_DECLARE_BOOL({		\
	.opt_name	= "pciexp_" #_suffix "_clk_pm",				\
	.ui_name	= _label " Clock Power Management",			\
	.ui_helptext	= "Enable or disable clock power management for " _label ".", \
	.default_value	= true,							\
}, WITH_CALLBACK(cezanne_update_pcie_clk_pm));					\
											\
static const struct sm_object pciexp_##_suffix##_aspm = SM_DECLARE_ENUM({		\
	.opt_name	= "pciexp_" #_suffix "_aspm",				\
	.ui_name	= _label " ASPM",					\
	.ui_helptext	= "Control Active State Power Management for " _label ".", \
	.default_value	= STARLABS_CFR_ASPM_L1,				\
	.values		= (const struct sm_enum_value[]) {			\
				{ "Disabled",	STARLABS_CFR_ASPM_DISABLE	}, \
				{ "L0s",	STARLABS_CFR_ASPM_L0S		}, \
				{ "L1",		STARLABS_CFR_ASPM_L1		}, \
				{ "L0sL1",	STARLABS_CFR_ASPM_L0S_L1	}, \
				{ "Auto",	STARLABS_CFR_ASPM_AUTO		}, \
				SM_ENUM_VALUE_END				}, \
}, WITH_DEP_VALUES(&pciexp_##_suffix##_clk_pm, true),				\
	WITH_CALLBACK(cezanne_update_pcie_aspm))

#define STARBOOK_CEZANNE_DECLARE_PCIE_L1SS_OBJECT(_suffix, _label)			\
static const struct sm_object pciexp_##_suffix##_l1ss = SM_DECLARE_ENUM({		\
	.opt_name	= "pciexp_" #_suffix "_l1ss",				\
	.ui_name	= _label " L1 Substates",				\
	.ui_helptext	= "Control PCIe L1 substates for " _label ".",		\
	.default_value	= STARLABS_CFR_L1SS_L1_2,				\
	.values		= (const struct sm_enum_value[]) {			\
				{ "Disabled",	STARLABS_CFR_L1SS_DISABLED	}, \
				{ "L1.1",	STARLABS_CFR_L1SS_L1_1		}, \
				{ "L1.2",	STARLABS_CFR_L1SS_L1_2		}, \
				SM_ENUM_VALUE_END				}, \
}, WITH_DEP_VALUES(&pciexp_##_suffix##_clk_pm, true),				\
	WITH_CALLBACK(cezanne_update_pcie_l1ss))

STARBOOK_CEZANNE_DECLARE_PCIE_PM_OBJECTS(wifi, "Wi-Fi");
STARBOOK_CEZANNE_DECLARE_PCIE_PM_OBJECTS(ssd, "SSD");
STARBOOK_CEZANNE_DECLARE_PCIE_L1SS_OBJECT(ssd, "SSD");

#undef STARBOOK_CEZANNE_DECLARE_PCIE_PM_OBJECTS
#undef STARBOOK_CEZANNE_DECLARE_PCIE_L1SS_OBJECT

static const struct sm_object bluetooth_rtd3 = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth_rtd3",
	.ui_name	= "Bluetooth Runtime-D3",
	.ui_helptext	= "Enable or disable Bluetooth power optimization.\n"
			  "Recommended to disable when booting Windows.",
	.default_value	= false,
});

static const struct sm_object wifi = SM_DECLARE_BOOL({
	.opt_name	= "wifi",
	.ui_name	= "Wi-Fi",
	.ui_helptext	= "Enable or disable the built-in Wi-Fi",
	.default_value	= true,
});

static struct sm_obj_form audio_video_group = {
	.ui_name = "Audio/Video",
	.obj_list = (const struct sm_object *[]) {
		&microphone,
		NULL
	},
};

static struct sm_obj_form battery_group = {
	.ui_name = "Battery",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_CHARGING_SPEED)
		&charging_speed,
		#endif
		#if CONFIG(EC_STARLABS_MAX_CHARGE)
		&max_charge,
		#endif
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
		&pciexp_wifi_clk_pm,
		&pciexp_wifi_aspm,
		&pciexp_ssd_clk_pm,
		&pciexp_ssd_aspm,
		&pciexp_ssd_l1ss,
		NULL
	},
};

static struct sm_obj_form performance_group = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		&power_profile,
		NULL
	},
};

static struct sm_obj_form security_group = {
	.ui_name = "Security",
	.obj_list = (const struct sm_object *[]) {
		NULL
	},
};

static struct sm_obj_form suspend_lid_group = {
	.ui_name = "Suspend & Lid",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_LID_SWITCH)
		&lid_switch,
		#endif
		NULL
	},
};

static struct sm_obj_form wireless_group = {
	.ui_name = "Wireless",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth_rtd3,
		&wifi,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&audio_video_group,
	&battery_group,
	&debug_group,
	&keyboard_group,
	&leds_group,
	&pcie_power_management_group,
	&performance_group,
	&security_group,
	&suspend_lid_group,
	&wireless_group,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_register_overrides(cezanne_cfr_overrides);
	cfr_write_setup_menu(cfr_root, sm_root);
}
