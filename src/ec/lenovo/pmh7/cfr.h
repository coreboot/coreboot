/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for Lenovo PMH7 EC
 */

#ifndef _LENOVO_PMH7_CFR_H_
#define _LENOVO_PMH7_CFR_H_

#include <drivers/option/cfr_frontend.h>

/* Touchpad */
static const struct sm_object touchpad = SM_DECLARE_BOOL({
	.opt_name	= "touchpad",
	.ui_name	= "Touchpad",
	.ui_helptext	= "Enable or disable the touchpad",
	.default_value	= true,
});

/* Trackpoint */
static const struct sm_object trackpoint = SM_DECLARE_BOOL({
	.opt_name	= "trackpoint",
	.ui_name	= "Trackpoint",
	.ui_helptext	= "Enable or disable the trackpoint",
	.default_value	= true,
});

#endif /* _LENOVO_PMH7_CFR_H_ */
