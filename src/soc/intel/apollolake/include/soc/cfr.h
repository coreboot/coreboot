/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs which are used to control SoC settings.
 */

#ifndef _APOLLOLAKE_CFR_H_
#define _APOLLOLAKE_CFR_H_

#include <drivers/option/cfr_frontend.h>
#include <soc/soc_chip.h>

/* Legacy 8254 Timer */
static const struct sm_object legacy_8254_timer = SM_DECLARE_BOOL({
	.opt_name	= "legacy_8254_timer",
	.ui_name	= "Legacy 8254 Timer",
	.ui_helptext	= "Enable the legacy 8254 timer by disabling clock gating.",
	.default_value	= false,
});

/* S0ix Enable */
static const struct sm_object s0ix_enable = SM_DECLARE_BOOL({
	.opt_name	= "s0ix_enable",
	.ui_name	= "Modern Standby (S0ix)",
	.ui_helptext	= "Enabled: use Modern Standby / S0ix. Disabled: use APCI S3 sleep",
	.default_value	= true,
});

/* VT-d */
static const struct sm_object vtd = SM_DECLARE_BOOL({
	.opt_name	= "vtd",
	.ui_name	= "VT-d",
	.ui_helptext	= "Enable or disable Intel VT-d (virtualization)",
	.default_value	= true,
});

#endif /* _APOLLOLAKE_CFR_H_ */
