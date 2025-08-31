/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <drivers/option/cfr_frontend.h>
#include <console/cfr.h>
#include <northbridge/intel/sandybridge/cfr.h>
#include <southbridge/intel/bd82x6x/cfr.h>

#if CONFIG(USE_NATIVE_RAMINIT)
static struct sm_obj_form sysagent = {
	.ui_name = "Graphics Configuration",
	.obj_list = (const struct sm_object *[]) {
		&gfx_uma_size,
		NULL
	},
};
#else
static const struct sm_object usb3_mode = SM_DECLARE_ENUM({
	.opt_name	= "usb3_mode",
	.ui_name	= "Intel xHCI Mode",
	.default_value	= 3,
	.values		= (const struct sm_enum_value[]) {
				{ "Disable",	0	},
				{ "Enable",	1	},
				{ "Auto",	2	},
				{ "Smart Auto",	3	},
				SM_ENUM_VALUE_END	},
});

static const struct sm_object usb3_drv = SM_DECLARE_BOOL({
	.opt_name	= "usb3_drv",
	.ui_name	= "Legacy USB3.0 Support",
	.ui_helptext	= "Enable/Disable USB3.0 (XHCI) Controller Legacy Support.",
	.default_value	= 1,
});

static const struct sm_object usb3_streams = SM_DECLARE_BOOL({
	.opt_name	= "usb3_streams",
	.ui_name	= "xHCI Streams",
	.default_value	= 1,
});

static struct sm_obj_form sysagent = {
	.ui_name = "System Agent Configuration",
	.obj_list = (const struct sm_object *[]) {
		&gfx_uma_size,
		&usb3_mode,
		&usb3_drv,
		&usb3_streams,
		NULL
	},
};
#endif

/* Front audio panel */
static const struct sm_object audio_panel_type = SM_DECLARE_ENUM({
	.opt_name	= "audio_panel_type",
	.ui_name	= "Front Panel Type",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "HD",	0	},
				{ "AC97", 1	},
				SM_ENUM_VALUE_END },
});

#if CONFIG(BOARD_ASUS_P8Z77_V_LE_PLUS)
static const struct sm_object spdif_dest = SM_DECLARE_ENUM({
	.opt_name	= "spdif_dest",
	.ui_name	= "SPDIF Out Type",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "SPDIF",	0	},
				{ "HDMI",	1	},
				SM_ENUM_VALUE_END	},
});
#endif

static struct sm_obj_form onboard_devices = {
	.ui_name = "Onboard Devices Configuration",
	.obj_list = (const struct sm_object *[]) {
		&sata_mode,
		&audio_panel_type,
#if CONFIG(BOARD_ASUS_P8Z77_V_LE_PLUS)
		&spdif_dest,
#endif
		NULL
	},
};

static struct sm_obj_form power = {
	.ui_name = "Power Management",
	.obj_list = (const struct sm_object *[]) {
		&power_on_after_fail,
		NULL
	},
};

static struct sm_obj_form system = {
	.ui_name = "Other System Settings",
	.obj_list = (const struct sm_object *[]) {
		&nmi,
		&debug_level,
		NULL
	},
};

static struct sm_obj_form *sm_root[] = {
	&sysagent,
	&onboard_devices,
	&power,
	&system,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	cfr_write_setup_menu(cfr_root, sm_root);
}
