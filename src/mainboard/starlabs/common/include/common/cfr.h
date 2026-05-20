/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_CFR_H_
#define _STARLABS_CMN_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/soc_chip.h>
#include <common/powercap.h>
#include <common/touchpad.h>

void cfr_card_reader_update(struct sm_object *new_obj);
void cfr_touchscreen_update(struct sm_object *new_obj);
void starlabs_cfr_custom_profile_update(struct sm_object *new_obj);
void starlabs_cfr_register_overrides(void);

static const struct sm_object accelerometer = SM_DECLARE_BOOL({
	.opt_name	= "accelerometer",
	.ui_name	= "Accelerometer",
	.ui_helptext	= "Enable or disable the built-in accelerometer",
	.default_value	= true,
});

static const struct sm_object bluetooth = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth",
	.ui_name	= "Bluetooth",
	.ui_helptext	= "Enable or disable the built-in Bluetooth",
	.default_value	= true,
});

static const struct sm_object bluetooth_rtd3 = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth_rtd3",
	.ui_name	= "Bluetooth Runtime-D3",
	.ui_helptext	= "Enable or disable Bluetooth power optimization.\n"
			  "Recommended to disable when booting Windows.",
	.default_value	= false,
});

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= !CONFIG(BOARD_STARLABS_LITE_ADL),
}, WITH_CALLBACK(cfr_card_reader_update));

static const struct sm_object fingerprint_reader = SM_DECLARE_BOOL({
	.opt_name	= "fingerprint_reader",
	.ui_name	= "Fingerprint Reader",
	.ui_helptext	= "Enable or disable the built-in fingerprint reader",
	.default_value	= true,
});

static const struct sm_object gna = SM_DECLARE_BOOL({
	.opt_name	= "gna",
	.ui_name	= "Gaussian & Neural Accelerator",
	.ui_helptext	= "Enable or Disable the Gaussian & Neural Accelerator",
	.default_value	= false,
});

static const struct sm_object memory_speed = SM_DECLARE_ENUM({
	.opt_name	= "memory_speed",
	.ui_name	= "Memory Speed",
	.ui_helptext	= "Configure the speed that the memory will run at. "
			  "Higher speeds produce more heat and consume more power "
			  "but provide higher performance.",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
		{ "5500MT/s", 0 },
		{ "6400MT/s", 1 },
		{ "7500MT/s", 2 },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object microphone = SM_DECLARE_BOOL({
	.opt_name	= "microphone",
	.ui_name	= "Microphone",
	.ui_helptext	= "Enable or disable the built-in microphone",
	.default_value	= true,
});

static const struct sm_object hda_dsp = SM_DECLARE_BOOL({
	.opt_name	= "hda_dsp",
	.ui_name	= "Digital Signal Processor",
	.ui_helptext	= "Enable or disable the Intel HD Audio Digital Signal Processor.\n"
			  "Recommended to disable when booting Windows.",
	.default_value	= true,
});

#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || \
	CONFIG(SOC_INTEL_RAPTORLAKE) || CONFIG(SOC_INTEL_METEORLAKE)
static const struct sm_object ibecc = SM_DECLARE_BOOL({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "ibecc_enable",
	.ui_name	= "In-Band ECC",
	.ui_helptext	= "Enable or disable In-Band ECC. Enabling this option reduces "
			  "available RAM because memory is reserved for ECC data.",
	.default_value	= false,
});
#endif

static const struct sm_object power_profile = SM_DECLARE_ENUM({
	.opt_name	= "power_profile",
	.ui_name	= "Power Profile",
	.ui_helptext	= "Choose maximum battery life, balanced behaviour, "
			  "maximum performance, or custom CPU power and "
			  "thermal settings.",
	.default_value	= PP_BALANCED,
	.values		= (const struct sm_enum_value[]) {
			{ "Power Saver",	PP_POWER_SAVER	},
			{ "Balanced",		PP_BALANCED	},
			{ "Performance",	PP_PERFORMANCE	},
			{ "Custom",		PP_CUSTOM	},
			SM_ENUM_VALUE_END			},
});

static const struct sm_object pl1_override = SM_DECLARE_NUMBER({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "pl1_override",
	.ui_name	= "Sustained CPU Power Limit (PL1, W)",
	.ui_helptext	= "Long-duration CPU package power limit in Watts.",
	.default_value	= 0,
	.step		= 1,
}, WITH_DEP_VALUES(&power_profile, PP_CUSTOM),
	WITH_CALLBACK(starlabs_cfr_custom_profile_update));

static const struct sm_object pl2_override = SM_DECLARE_NUMBER({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "pl2_override",
	.ui_name	= "Short Boost CPU Power Limit (PL2, W)",
	.ui_helptext	= "Short-duration CPU package power limit in Watts. "
			  "Runtime clamped so PL2 never exceeds PL4.",
	.default_value	= 0,
	.step		= 1,
}, WITH_DEP_VALUES(&power_profile, PP_CUSTOM),
	WITH_CALLBACK(starlabs_cfr_custom_profile_update));

static const struct sm_object pl4_override = SM_DECLARE_NUMBER({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "pl4_override",
	.ui_name	= "Hard CPU Power Limit (PL4, W)",
	.ui_helptext	= "Hard CPU package power limit in Watts. This can only be reduced "
			  "from the stock board limit.",
	.default_value	= 0,
	.step		= 1,
}, WITH_DEP_VALUES(&power_profile, PP_CUSTOM),
	WITH_CALLBACK(starlabs_cfr_custom_profile_update));

static const struct sm_object tcc_temp = SM_DECLARE_NUMBER({
	.flags		= CFR_OPTFLAG_RUNTIME,
	.opt_name	= "tcc_temp",
	.ui_name	= "CPU Thermal Throttling Temperature (TCC, C)",
	.ui_helptext	= "CPU temperature in Celsius where thermal throttling "
			  "starts. Higher values let the CPU run hotter "
			  "before throttling.",
	.default_value	= 0,
	.step		= 1,
}, WITH_DEP_VALUES(&power_profile, PP_CUSTOM),
	WITH_CALLBACK(starlabs_cfr_custom_profile_update));

static void starlabs_update_pcie_clk_pm(struct sm_object *new_obj)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM))
		new_obj->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void starlabs_update_pcie_aspm(struct sm_object *new_obj)
{
	if (!CONFIG(PCIEXP_ASPM))
		new_obj->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void starlabs_update_pcie_l1ss(struct sm_object *new_obj)
{
	if (!CONFIG(PCIEXP_ASPM) || !CONFIG(PCIEXP_L1_SUB_STATE))
		new_obj->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

#define STARLABS_DECLARE_PCIE_PM_OBJECTS(_suffix, _label)					\
static const struct sm_object pciexp_##_suffix##_clk_pm = SM_DECLARE_BOOL({			\
	.opt_name	= "pciexp_" #_suffix "_clk_pm",					\
	.ui_name	= _label " Clock Power Management",				\
	.ui_helptext	= "Enable or disable clock power management for " _label ".",	\
	.default_value	= true,								\
}, WITH_CALLBACK(starlabs_update_pcie_clk_pm));					\
											\
static const struct sm_object pciexp_##_suffix##_aspm = SM_DECLARE_ENUM({			\
	.opt_name	= "pciexp_" #_suffix "_aspm",					\
	.ui_name	= _label " ASPM",						\
	.ui_helptext	= "Control Active State Power Management for " _label ".",	\
	.default_value	= ASPM_L0S_L1,						\
	.values		= (const struct sm_enum_value[]) {				\
				{ "Disabled",	ASPM_DISABLE	},			\
				{ "L0s",	ASPM_L0S	},			\
				{ "L1",		ASPM_L1		},			\
				{ "L0sL1",	ASPM_L0S_L1	},			\
				{ "Auto",	ASPM_AUTO	},			\
				SM_ENUM_VALUE_END				},	\
}, WITH_DEP_VALUES(&pciexp_##_suffix##_clk_pm, true),					\
	WITH_CALLBACK(starlabs_update_pcie_aspm));					\
											\
static const struct sm_object pciexp_##_suffix##_l1ss = SM_DECLARE_ENUM({			\
	.opt_name	= "pciexp_" #_suffix "_l1ss",					\
	.ui_name	= _label " L1 Substates",					\
	.ui_helptext	= "Control PCIe L1 substates for " _label ".",			\
	.default_value	= L1_SS_L1_2,						\
	.values		= (const struct sm_enum_value[]) {				\
				{ "Disabled",	L1_SS_DISABLED	},			\
				{ "L1.1",	L1_SS_L1_1	},			\
				{ "L1.2",	L1_SS_L1_2	},			\
				SM_ENUM_VALUE_END				},	\
}, WITH_DEP_VALUES(&pciexp_##_suffix##_clk_pm, true),					\
	WITH_CALLBACK(starlabs_update_pcie_l1ss))

STARLABS_DECLARE_PCIE_PM_OBJECTS(wifi, "WiFi");
STARLABS_DECLARE_PCIE_PM_OBJECTS(ssd, "SSD");
STARLABS_DECLARE_PCIE_PM_OBJECTS(ssd2, "SSD 2");
STARLABS_DECLARE_PCIE_PM_OBJECTS(lan1, "LAN 1");
STARLABS_DECLARE_PCIE_PM_OBJECTS(lan2, "LAN 2");

#undef STARLABS_DECLARE_PCIE_PM_OBJECTS

static const struct sm_object s0ix_enable = SM_DECLARE_BOOL({
	.opt_name	= "s0ix_enable",
	.ui_name	= "Modern Standby (S0ix)",
	.ui_helptext	= "Enabled: Use S0ix for device sleep.\n"
			  "Disabled: Use ACPI S3 for device sleep.\n"
			  "Requires Intel ME to be enabled.",
	.default_value	= false,
});

static const struct sm_object thunderbolt = SM_DECLARE_BOOL({
	.opt_name	= "thunderbolt",
	.ui_name	= "Thunderbolt",
	.ui_helptext	= "Enable or disable Thunderbolt support",
	.default_value	= true,
});

static const struct sm_object touchpad_haptics = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_haptics",
	.ui_name	= "Touchpad Vibration Intensity",
	.ui_helptext	= "Choose how strong the touchpad click vibration feels.",
	.default_value	= STARLABS_TOUCHPAD_HAPTICS_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Off",	0 },
		{ "Low",	1 },
		{ "Medium",	2 },
		{ "High",	3 },
		{ "Maximum",	4 },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_force_press = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_force_press",
	.ui_name	= "Touchpad Click Force",
	.ui_helptext	= "Choose how much force it takes to click the touchpad.",
	.default_value	= STARLABS_TOUCHPAD_PRESS_FORCE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Minimal",	STARLABS_TOUCHPAD_FORCE_MINIMAL },
		{ "Low",	STARLABS_TOUCHPAD_FORCE_LOW },
		{ "Average",	STARLABS_TOUCHPAD_FORCE_AVERAGE },
		{ "High",	STARLABS_TOUCHPAD_FORCE_HIGH },
		{ "Hulk",	STARLABS_TOUCHPAD_FORCE_HULK },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_force_release = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_force_release",
	.ui_name	= "Touchpad Release Force",
	.ui_helptext	= "Choose how much force it takes for the touchpad click to release.",
	.default_value	= STARLABS_TOUCHPAD_RELEASE_FORCE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Minimal",	STARLABS_TOUCHPAD_FORCE_MINIMAL },
		{ "Low",	STARLABS_TOUCHPAD_FORCE_LOW },
		{ "Average",	STARLABS_TOUCHPAD_FORCE_AVERAGE },
		{ "High",	STARLABS_TOUCHPAD_FORCE_HIGH },
		{ "Hulk",	STARLABS_TOUCHPAD_FORCE_HULK },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchpad_report_rate = SM_DECLARE_ENUM({
	.opt_name	= "touchpad_report_rate",
	.ui_name	= "Touchpad Tracking Speed",
	.ui_helptext	= "Choose how quickly the touchpad reports movement.",
	.default_value	= STARLABS_TOUCHPAD_REPORT_RATE_DEFAULT,
	.values		= (const struct sm_enum_value[]) {
		{ "Relaxed",	STARLABS_TOUCHPAD_RATE_RELAXED },
		{ "Balanced",	STARLABS_TOUCHPAD_RATE_BALANCED },
		{ "Fast",	STARLABS_TOUCHPAD_RATE_FAST },
		{ "Ludicrous",	STARLABS_TOUCHPAD_RATE_LUDICROUS },
		{ "Plaid",	STARLABS_TOUCHPAD_RATE_PLAID },
		SM_ENUM_VALUE_END,
	},
});

static const struct sm_object touchscreen = SM_DECLARE_BOOL({
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen",
	.ui_helptext	= "Enable or disable the built-in touch-screen",
	.default_value	= true,
}, WITH_CALLBACK(cfr_touchscreen_update));

static const struct sm_object vpu = SM_DECLARE_BOOL({
	.opt_name	= "vpu",
	.ui_name	= "VPU",
	.ui_helptext	= "Enable or disable VPU",
	.default_value	= false,
});

static const struct sm_object vtd = SM_DECLARE_BOOL({
	.opt_name	= "vtd",
	.ui_name	= "VT-d",
	.ui_helptext	= "Enable or disable Intel VT-d (virtualization)",
	.default_value	= true,
});

static const struct sm_object webcam = SM_DECLARE_BOOL({
	.opt_name	= "webcam",
	.ui_name	= "Webcam",
	.ui_helptext	= "Enable or disable the built-in webcam",
	.default_value	= true,
});

static const struct sm_object wifi = SM_DECLARE_BOOL({
	.opt_name	= "wifi",
	.ui_name	= "Wi-Fi",
	.ui_helptext	= "Enable or disable the built-in Wi-Fi",
	.default_value	= true,
});

#endif /* _STARLABS_CMN_CFR_H_ */
