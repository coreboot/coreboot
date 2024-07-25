/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_PORT_DESCRIPTORS_H__
#define __BASEBOARD_PORT_DESCRIPTORS_H__

#define WLAN_DEVFN PCIE_GPP_2_0_DEVFN
#define SD_DEVFN PCIE_GPP_2_1_DEVFN
#define NVME_DEVFN PCIE_GPP_2_2_DEVFN

#define WLAN_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_logical_lane = 0,                 \
	.end_logical_lane = 0,                   \
	.device_number = PCI_SLOT(WLAN_DEVFN),   \
	.function_number = PCI_FUNC(WLAN_DEVFN), \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.link_aspm = ASPM_L1,                    \
	.link_aspm_L1_1 = true,                  \
	.link_aspm_L1_2 = true,                  \
	.clk_req = CLK_REQ2,                     \
}

#define SD_DXIO_DESCRIPTOR {                     \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_logical_lane = 1,                 \
	.end_logical_lane = 1,                   \
	.device_number = PCI_SLOT(SD_DEVFN),     \
	.function_number = PCI_FUNC(SD_DEVFN),   \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.link_hotplug = HOTPLUG_ENHANCED,        \
	.link_aspm = ASPM_L1,                    \
	.link_aspm_L1_1 = true,                  \
	.gpio_group_id = GPIO_27,                \
	.clk_req = CLK_REQ1,                     \
}

#define NVME_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_logical_lane = 2,                 \
	.end_logical_lane = 3,                   \
	.device_number = PCI_SLOT(NVME_DEVFN),   \
	.function_number = PCI_FUNC(NVME_DEVFN), \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.link_aspm = ASPM_L1,                    \
	.link_aspm_L1_1 = true,                  \
	.link_aspm_L1_2 = true,                  \
	.gpio_group_id = GPIO_6,                 \
	.clk_req = CLK_REQ0,                     \
}

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

#endif //__BASEBOARD_PORT_DESCRIPTORS_H__
