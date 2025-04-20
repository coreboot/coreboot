/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for soc/intel/broadwell
 */

#ifndef _BROADWELL_CFR_H_
#define _BROADWELL_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include "pm.h"

/* Power state after power loss */
static const struct sm_object power_on_after_fail = SM_DECLARE_ENUM({
	.opt_name	= "power_on_after_fail",
	.ui_name	= "Restore AC Power Loss",
	.ui_helptext	= "Specify what to do when power is re-applied after a power loss.",
	.default_value	= CONFIG_MAINBOARD_POWER_FAILURE_STATE,
	.values		= (const struct sm_enum_value[]) {
				{ "Power off (S5)", MAINBOARD_POWER_OFF		},
				{ "Power on  (S0)", MAINBOARD_POWER_ON		},
				{ "Previous state", MAINBOARD_POWER_KEEP	},
				SM_ENUM_VALUE_END				},
});

/* Intel ME State */
static const struct sm_object me_disable = SM_DECLARE_ENUM({
	.opt_name	= "me_disable",
	.ui_name	= "Intel Management Engine",
	.ui_helptext	= "Enable or disable the PCI/HECI interface of the Intel Management Engine",
	.default_value	= 0,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	1	},
				{ "Enabled",	0	},
				SM_ENUM_VALUE_END	},
});

#endif /* _BROADWELL_CFR_H_ */
