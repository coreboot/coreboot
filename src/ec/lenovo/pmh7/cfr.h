/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for Lenovo PMH7 EC
 */

#ifndef _LENOVO_PMH7_CFR_H_
#define _LENOVO_PMH7_CFR_H_

#include <drivers/option/cfr_frontend.h>

/* Touchpad */
static const struct sm_object touchpad = SM_DECLARE_ENUM({
	.opt_name	= "touchpad",
	.ui_name	= "Touchpad",
	.ui_helptext	= "Enable or disable the touchpad",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

/* Trackpoint */
static const struct sm_object trackpoint = SM_DECLARE_ENUM({
	.opt_name	= "trackpoint",
	.ui_name	= "Trackpoint",
	.ui_helptext	= "Enable or disable the trackpoint",
	.default_value	= 1,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	0	},
				{ "Enabled",	1	},
				SM_ENUM_VALUE_END	},
});

#endif /* _LENOVO_PMH7_CFR_H_ */
