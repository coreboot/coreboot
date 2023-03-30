/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <commonlib/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/pmclib.h>
#include <inttypes.h>
#include <soc/pm.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include "vpd.h"

static void update_rt_perf(const struct sm_object *obj, struct sm_object *new)
{
	const bool rt_perf = get_emi_eeprom_vpd()->profile == ATLAS_PROF_REALTIME_PERFORMANCE;

	if (!rt_perf)
		return;

	if (obj->kind == SM_OBJ_BOOL) {
		new->sm_bool.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_bool.default_value = false;
	} else if (obj->kind == SM_OBJ_ENUM) {
		new->sm_enum.flags = CFR_OPTFLAG_SUPPRESS;
		new->sm_enum.default_value = 0;
	}
}

static void update_bad_profile(const struct sm_object *obj, struct sm_object *new)
{
	const bool pf_ok = get_emi_eeprom_vpd()->profile != 0;

	if (!pf_ok)
		return;
	new->sm_comment.flags |= CFR_OPTFLAG_SUPPRESS;
}

static void update_serial(const struct sm_object *obj, struct sm_object *new)
{
	new->sm_varchar.default_value = get_emi_eeprom_vpd()->serial_number;
}

static void update_part_number(const struct sm_object *obj, struct sm_object *new)
{
	new->sm_varchar.default_value = get_emi_eeprom_vpd()->part_number;
}

static void update_profile(const struct sm_object *obj, struct sm_object *new)
{
	new->sm_number.default_value = get_emi_eeprom_vpd()->profile;
}

/* TODO: Suppress option if carrier board is known to not have a RTC battery */
static const struct sm_enum_value pwr_after_g3_values[] = {
	{ "Power off (S5)", MAINBOARD_POWER_STATE_OFF },
	{ "Power on (S0)",  MAINBOARD_POWER_STATE_ON  },
	/* No support for previous/last power state */
	SM_ENUM_VALUE_END,
};

static const struct sm_object power_on_after_fail = SM_DECLARE_ENUM({
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Restore AC Power Loss",
	.ui_helptext	= "Specify what to do when power is re-applied "
				"after a power loss. This option has no effect "
				"on systems without a RTC battery.",
	.default_value	= CONFIG_MAINBOARD_POWER_FAILURE_STATE,
	.values		= pwr_after_g3_values,
});

static const struct sm_enum_value primary_display_values[] = {
	{ "Intel iGPU",    0 },
	{ "CPU PEG dGPU",  1 },
	{ "PCH PCIe dGPU", 2 },
	{ "Auto",          3 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object primary_display = SM_DECLARE_ENUM({
	.opt_name	= "primary_display",
	.ui_name	= "Primary display device",
	.ui_helptext	= "Specify which display device to use as primary.",
	.default_value	= CONFIG(ONBOARD_VGA_IS_PRIMARY) ? 0 : 3,
	.values		= primary_display_values,
});

static const struct sm_enum_value pkg_c_state_limit_values[] = {
	{ "C0/C1",     0 },
	{ "C2",        1 },
	{ "C3",        2 },
	{ "C6",        3 },
	{ "C7",        4 },
	{ "C7S",       5 },
	{ "C8",        6 },
	{ "C9",        7 },
	{ "C10",       8 },
	{ "Default", 254 },
	{ "Auto",    255 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object turbo_mode = SM_DECLARE_BOOL({
	.opt_name	= "turbo_mode",
	.ui_name	= "Turbo Boost",
	/* No help text */
	.default_value	= true,
});

static const struct sm_object vmx = SM_DECLARE_BOOL({
	.opt_name	= "vmx",
	.ui_name	= "Intel Virtualization Technology (VT-x)",
	/* No help text */
	.default_value	= false,
});

static const struct sm_object vtd = SM_DECLARE_BOOL({
	.opt_name	= "vtd",
	.ui_name	= "Intel Virtualization Technology for Directed I/O (VT-d)",
	/* No help text */
	.default_value	= false,
});

static const struct sm_object ibecc = SM_DECLARE_BOOL({
	.opt_name	= "ibecc",
	.ui_name	= "In-Band ECC",
	.ui_helptext	= "Specify whether In-Band error checking and "
				"correction is to be enabled. Enabling this "
				"option will reduce the amount of available "
				"RAM because some memory is needed to store "
				"ECC codes.",
	.default_value	= false,
});

static const struct sm_object serial_number = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "serial_number",
	.ui_name	= "Serial Number",
}, WITH_CALLBACK(update_serial));

static const struct sm_object part_number = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "part_number",
	.ui_name	= "Part Number",
}, WITH_CALLBACK(update_part_number));

static const struct sm_object bad_profile = SM_DECLARE_COMMENT({
	.flags		= CFR_OPTFLAG_READONLY,
	.ui_name	= "WARNING: Profile code is invalid",
}, WITH_CALLBACK(update_bad_profile));

static const struct sm_object profile = SM_DECLARE_NUMBER({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "profile",
	.ui_name	= "Profile code",
	.ui_helptext	= "The profile code obtained from the EEPROM",
}, WITH_CALLBACK(update_profile));

static const struct sm_object energy_eff_turbo = SM_DECLARE_BOOL({
	.flags		= 0,
	.opt_name	= "energy_eff_turbo",
	.ui_name	= "Energy Efficient Turbo",
	/* No help text */
	.default_value	= false,
}, WITH_CALLBACK(update_rt_perf));

static const struct sm_object pkg_c_state_limit = SM_DECLARE_ENUM({
	.flags		=  0,
	.opt_name	= "pkg_c_state_limit",
	.ui_name	= "Package C-state limit",
	.ui_helptext	= "", /* TODO: write something */
	.default_value	= 255,
	.values		= pkg_c_state_limit_values,
}, WITH_CALLBACK(update_rt_perf));

#define NUM_PCIE_SSC_SETTINGS	21
static struct sm_enum_value pch_pm_pcie_pll_ssc_values[] = {
	[NUM_PCIE_SSC_SETTINGS] = { "Auto", 0xff },
	SM_ENUM_VALUE_END,
};

static void update_pll_ssc_values(const struct sm_object *obj, struct sm_object *new)
{
	for (size_t i = 0; i < NUM_PCIE_SSC_SETTINGS; i++) {
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%u.%u%%", i / 10, i % 10);
		pch_pm_pcie_pll_ssc_values[i].ui_name = strdup(buffer);
		pch_pm_pcie_pll_ssc_values[i].value = i;
	}
}

static const struct sm_object pch_pcie_pll_ssc = SM_DECLARE_ENUM({
	.opt_name	= "pch_pcie_pll_ssc",
	.ui_name	= "PCH PCIe PLL Spread Spectrum Clocking",
	/* No help text */
	.default_value	= 0xff,
	.values		= pch_pm_pcie_pll_ssc_values,
}, WITH_CALLBACK(update_pll_ssc_values));

static const struct sm_object c_states = SM_DECLARE_BOOL({
	.flags		= 0,
	.opt_name	= "c_states",
	.ui_name	= "CPU power states (C-states)",
	.ui_helptext	= "Specify whether C-states are supported.",
	.default_value	= true,
}, WITH_CALLBACK(update_rt_perf));

static const struct sm_object hyper_threading = SM_DECLARE_BOOL({
	.flags		= 0,
	.opt_name	= "hyper_threading",
	.ui_name	= "Hyper-Threading Technology",
	/* No help text */
	.default_value	= true,
}, WITH_CALLBACK(update_rt_perf));

static struct sm_obj_form cpu = {
	.ui_name	= "CPU",
	.obj_list	= (const struct sm_object *[]) {
		&c_states,
		&hyper_threading,
		&turbo_mode,
		&energy_eff_turbo,
		&vmx,
		&vtd,
		NULL
	},
};

static struct sm_obj_form main = {
	.ui_name	= "Main",
	.obj_list	= (const struct sm_object *[]) {
		&serial_number,
		&part_number,
		&bad_profile,
		&profile,
		&power_on_after_fail,
		&primary_display,
		&pkg_c_state_limit,
		&pch_pcie_pll_ssc,
		&ibecc,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&main,
	&cpu,
	NULL
};

/*
 * TODO: Writing this by hand is extremely tedious. Introducing a DSL
 * (Domain-Specific Language) to describe options which is translated
 * into code at build time may be the way to go. Maybe expand SCONFIG
 * so that these can be devicetree options?
 */
void lb_board(struct lb_header *header)
{
	char *current = (char *)lb_new_record(header);
	struct lb_cfr *cfr_root = (struct lb_cfr *)current;

	cfr_write_setup_menu(cfr_root, sm_root);
}
