/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs which are used to control various Intel common block settings.
 */

#ifndef SOC_INTEL_CMN_CFR_H
#define SOC_INTEL_CMN_CFR_H

#include <drivers/option/cfr_frontend.h>
#include <cpu/intel/common/common.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/pmclib.h>

/* Intel ME State */
static const struct sm_object me_state = SM_DECLARE_ENUM({
	.opt_name	= "me_state",
	.ui_name	= "Intel Management Engine",
	.ui_helptext	= "Enable or disable the Intel Management Engine",
	.default_value	= CONFIG(CSE_DEFAULT_CFR_OPTION_STATE_DISABLED),
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",		1		},
				{ "Enabled",		0		},
				SM_ENUM_VALUE_END			},
});

/* Intel ME State Counter */
static const struct sm_object me_state_counter = SM_DECLARE_NUMBER({
	.opt_name	= "me_state_counter",
	.ui_name	= "ME State Counter",
	.flags		= CFR_OPTFLAG_SUPPRESS,
	.default_value	= 0,
});

/*
 * Power state after power loss
 * Use this option or the one below, but not both
 */
static const struct sm_object power_on_after_fail = SM_DECLARE_ENUM({
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Restore AC power after loss",
	.ui_helptext	= "Specify what to do when power is re-applied after a power loss.",
	.default_value	= CONFIG_MAINBOARD_POWER_FAILURE_STATE,
	.values		= (const struct sm_enum_value[]) {
				{ "Power off (S5)", MAINBOARD_POWER_STATE_OFF		},
				{ "Power on  (S0)", MAINBOARD_POWER_STATE_ON		},
				{ "Previous state", MAINBOARD_POWER_STATE_PREVIOUS	},
				SM_ENUM_VALUE_END					},
});

/*
 * Automatic power-on toggle
 * Use this option or the one above, but not both
 */
static const struct sm_object power_on_after_fail_bool = SM_DECLARE_BOOL({
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Power on after failure",
	.ui_helptext	= "Automatically turn on after a power failure",
	.default_value	= CONFIG_MAINBOARD_POWER_FAILURE_STATE,
});

/* PCIe PCH RP ASPM */
static void update_pcie_aspm(struct sm_object *new)
{
	if (!CONFIG(PCIEXP_ASPM))
		new->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void update_pcie_aspm_cpu(struct sm_object *new)
{
	if (!CONFIG(PCIEXP_ASPM) || !CONFIG(HAS_INTEL_CPU_ROOT_PORTS))
		new->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void update_pcie_l1ss(struct sm_object *new)
{
	if (!CONFIG(PCIEXP_ASPM) || !CONFIG(PCIEXP_L1_SUB_STATE))
		new->sm_enum.flags |= CFR_OPTFLAG_SUPPRESS;
}

/* PCIe Clock PM */
static const struct sm_object pciexp_clk_pm = SM_DECLARE_BOOL({
	.opt_name	= "pciexp_clk_pm",
	.ui_name	= "PCIe Clock Power Management",
	.ui_helptext	= "Enables or disables power management for the PCIe clock. When"
			  " enabled, it reduces power consumption during idle states."
			  " This can help lower overall energy use but may impact"
			  " performance in power-sensitive tasks.",
	.default_value	= true,
});

static const struct sm_object pciexp_aspm = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_aspm",
	.ui_name	= "PCIe PCH RP ASPM",
	.ui_helptext	= "Controls the Active State Power Management for PCIe devices."
			  " Enabling this feature can reduce power consumption of"
			  " PCIe-connected devices during idle times.",
	.default_value	= ASPM_AUTO,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	ASPM_DISABLE	},
				{ "L0s",	ASPM_L0S	},
				{ "L1",		ASPM_L1		},
				{ "L0sL1",	ASPM_L0S_L1	},
				{ "Auto",	ASPM_AUTO	},
				SM_ENUM_VALUE_END		},
}, WITH_DEP_VALUES(&pciexp_clk_pm, 1), WITH_CALLBACK(update_pcie_aspm));

/* PCIe CPU RP ASPM */
static const struct sm_object pciexp_aspm_cpu = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_aspm_cpu",
	.ui_name	= "PCIe CPU RP ASPM",
	.ui_helptext	= "Controls the Active State Power Management for PCIe devices."
			  " Enabling this feature can reduce power consumption of"
			  " PCIe-connected devices during idle times.",
	.default_value	= ASPM_L0S_L1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	ASPM_DISABLE	},
				{ "L0s",	ASPM_L0S	},
				{ "L1",		ASPM_L1		},
				{ "L0sL1",	ASPM_L0S_L1	},
				SM_ENUM_VALUE_END		},
}, WITH_DEP_VALUES(&pciexp_clk_pm, 1), WITH_CALLBACK(update_pcie_aspm_cpu));

/* PCIe L1 Substates */
static const struct sm_object pciexp_l1ss = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_l1ss",
	.ui_name	= "PCIe L1 Substates",
	.ui_helptext	= "Controls deeper power-saving states for PCIe devices."
			  " Enabling this feature allows supported devices to achieve"
			  " lower power states at the cost of slightly increased"
			  " latency when exiting these states.",
	.default_value	= L1_SS_L1_2,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	L1_SS_DISABLED	},
				{ "L1.1",	L1_SS_L1_1	},
				{ "L1.2",	L1_SS_L1_2	},
				SM_ENUM_VALUE_END		},
}, WITH_DEP_VALUES(&pciexp_clk_pm, 1), WITH_CALLBACK(update_pcie_l1ss));

/* PCIe PCH Root Port Speed */
static const struct sm_object pciexp_speed = SM_DECLARE_ENUM({
	.opt_name	= "pciexp_speed",
	.ui_name	= "PCIe PCH Root Port Speed",
	.ui_helptext	= "Sets the maximum port speed for PCIe devices attached to PCH root ports.",
	.default_value	= SPEED_AUTO,
	.values		= (const struct sm_enum_value[]) {
				{ "Auto",	SPEED_AUTO	},
				{ "Gen1",	SPEED_GEN1	},
				{ "Gen2",	SPEED_GEN2	},
				{ "Gen3",	SPEED_GEN3	},
				{ "Gen4",	SPEED_GEN4	},
				SM_ENUM_VALUE_END		},
});

/* TME */
static void update_intel_tme(struct sm_object *new)
{
	if (!is_tme_supported())
		new->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object intel_tme = SM_DECLARE_BOOL({
	.opt_name	= "intel_tme",
	.ui_name	= "Total Memory Encryption",
	.ui_helptext	= "Enable TME (Total Memory Encryption). When enabled, all data stored in"
			  " system memory is encrypted to prevent unauthorized access or data theft."
			  " Disabling TME decreases boot time by around 100ms",
	.default_value	= CONFIG(INTEL_TME),
}, WITH_CALLBACK(update_intel_tme));

/* BIOS Lock */
static void update_smm_bwp(struct sm_object *new)
{
	if (!CONFIG(BOOTMEDIA_SMM_BWP_RUNTIME_OPTION))
		new->sm_bool.flags |= CFR_OPTFLAG_SUPPRESS;
}

static const struct sm_object bios_lock = SM_DECLARE_ENUM({
	.opt_name	= "bios_lock",
	.ui_name	= "BIOS Lock",
	.ui_helptext	= "Enable BIOS write protection in SMM. When enabled, the boot media"
			  " (SPI flash) is only writable in System Management Mode, preventing"
			  " unauthorized writes through the internal controller.",
	.default_value	= CONFIG(BOOTMEDIA_SMM_BWP),
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	false	},
				{ "Enabled",	true	},
				SM_ENUM_VALUE_END	},
}, WITH_CALLBACK(update_smm_bwp));

#endif /* SOC_INTEL_CMN_CFR_H */
