/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_dxio_descriptor emmc_dxio_descriptors[] = {
	{
		/* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = PCI_SLOT(WLAN_DEVFN),
		.function_number = PCI_FUNC(WLAN_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.clk_req = CLK_REQ2,
	},
	{
		/* eMMC */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = PCI_SLOT(SD_DEVFN),
		.function_number = PCI_FUNC(SD_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.gpio_group_id = GPIO_6,
		.clk_req = CLK_REQ1,
	},
};

static const fsp_dxio_descriptor nvme_dxio_descriptors[] = {
	{
		/* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = PCI_SLOT(WLAN_DEVFN),
		.function_number = PCI_FUNC(WLAN_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.clk_req = CLK_REQ2,
	},
	{
		/* SSD */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 3,
		.device_number = PCI_SLOT(NVME_DEVFN),
		.function_number = PCI_FUNC(NVME_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.gpio_group_id = GPIO_6,
		.clk_req = CLK_REQ0,
	},
};

#define EMMC_CLKREQ_GPIO 115
void variant_get_dxio_descriptor(const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num)
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
		*dxio_num = ARRAY_SIZE(nvme_dxio_descriptors);
		*dxio_descs = nvme_dxio_descriptors;
	} else {
		printk(BIOS_DEBUG, "Enabling eMMC.\n");
		*dxio_num = ARRAY_SIZE(emmc_dxio_descriptors);
		*dxio_descs = emmc_dxio_descriptors;
	}
}
