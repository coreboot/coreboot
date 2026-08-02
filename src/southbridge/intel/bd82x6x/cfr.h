/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for sb/bd82x6x
 */

#ifndef SB_BD82X6X_CFR_H
#define SB_BD82X6X_CFR_H

#include <drivers/option/cfr_frontend.h>
#include <southbridge/intel/common/pmutil.h>
#include "me.h"

#define DEFINE_RP_ENABLE_OPTION(_rp, _name)			\
static const struct sm_object pcie_rp##_rp = SM_DECLARE_BOOL({	\
	.opt_name	= "pch_pcie_enable_rp"#_rp,		\
	.ui_name	= _name,				\
	.ui_helptext	= "Enable or disable "#_name,		\
	.default_value	= true,					\
})

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
	SATA_MODE_IDE_COMPAT,
	SATA_MODE_IDE_LEGACY,
};

/* SATA controller mode */
static const struct sm_object sata_mode = SM_DECLARE_ENUM({
	.opt_name	= "sata_mode",
	.ui_name	= "SATA Mode",
	.ui_helptext	= "Specify mode of the SATA controller",
	.default_value	= SATA_MODE_AHCI,
	.values		= (const struct sm_enum_value[]) {
				{ "AHCI",		SATA_MODE_AHCI		},
				{ "IDE (compatible)",	SATA_MODE_IDE_COMPAT	},
				{ "IDE (legacy)",	SATA_MODE_IDE_LEGACY	},
				SM_ENUM_VALUE_END				},
});

enum {
	NMI_OFF,
	NMI_ON,
};

/* Non-maskable interrupts */
static const struct sm_object nmi = SM_DECLARE_ENUM({
	.opt_name	= "nmi",
	.ui_name	= "Non-maskable Interrupts",
	.ui_helptext	= "Enable or disable non-maskable interrupts",
	.default_value	= NMI_OFF,
	.values		= (const struct sm_enum_value[]) {
				{ "Disabled",	NMI_OFF	},
				{ "Enabled",	NMI_ON	},
				SM_ENUM_VALUE_END	},
});

enum sb_cfr_strings {
	ME_STRING_VERSION,
	ME_STRING_OPERATIONAL_STATE,
	ME_STRING_OPERATIONAL_MODE,
	ME_STRING_WORKING_STATE,
	ME_STRING_PROGRESS_PHASE,
};

#if ENV_RAMSTAGE && CONFIG(DRIVERS_OPTION_CFR)
void sb_cfr_set_string(enum sb_cfr_strings e, const char *value);
#else
static inline void sb_cfr_set_string(enum sb_cfr_strings e, const char *value) {}
#endif

#endif /* SB_BD82X6X_CFR_H */
