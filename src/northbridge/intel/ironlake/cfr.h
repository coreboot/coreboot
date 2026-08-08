/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs for nb/ironlake
 */

#ifndef NB_INTEL_IRONLAKE_CFR_H
#define NB_INTEL_IRONLAKE_CFR_H

#include <drivers/option/cfr_frontend.h>

enum {
	IRONLAKE_IGD_UMA_SIZE_32MB,
	IRONLAKE_IGD_UMA_SIZE_48MB,
	IRONLAKE_IGD_UMA_SIZE_64MB,
	IRONLAKE_IGD_UMA_SIZE_128MB,
	IRONLAKE_IGD_UMA_SIZE_256MB,
	IRONLAKE_IGD_UMA_SIZE_96MB,
	IRONLAKE_IGD_UMA_SIZE_160MB,
	IRONLAKE_IGD_UMA_SIZE_224MB,
	IRONLAKE_IGD_UMA_SIZE_352MB,
};

/* IGD UMA Size */
static const struct sm_object gfx_uma_size = SM_DECLARE_ENUM({
	.opt_name	= "gfx_uma_size",
	.ui_name	= "IGD UMA Size",
	.ui_helptext	= "Size of memory preallocated for internal graphics.",
	.default_value	= IRONLAKE_IGD_UMA_SIZE_32MB,
	.values		= (const struct sm_enum_value[]) {
				{ " 32MB", IRONLAKE_IGD_UMA_SIZE_32MB	},
				{ " 48MB", IRONLAKE_IGD_UMA_SIZE_48MB	},
				{ " 64MB", IRONLAKE_IGD_UMA_SIZE_64MB	},
				{  "96MB", IRONLAKE_IGD_UMA_SIZE_96MB	},
				{ "128MB", IRONLAKE_IGD_UMA_SIZE_128MB	},
				{ "160MB", IRONLAKE_IGD_UMA_SIZE_160MB	},
				{ "224MB", IRONLAKE_IGD_UMA_SIZE_224MB	},
				{ "256MB", IRONLAKE_IGD_UMA_SIZE_256MB	},
				{ "352MB", IRONLAKE_IGD_UMA_SIZE_352MB	},
				SM_ENUM_VALUE_END			},
});

#endif /* NB_INTEL_IRONLAKE_CFR_H */
