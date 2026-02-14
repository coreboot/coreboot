
/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/lenovo/h8/cfr.h>
#include <ec/lenovo/pmh7/cfr.h>
#include <northbridge/intel/sandybridge/cfr.h>
#include <southbridge/intel/bd82x6x/cfr.h>

GEN_RP_ENUM(1, "Wifi")
#if CONFIG(BOARD_LENOVO_X220)
GEN_RP_ENUM(3, "ExpressCard")
#endif
GEN_RP_ENUM(4, "SD Card Reader")
GEN_RP_ENUM(6, "xHCI")

static struct sm_obj_form pcie = {
	.ui_name = "PCH PCIe",
	.obj_list = (const struct sm_object *[]) {
		&pcie_rp1,
#if CONFIG(BOARD_LENOVO_X220)
		&pcie_rp3,
#endif
		&pcie_rp4,
		&pcie_rp6,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&bluetooth,
		&usb_always_on,
		&wlan,
		&wwan,
		&pm_beeps,
		&battery_beep,
		&fn_ctrl_swap,
		&sticky_fn,
		&f1_to_f12_as_primary,
		&touchpad,
		&trackpoint,
		NULL
	},
};

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&gfx_uma_size,
		&me_state,
		&nmi,
		&sata_mode,
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
	&power,
	&ec,
	&pcie,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
