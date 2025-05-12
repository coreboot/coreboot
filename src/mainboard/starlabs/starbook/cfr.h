/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _STARBOOK_CFR_H_
#define _STARBOOK_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include <intelblocks/pcie_rp.h>
#include <variants.h>

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= true,
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

static const struct sm_object me_state = SM_DECLARE_ENUM({
	.opt_name	= "me_state",
	.ui_name	= "Intel Management Engine",
	.ui_helptext	= "Enable or disable the Intel Management Engine",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		1		},
				{ "Enabled",		0		},
				SM_ENUM_VALUE_END			},
});

static const struct sm_object me_state_counter = SM_DECLARE_NUMBER({
	.opt_name	= "me_state_counter",
	.ui_name	= "ME State Counter",
	.flags		= CFR_OPTFLAG_SUPPRESS,
	.default_value	= 0,
});

static const struct sm_object power_on_after_fail = SM_DECLARE_BOOL({
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Power on after failure",
	.ui_helptext	= "Automatically turn on after a power failure",
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

static const struct sm_object pciexp_aspm = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_aspm",
	.ui_name	= "PCI ASPM",
	.ui_helptext	= "Controls the Active State Power Management for PCI devices."
			  " Enabling this feature can reduce power consumption of"
			  " PCI-connected devices during idle times.",
	.default_value	= ASPM_L0S_L1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		ASPM_DISABLE	},
				{ "L0s",		ASPM_L0S	},
				{ "L1",			ASPM_L1		},
				{ "L0sL1",		ASPM_L0S_L1	},
				SM_ENUM_VALUE_END			},
});

static const struct sm_object pciexp_clk_pm = SM_DECLARE_BOOL({
	.opt_name	= "pciexp_clk_pm",
	.ui_name	= "PCI Clock Power Management",
	.ui_helptext	= "Enables or disables power management for the PCI clock. When"
			  " enabled, it reduces power consumption during idle states."
			  " This can help lower overall energy use but may impact"
			  " performance in power-sensitive tasks.",
	.default_value	= true,
});

static const struct sm_object pciexp_l1ss = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_l1ss",
	.ui_name	= "PCI L1 Substates",
	.ui_helptext	= "Controls deeper power-saving states for PCI devices."
			  " Enabling this feature allows supported devices to achieve"
			  " lower power states at the cost of slightly increased"
			  " latency when exiting these states.",
	.default_value	= L1_SS_L1_2,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		L1_SS_DISABLED	},
				{ "L1.1",		L1_SS_L1_1	},
				{ "L1.2",		L1_SS_L1_2	},
				SM_ENUM_VALUE_END			},
});

static const struct sm_object thunderbolt = SM_DECLARE_BOOL({
	.opt_name	= "thunderbolt",
	.ui_name	= "Thunderbolt",
	.ui_helptext	= "Enable or disable Thunderbolt support",
	.default_value	= true,
});


static const struct sm_object vpu = SM_DECLARE_BOOL({
	.opt_name	= "vpu",
	.ui_name	= "VPU",
	.ui_helptext	= "Enable or disable VPU",
	.default_value	= true,
});

static const struct sm_object webcam = SM_DECLARE_BOOL({
	.opt_name	= "webcam",
	.ui_name	= "Webcam",
	.ui_helptext	= "Enable or disable the built-in webcam",
	.default_value	= true,
});

static const struct sm_object wireless = SM_DECLARE_BOOL({
	.opt_name	= "wireless",
	.ui_name	= "Wireless",
	.ui_helptext	= "Enable or disable the built-in wireless card",
	.default_value	= true,
});

static const struct sm_object vtd = SM_DECLARE_BOOL({
	.opt_name	= "vtd",
	.ui_name	= "VT-d",
	.ui_helptext	= "Enable or disable Intel VT-d (virtualization)",
	.default_value	= true,
});

static const struct sm_object bluetooth_rtd3 = SM_DECLARE_BOOL({
	.opt_name	= "bluetooth_rtd3",
	.ui_name	= "Bluetooth Runtime-D3",
	.ui_helptext	= "Enable or disable Bluetooth power optimization.\n"
			  "Recommended to disable when booting Windows.",
	.default_value	= true,
});

static const struct sm_object display_native_res = SM_DECLARE_BOOL({
	.opt_name	= "display_native_res",
	.ui_name	= "Display: Use Native Resolution",
	.ui_helptext	= "Enabled: use the native panel resolution at boot.\n"
			  "Disabled: use a fixed/scaled video mode at boot.",
	.default_value	= false,
});

#endif /* _STARBOOK_CFR_H_ */
