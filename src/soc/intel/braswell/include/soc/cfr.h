/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * CFR enums and structs which are used to control SoC settings.
 */

#ifndef _BRASWELL_CFR_H_
#define _BRASWELL_CFR_H_

#include <device/device.h>
#include <drivers/option/cfr_frontend.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>

/* IGD Aperture Size */
static const struct sm_object igd_aperture = SM_DECLARE_ENUM({
	.opt_name	= "igd_aperture_size",
	.ui_name	= "IGD Aperture Size",
	.ui_helptext	= "Select the Aperture Size",
	.default_value	= IGD_AP_SZ_256MB,
	.values		= (const struct sm_enum_value[]) {
				{ "128 MB",	IGD_AP_SZ_128MB	},
				{ "256 MB",	IGD_AP_SZ_256MB	},
				{ "512 MB",	IGD_AP_SZ_512MB	},
				SM_ENUM_VALUE_END		},
});

/* Match CFR default to the board's PcdIgdDvmt50PreAlloc chip config. */
static void update_igd_dvmt_default(struct sm_object *new)
{
	const struct device *dev = pcidev_on_root(LPC_DEV, LPC_FUNC);
	const struct soc_intel_braswell_config *config;

	if (!dev)
		return;

	config = config_of(dev);
	new->sm_enum.default_value = config->PcdIgdDvmt50PreAlloc;
}

/* IGD DVMT pre-allocated memory */
static const struct sm_object igd_dvmt = SM_DECLARE_ENUM({
	.opt_name	= "igd_dvmt_prealloc",
	.ui_name	= "IGD DVMT Size",
	.ui_helptext	= "Size of memory preallocated for internal graphics",
	.default_value	= IGD_MEMSIZE_32MB,
	.values		= (const struct sm_enum_value[]) {
				{ " 32 MB",	IGD_MEMSIZE_32MB	},
				{ " 64 MB",	IGD_MEMSIZE_64MB	},
				{ " 96 MB",	IGD_MEMSIZE_96MB	},
				{ "128 MB",	IGD_MEMSIZE_128MB	},
				{ "160 MB",	IGD_MEMSIZE_160MB	},
				{ "192 MB",	IGD_MEMSIZE_192MB	},
				{ "224 MB",	IGD_MEMSIZE_224MB	},
				{ "256 MB",	IGD_MEMSIZE_256MB	},
				{ "288 MB",	IGD_MEMSIZE_288MB	},
				{ "320 MB",	IGD_MEMSIZE_320MB	},
				{ "352 MB",	IGD_MEMSIZE_352MB	},
				{ "384 MB",	IGD_MEMSIZE_384MB	},
				{ "416 MB",	IGD_MEMSIZE_416MB	},
				{ "448 MB",	IGD_MEMSIZE_448MB	},
				{ "480 MB",	IGD_MEMSIZE_480MB	},
				{ "512 MB",	IGD_MEMSIZE_512MB	},
				SM_ENUM_VALUE_END			},
}, WITH_CALLBACK(update_igd_dvmt_default));

#endif /* _BRASWELL_CFR_H_ */
