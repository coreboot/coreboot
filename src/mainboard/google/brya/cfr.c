/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <intelblocks/cfr.h>
#include <soc/cfr.h>

#if CONFIG(BOARD_GOOGLE_TANIKS) || CONFIG(BOARD_GOOGLE_TAEKO)
#include <fw_config.h>

enum storage_device {
	STORAGE_NVME = 0,
	STORAGE_EMMC = 1,
};

/* Update storage device default value based on fw_config */
static void update_storage_device(struct sm_object *new)
{
	new->sm_enum.default_value =
		(fw_config_get() & FW_CONFIG_FIELD_BOOT_EMMC_MASK_MASK) ?
		STORAGE_EMMC : STORAGE_NVME;
}

static const struct sm_object storage_device_opt = SM_DECLARE_ENUM({
	.opt_name	= "storage_device",
	.ui_name	= "Storage Device",
	.ui_helptext	= "Select which storage device to use (NVMe SSD or eMMC)",
	.default_value	= STORAGE_NVME,
	.values		= (const struct sm_enum_value[]) {
				{ "NVMe SSD",		STORAGE_NVME	},
				{ "eMMC",		STORAGE_EMMC	},
				SM_ENUM_VALUE_END			},
}, WITH_CALLBACK(update_storage_device));

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&storage_device_opt,
		NULL
	},
};
#endif

static struct sm_obj_form system = {
	.ui_name = "System",
	.obj_list = (const struct sm_object *[]) {
		&hyper_threading,
		&igd_dvmt,
		&igd_aperture,
		&legacy_8254_timer,
		&me_state,
		&me_state_counter,
		&disable_heci1_at_pre_boot,
		&pciexp_aspm,
		&pciexp_clk_pm,
		&pciexp_l1ss,
		&pciexp_speed,
		&s0ix_enable,
		&vtd,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&system,
#if CONFIG(BOARD_GOOGLE_TANIKS) || CONFIG(BOARD_GOOGLE_TAEKO)
	&devices,
#endif
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
