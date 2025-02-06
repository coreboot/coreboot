/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <commonlib/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <inttypes.h>
#include <intelblocks/pcie_rp.h>
#include <string.h>
#include <types.h>
#include <variants.h>

static const struct sm_object boot_option = SM_DECLARE_ENUM({
	.opt_name	= "boot_option",
	.ui_name	= "Boot Option",
	.ui_helptext	= "Change the boot device in the event of a failed boot",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Fallback",		0		},
				{ "Normal",		1		},
				SM_ENUM_VALUE_END			},
});

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= true,
});

#if CONFIG(EC_STARLABS_CHARGING_SPEED)
static const struct sm_object charging_speed = SM_DECLARE_ENUM({
	.opt_name	= "charging_speed",
	.ui_name	= "Charging Speed",
	.ui_helptext	= "Set the maximum speed to charge the battery. Charging faster"
			  " will increase heat and battery wear.",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "1.0C",		0	},
				{ "0.5C",		1	},
				{ "0.2C",		2	},
				SM_ENUM_VALUE_END		},
});
#endif

static const struct sm_object debug_level = SM_DECLARE_ENUM({
	.opt_name	= "debug_level",
	.ui_name	= "Debug Level",
	.ui_helptext	= "Set the verbosity of the debug output.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Emergency",		0		},
				{ "Alert",		1		},
				{ "Critical",		2		},
				{ "Error",		3		},
				{ "Warning",		4		},
				{ "Notice",		5		},
				{ "Info",		6		},
				{ "Debug",		7		},
				{ "Spew",		8		},
				SM_ENUM_VALUE_END			},
});

static const struct sm_object fan_mode = SM_DECLARE_ENUM({
	.opt_name	= "fan_mode",
	.ui_name	= "Fan Mode",
	.ui_helptext	= "Adjust the fan curve to prioritize performance or noise levels.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Normal",		0		},
				{ "Aggressive",		1		},
				{ "Quiet",		2		},
				{ "Disabled",		3		},
				SM_ENUM_VALUE_END			},
});

#if CONFIG(BOARD_STARLABS_STARBOOK_ADL) || CONFIG(BOARD_STARLABS_STARBOOK_RPL)
static const struct sm_object fingerprint_reader = SM_DECLARE_BOOL({
	.opt_name	= "fingerprint_reader",
	.ui_name	= "Fingerprint Reader",
	.ui_helptext	= "Enable or disable the built-in fingerprint reader",
	.default_value	= true,
});
#endif

static const struct sm_object fn_ctrl_swap = SM_DECLARE_BOOL({
	.opt_name	= "fn_ctrl_swap",
	.ui_name	= "Fn Ctrl Reverse",
	.ui_helptext	= "Swap the functions of the [Fn] and [Ctrl] keys",
	.default_value	= false,
});

#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_RAPTORLAKE)
static const struct sm_object gna = SM_DECLARE_BOOL({
	.opt_name	= "gna",
	.ui_name	= "Gaussian & Neural Accelerator",
	.ui_helptext	= "Enable or Disable the Gaussian & Neural Accelerator",
	.default_value	= false,
});
#endif

#if !CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading",
	.ui_helptext	= "Enable or disable Hyper-Threading",
	.default_value	= true,
});
#endif

static const struct sm_object kbl_timeout = SM_DECLARE_ENUM({
	.opt_name	= "kbl_timeout",
	.ui_name	= "Keyboard Backlight Timeout",
	.ui_helptext	= "Set the amount of time before the keyboard backlight turns off"
			  " when un-used",
	.default_value	= 0,
	.values		= (struct sm_enum_value[]) {
				{ "30 seconds",		0	},
				{ "1 minute",		1	},
				{ "3 minutes",		2	},
				{ "5 minutes",		3	},
				{ "Never",		4	},
				SM_ENUM_VALUE_END		},
});

#if CONFIG(EC_STARLABS_LID_SWITCH)
static const struct sm_object lid_switch = SM_DECLARE_ENUM({
	.opt_name	= "lid_switch",
	.ui_name	= "Lid Switch",
	.ui_helptext	= "Enable or disable the lid switch.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Enabled",		0	},
				{ "Disabled",		1	},
				SM_ENUM_VALUE_END		},
});
#endif

static const struct sm_object max_charge = SM_DECLARE_ENUM({
	.opt_name	= "max_charge",
	.ui_name	= "Maximum Charge Level",
	.ui_helptext	= "Set the maximum level the battery will charge to.",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "100%",		0	},
				{ "80%",		1	},
				{ "60%",		2	},
				SM_ENUM_VALUE_END		},
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

#if CONFIG(BOARD_STARLABS_STARBOOK_ADL)
static const struct sm_object pci_hot_plug = SM_DECLARE_BOOL({
	.opt_name	= "pci_hot_plug",
	.ui_name	= "Third-Party SSD Support",
	.ui_helptext	= "Enables PCI Hot Plug, which slows down the SSD initialization. It"
			  " is required for certain third-party SSDs to be detected.",
	.default_value	= false,
});
#endif

#if CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_METEORLAKE)
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
#endif

static const struct sm_object reboot_counter = SM_DECLARE_NUMBER({
	.opt_name	= "reboot_counter",
	.ui_name	= "Reboot Counter",
	.flags		= CFR_OPTFLAG_SUPPRESS,
	.default_value	= 0,
});

#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
static const struct sm_object thunderbolt = SM_DECLARE_BOOL({
	.opt_name	= "thunderbolt",
	.ui_name	= "Thunderbolt",
	.ui_helptext	= "Enable or disable Thunderbolt support",
	.default_value	= true,
});
#endif

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

static struct sm_obj_form performance = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&fan_mode,
		&power_profile,
		NULL
	},
};

static struct sm_obj_form processor = {
	.ui_name = "Processor",
	.obj_list = (const struct sm_object *[]) {
		&me_state,
		&me_state_counter,
		#if !CONFIG(SOC_INTEL_ALDERLAKE_PCH_N)
		&hyper_threading,
		#endif
		&vtd,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&max_charge,
		#if CONFIG(EC_STARLABS_CHARGING_SPEED)
		&charging_speed,
		#endif
		&power_on_after_fail,
		NULL
	},
};

static struct sm_obj_form keyboard = {
	.ui_name = "Keyboard",
	.obj_list = (const struct sm_object *[]) {
		&kbl_timeout,
		&fn_ctrl_swap,
		NULL
	},
};

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&card_reader,
		#if CONFIG(BOARD_STARLABS_STARBOOK_ADL) || CONFIG(BOARD_STARLABS_STARBOOK_RPL)
		&fingerprint_reader,
		#endif
		#if CONFIG(SOC_INTEL_TIGERLAKE) || CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_RAPTORLAKE)
		&gna,
		#endif
		#if CONFIG(EC_STARLABS_LID_SWITCH)
		&lid_switch,
		#endif
		&microphone,
		#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
		&thunderbolt,
		#endif
		&webcam,
		&wireless,
		NULL
	},
};

static struct sm_obj_form pci = {
	.ui_name = "PCI",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(BOARD_STARLABS_STARBOOK_ADL)
		&pci_hot_plug,
		#endif
		#if CONFIG(SOC_INTEL_ALDERLAKE) || CONFIG(SOC_INTEL_METEORLAKE)
		&pciexp_clk_pm,
		&pciexp_aspm,
		&pciexp_l1ss,
		#endif
		NULL
	},
};

static struct sm_obj_form coreboot = {
	.ui_name = "coreboot",
	.obj_list = (const struct sm_object *[]) {
		&boot_option,
		&debug_level,
		&reboot_counter,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&performance,
	&processor,
	&power,
	&keyboard,
	&devices,
	&pci,
	&coreboot,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
