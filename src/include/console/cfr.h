/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for console
 */

#ifndef _CONSOLE_CFR_H_
#define _CONSOLE_CFR_H_

#include <drivers/option/cfr_frontend.h>

const struct sm_object debug_level = SM_DECLARE_ENUM({
	.opt_name	= "debug_level",
	.ui_name	= "Console Log Level",
	.ui_helptext	= "Set the verbosity of the coreboot console output.",
	.default_value	= CONFIG_DEFAULT_CONSOLE_LOGLEVEL,
	.values		= (const struct sm_enum_value[]) {
				{ "Emergency",		0		},
				{ "Alert",		1		},
				{ "Critical",		2		},
				{ "Error",		3		},
				{ "Warning",		4		},
				{ "Notice",		5		},
				{ "Info",		6		},
				{ "Debug",		7		},
				{ "Spew",		8		},
				SM_ENUM_VALUE_END			},
});

#endif /* _CONSOLE_CFR_H_ */
