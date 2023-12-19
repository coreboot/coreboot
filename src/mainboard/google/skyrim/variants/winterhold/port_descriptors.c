/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <baseboard/port_descriptors.h>
#include <console/console.h>
#include <soc/platform_descriptors.h>

enum winterhold_dxio_port_id {
	WINTERHOLD_DXIO_WLAN,
	WINTERHOLD_DXIO_STORAGE,
};

#define EMMC_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_logical_lane = 1,                 \
	.end_logical_lane = 1,                   \
	.device_number = PCI_SLOT(SD_DEVFN),     \
	.function_number = PCI_FUNC(SD_DEVFN),   \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.link_aspm = ASPM_L1,                    \
	.link_aspm_L1_1 = true,                  \
	.link_aspm_L1_2 = true,                  \
	.gpio_group_id = GPIO_6,                 \
	.clk_req = CLK_REQ1,                     \
}

#define EMMC_CLKREQ_GPIO 115

static fsp_dxio_descriptor winterhold_dxio_descriptors[] = {
	[WINTERHOLD_DXIO_WLAN] = WLAN_DXIO_DESCRIPTOR,
	/* This value modified at runtime, default to emmc */
	[WINTERHOLD_DXIO_STORAGE] = EMMC_DXIO_DESCRIPTOR,
};

void variant_get_dxio_descriptors(const fsp_dxio_descriptor **dxio_descriptor, size_t *num)
{
	/*
	 * We can determine if a device is populated based on the state of the clkreq
	 * signal. If the device is present, the clkreq is held low by the device. If
	 * no device is present, clkreq is pulled high by an external pull-up.
	 *
	 * This allows checking the state of the NVMe device clkreq signal and enabling
	 * either eMMC or NVMe based on that.
	 */
	if (gpio_get(EMMC_CLKREQ_GPIO)) {
		printk(BIOS_DEBUG, "Enabling NVMe.\n");
		winterhold_dxio_descriptors[WINTERHOLD_DXIO_STORAGE] = (fsp_dxio_descriptor)NVME_DXIO_DESCRIPTOR;
	} else {
		printk(BIOS_DEBUG, "Defaulting to eMMC.\n");
	}
	*dxio_descriptor = winterhold_dxio_descriptors;
	*num = ARRAY_SIZE(winterhold_dxio_descriptors);
}
