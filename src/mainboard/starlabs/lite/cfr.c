/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <commonlib/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <inttypes.h>
#include <intelblocks/pcie_rp.h>
#include <string.h>
#include <types.h>
#include <variants.h>

static const struct sm_object card_reader = SM_DECLARE_BOOL({
	.opt_name	= "card_reader",
	.ui_name	= "Card Reader",
	.ui_helptext	= "Enable or disable the built-in card reader",
	.default_value	= true,
});

#if CONFIG(EC_STARLABS_FAST_CHARGE)
static const struct sm_object fast_charge = SM_DECLARE_BOOL({
	.opt_name	= "fast_charge",
	.ui_name	= "Fast Charge",
	.ui_helptext	= "Charge the battery faster at the cost of heat and battery wear.",
	.default_value	= false,
});
#endif

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
		&power_profile,
		NULL
	},
};

static struct sm_obj_form processor = {
	.ui_name = "Processor",
	.obj_list = (const struct sm_object *[]) {
		&vtd,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(EC_STARLABS_FAST_CHARGE)
		&fast_charge,
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
		&microphone,
		&webcam,
		&wireless,
		NULL
	},
};

static struct sm_obj_form pci = {
	.ui_name = "PCI",
	.obj_list = (const struct sm_object *[]) {
		#if CONFIG(SOC_INTEL_ALDERLAKE)
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
		&debug_level,
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
