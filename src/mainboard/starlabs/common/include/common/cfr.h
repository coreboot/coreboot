/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _STARLABS_CMN_CFR_H_
#define _STARLABS_CMN_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include <common/powercap.h>

void cfr_card_reader_update(const struct sm_object *obj, struct sm_object *new_obj);

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
	.default_value	= true,
});

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= true,
}, WITH_CALLBACK(cfr_card_reader_update));

static const struct sm_object display_native_res = SM_DECLARE_BOOL({
	.opt_name	= "display_native_res",
	.ui_name	= "Display: Use Native Resolution",
	.ui_helptext	= "Enabled: use the native panel resolution at boot.\n"
			  "Disabled: use a fixed/scaled video mode at boot.",
	.default_value	= false,
});

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

static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading",
	.ui_helptext	= "Enable or disable Hyper-Threading",
	.default_value	= true,
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

static const struct sm_object pci_hot_plug = SM_DECLARE_BOOL({
	.opt_name	= "pci_hot_plug",
	.ui_name	= "Third-Party SSD Support",
	.ui_helptext	= "Enables PCI Hot Plug, which slows down the SSD initialization. It"
			  " is required for certain third-party SSDs to be detected.",
	.default_value	= false,
});

static const struct sm_object power_profile = SM_DECLARE_ENUM({
	.opt_name	= "power_profile",
	.ui_name	= "Power Profile",
	.ui_helptext	= "Select whether to maximize performance, battery life or both.",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
			{ "Power Saver",	PP_POWER_SAVER	},
			{ "Balanced",		PP_BALANCED	},
			{ "Performance",	PP_PERFORMANCE	},
			SM_ENUM_VALUE_END			},
});

static const struct sm_object s0ix_enable = SM_DECLARE_BOOL({
	.opt_name	= "s0ix_enable",
	.ui_name	= "Modern Standby (S0ix)",
	.ui_helptext	= "Enabled: Use S0ix for device sleep.\n"
			  "Disabled: Use ACPI S3 for device sleep.\n"
			  "Requires Intel ME to be enabled.\n"
			  "Recommended: Enabled when booting Windows, disabled otherwise.",
	.default_value	= false,
});

static const struct sm_object thunderbolt = SM_DECLARE_BOOL({
	.opt_name	= "thunderbolt",
	.ui_name	= "Thunderbolt",
	.ui_helptext	= "Enable or disable Thunderbolt support",
	.default_value	= true,
});

static const struct sm_object touchscreen = SM_DECLARE_BOOL({
	.opt_name	= "touchscreen",
	.ui_name	= "Touchscreen",
	.ui_helptext	= "Enable or disable the built-in touch-screen",
	.default_value	= true,
});

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
	.ui_name	= "WiFi",
	.ui_helptext	= "Enable or disable the built-in WiFi",
	.default_value	= true,
});

#endif /* _STARLABS_CMN_CFR_H_ */
