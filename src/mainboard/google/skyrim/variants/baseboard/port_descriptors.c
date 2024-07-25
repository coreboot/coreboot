/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/port_descriptors.h>
#include <soc/platform_descriptors.h>
#include <console/console.h>

enum emmc_dxio_port_id {
	EMMC_DXIO_WLAN,
	EMMC_DXIO_STORAGE,
};

static fsp_dxio_descriptor emmc_dxio_descriptors[] = {
	[EMMC_DXIO_WLAN] = WLAN_DXIO_DESCRIPTOR,
	/* This value modified at runtime, default to emmc */
	[EMMC_DXIO_STORAGE] = EMMC_DXIO_DESCRIPTOR,
};

#define EMMC_CLKREQ_GPIO 115

enum baseboard_dxio_port_id {
	BASEBOARD_DXIO_WLAN,
	BASEBOARD_DXIO_SD,
	BASEBOARD_DXIO_STORAGE,
};

static fsp_dxio_descriptor skyrim_mdn_dxio_descriptors[] = {
	[BASEBOARD_DXIO_WLAN] = WLAN_DXIO_DESCRIPTOR,
	[BASEBOARD_DXIO_SD] = SD_DXIO_DESCRIPTOR,
	[BASEBOARD_DXIO_STORAGE] = NVME_DXIO_DESCRIPTOR,
};

void variant_get_dxio_descriptors(const fsp_dxio_descriptor **dxio_descriptor, size_t *num)
{
	if (CONFIG(BOARD_GOOGLE_MARKARTH) || CONFIG(BOARD_GOOGLE_WINTERHOLD)) {
		if (gpio_get(EMMC_CLKREQ_GPIO)) {
			printk(BIOS_DEBUG, "Enabling NVMe.\n");
			emmc_dxio_descriptors[EMMC_DXIO_STORAGE] =
				(fsp_dxio_descriptor)NVME_DXIO_DESCRIPTOR;
		} else {
			printk(BIOS_DEBUG, "Defaulting to eMMC.\n");
		}
		*dxio_descriptor = emmc_dxio_descriptors;
		*num = ARRAY_SIZE(emmc_dxio_descriptors);
	} else {
		*dxio_descriptor = skyrim_mdn_dxio_descriptors;
		*num = ARRAY_SIZE(skyrim_mdn_dxio_descriptors);
	}
}
