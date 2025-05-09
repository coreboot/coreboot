/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/starlabs/merlin/cfr.h>
#include <intelblocks/cfr.h>
#include "cfr.h"

static struct sm_obj_form performance = {
	.ui_name = "Performance",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth_rtd3,
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
		#if CONFIG(BOARD_SUPPORTS_HT)
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
		&power_on_after_fail_bool,
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
		#if CONFIG(BOARD_USES_FIXED_MODE_VBT)
		&display_native_res,
		#endif
		#if CONFIG(BOARD_HAS_FPR)
		&fingerprint_reader,
		#endif
		#if CONFIG(BOARD_HAS_GNA)
		&gna,
		#endif
		#if CONFIG(EC_STARLABS_LID_SWITCH)
		&lid_switch,
		#endif
		&microphone,
		#if CONFIG(DRIVERS_INTEL_USB4_RETIMER)
		&thunderbolt,
		#endif
		#if CONFIG(SOC_INTEL_METEORLAKE)
		&vpu,
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
		#if CONFIG(SOC_INTEL_COMMON_BLOCK_ASPM)
		&pciexp_clk_pm,
		&pciexp_aspm,
		#if CONFIG(HAS_INTEL_CPU_ROOT_PORTS)
		&pciexp_aspm_cpu,
		#endif
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
