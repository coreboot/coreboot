#ifndef __BASEBOARD_PORT_DESCRIPTORS_H__
#define __BASEBOARD_PORT_DESCRIPTORS_H__

#define WWAN_DEVFN PCIE_GPP_2_1_DEVFN
#define WLAN_DEVFN PCIE_GPP_2_2_DEVFN
#define SD_DEVFN PCIE_GPP_2_3_DEVFN
#define NVME_DEVFN PCIE_GPP_2_4_DEVFN

#define WWAN_DXIO_DESCRIPTOR {                   \
	.engine_type = UNUSED_ENGINE,            \
	.port_present = true,                    \
	.start_lane = 13,                        \
	.end_lane = 13,                          \
	.device_number = PCI_SLOT(WWAN_DEVFN),   \
	.function_number = PCI_FUNC(WWAN_DEVFN), \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.clk_req = CLK_REQ2,                     \
}

#define WLAN_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_lane = 14,                        \
	.end_lane = 14,                          \
	.device_number = PCI_SLOT(WLAN_DEVFN),   \
	.function_number = PCI_FUNC(WLAN_DEVFN), \
	.link_speed_capability = GEN3,           \
	.turn_off_unused_lanes = true,           \
	.clk_req = CLK_REQ0,                     \
}

#define SD_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,            \
	.port_present = true,                  \
	.start_lane = 15,                      \
	.end_lane = 15,                        \
	.device_number = PCI_SLOT(SD_DEVFN),   \
	.function_number = PCI_FUNC(SD_DEVFN), \
	.link_speed_capability = GEN1,         \
	.turn_off_unused_lanes = true,         \
	.link_hotplug = HOTPLUG_ENHANCED,      \
	.clk_req = CLK_REQ1,                   \
}

#define NVME_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_lane = 16,                        \
	.end_lane = 19,                          \
	.device_number = PCI_SLOT(NVME_DEVFN),   \
	.function_number = PCI_FUNC(NVME_DEVFN), \
	.link_speed_capability = GEN_MAX,        \
	.turn_off_unused_lanes = true,           \
	.clk_req = CLK_REQ3,                     \
}

#define EMMC_DXIO_DESCRIPTOR {                   \
	.engine_type = PCIE_ENGINE,              \
	.port_present = true,                    \
	.start_lane = 16,                        \
	.end_lane = 16,                          \
	.device_number = PCI_SLOT(NVME_DEVFN),   \
	.function_number = PCI_FUNC(NVME_DEVFN), \
	.link_speed_capability = GEN_MAX,        \
	.turn_off_unused_lanes = true,           \
	.clk_req = CLK_REQ3,                     \
}

#endif //__BASEBOARD_PORT_DESCRIPTORS_H__
