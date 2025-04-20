/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for nb/sandybridge
 */

#ifndef NB_SANDYBRIDGE_CFR_H
#define NB_SANDYBRIDGE_CFR_H

#include <drivers/option/cfr_frontend.h>

/* Values must match nb/sandybridge/Kconfig */
enum {
	IGD_UMA_SIZE_32MB,
	IGD_UMA_SIZE_64MB,
	IGD_UMA_SIZE_96MB,
	IGD_UMA_SIZE_128MB,
};

/* IGD UMA Size */
static const struct sm_object gfx_uma_size = SM_DECLARE_ENUM({
	.opt_name	= "gfx_uma_size",
	.ui_name	= "IGD UMA Size",
	.ui_helptext	= "Size of memory preallocated for internal graphics.",
	.default_value	= CONFIG_IGD_DEFAULT_UMA_INDEX,
	.values		= (const struct sm_enum_value[]) {
				{ " 32MB", IGD_UMA_SIZE_32MB	},
				{ " 64MB", IGD_UMA_SIZE_64MB	},
				{ " 96MB", IGD_UMA_SIZE_96MB	},
				{ "128MB", IGD_UMA_SIZE_128MB	},
				SM_ENUM_VALUE_END		},
});

#endif /* NB_SANDYBRIDGE_CFR_H */
