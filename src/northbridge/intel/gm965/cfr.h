/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for nb/gm965
 */

#ifndef NB_INTEL_GM965_CFR_H
#define NB_INTEL_GM965_CFR_H

#include <drivers/option/cfr_frontend.h>

enum {
	GM965_IGD_UMA_SIZE_1MB,
	GM965_IGD_UMA_SIZE_4MB,
	GM965_IGD_UMA_SIZE_8MB,
	GM965_IGD_UMA_SIZE_16MB,
	GM965_IGD_UMA_SIZE_32MB,
	GM965_IGD_UMA_SIZE_48MB,
	GM965_IGD_UMA_SIZE_64MB,
};

/* IGD UMA Size */
static const struct sm_object gfx_uma_size = SM_DECLARE_ENUM({
	.opt_name	= "gfx_uma_size",
	.ui_name	= "IGD UMA Size",
	.ui_helptext	= "Size of memory preallocated for internal graphics.",
	.default_value	= GM965_IGD_UMA_SIZE_32MB,
	.values		= (const struct sm_enum_value[]) {
				{ " 1MB", GM965_IGD_UMA_SIZE_1MB	},
				{ " 4MB", GM965_IGD_UMA_SIZE_4MB	},
				{ " 8MB", GM965_IGD_UMA_SIZE_8MB	},
				{ "16MB", GM965_IGD_UMA_SIZE_16MB	},
				{ "32MB", GM965_IGD_UMA_SIZE_32MB	},
				{ "48MB", GM965_IGD_UMA_SIZE_48MB	},
				{ "64MB", GM965_IGD_UMA_SIZE_64MB	},
				SM_ENUM_VALUE_END			},
});

#endif /* NB_INTEL_GM965_CFR_H */
