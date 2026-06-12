/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for sb/i82801hx
 */

#ifndef SB_INTEL_I82801HX_CFR_H
#define SB_INTEL_I82801HX_CFR_H

#include <drivers/option/cfr_frontend.h>
#include <southbridge/intel/common/pmutil.h>

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

enum {
	SATA_MODE_AHCI,
	SATA_MODE_IDE_NATIVE,
};

/* SATA controller mode */
static const struct sm_object sata_mode = SM_DECLARE_ENUM({
	.opt_name	= "sata_mode",
	.ui_name	= "SATA Mode",
	.ui_helptext	= "Specify mode of the SATA controller.",
	.default_value	= SATA_MODE_AHCI,
	.values		= (const struct sm_enum_value[]) {
				{ "AHCI", SATA_MODE_AHCI		},
				{ "IDE",  SATA_MODE_IDE_NATIVE	},
				SM_ENUM_VALUE_END			},
});

enum {
	NMI_OFF,
	NMI_ON,
};

/* Non-maskable interrupts */
static const struct sm_object nmi = SM_DECLARE_ENUM({
	.opt_name	= "nmi",
	.ui_name	= "Non-maskable Interrupts",
	.ui_helptext	= "Enable or disable non-maskable interrupts.",
	.default_value	= NMI_ON,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled", NMI_OFF	},
				{ "Enabled",  NMI_ON	},
				SM_ENUM_VALUE_END	},
});

#endif /* SB_INTEL_I82801HX_CFR_H */
