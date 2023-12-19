/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/port_descriptors.h>
#include <console/console.h>
#include <fw_config.h>
#include <soc/platform_descriptors.h>

enum baseboard_dxio_port_id {
	BASEBOARD_DXIO_WWAN,
	BASEBOARD_DXIO_WLAN,
	BASEBOARD_DXIO_SD,
	BASEBOARD_DXIO_STORAGE,
};

static fsp_dxio_descriptor myst_dxio_descriptors[] = {
	[BASEBOARD_DXIO_WWAN] = WWAN_DXIO_DESCRIPTOR,
	[BASEBOARD_DXIO_WLAN] = WLAN_DXIO_DESCRIPTOR,
	[BASEBOARD_DXIO_SD] = SD_DXIO_DESCRIPTOR,
	/* This value modified at runtime, default to emmc */
	[BASEBOARD_DXIO_STORAGE] = EMMC_DXIO_DESCRIPTOR,
};

__weak void variant_get_dxio_descriptors(const fsp_dxio_descriptor **dxio_descriptor, size_t *num)
{
	if (fw_config_is_provisioned() && fw_config_probe(FW_CONFIG(STORAGE, NVME))) {
		printk(BIOS_DEBUG, "Enabling NVMe.\n");
		myst_dxio_descriptors[BASEBOARD_DXIO_STORAGE] = (fsp_dxio_descriptor)NVME_DXIO_DESCRIPTOR;
	} else {
		printk(BIOS_DEBUG, "Defaulting to eMMC.\n");
	}
	*dxio_descriptor = myst_dxio_descriptors;
	*num = ARRAY_SIZE(myst_dxio_descriptors);
}
