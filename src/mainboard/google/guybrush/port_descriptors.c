/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <device/pci_def.h>
#include <soc/platform_descriptors.h>
#include <soc/gpio.h>
#include <types.h>

/* All PCIe Resets are handled in coreboot */
static const fsp_dxio_descriptor guybrush_czn_pci_wwan_descriptors[] = {
	{ /* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = PCI_SLOT(WLAN_DEVFN),
		.function_number = PCI_FUNC(WLAN_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* SD */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = PCI_SLOT(SD_DEVFN),
		.function_number = PCI_FUNC(SD_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* WWAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 2,
		.device_number = PCI_SLOT(WWAN_DEVFN),
		.function_number = PCI_FUNC(WWAN_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* NVME */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = PCI_SLOT(NVME_DEVFN),
		.function_number = PCI_FUNC(NVME_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ3,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* TODO: remove this temporary workaround */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 8,
		.end_logical_lane = 11,
		.device_number = PCIE_GPP_BRIDGE_2_DEV,
		.function_number = PCIE_GPP_2_4_FUNC,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* TODO: remove this temporary workaround */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 16,
		.end_logical_lane = 23,
		.device_number = PCIE_GPP_BRIDGE_1_DEV,
		.function_number = PCIE_GPP_1_0_FUNC,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	}
};

/* All PCIe Resets are handled in coreboot */
static const fsp_dxio_descriptor guybrush_czn_dxio_descriptors[] = {
	{ /* WLAN */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = PCI_SLOT(WLAN_DEVFN),
		.function_number = PCI_FUNC(WLAN_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* SD */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = PCI_SLOT(SD_DEVFN),
		.function_number = PCI_FUNC(SD_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* WWAN */
		.engine_type = UNUSED_ENGINE,
		.port_present = true,
		.start_logical_lane = 2,
		.end_logical_lane = 2,
		.device_number = PCI_SLOT(WWAN_DEVFN),
		.function_number = PCI_FUNC(WWAN_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* NVME */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 4,
		.end_logical_lane = 7,
		.device_number = PCI_SLOT(NVME_DEVFN),
		.function_number = PCI_FUNC(NVME_DEVFN),
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ3,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* TODO: remove this temporary workaround */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 8,
		.end_logical_lane = 11,
		.device_number = PCIE_GPP_BRIDGE_2_DEV,
		.function_number = PCIE_GPP_2_4_FUNC,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ5,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	},
	{ /* TODO: remove this temporary workaround */
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_logical_lane = 16,
		.end_logical_lane = 23,
		.device_number = PCIE_GPP_BRIDGE_1_DEV,
		.function_number = PCIE_GPP_1_0_FUNC,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ6,
		.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}
	}
};

/* TODO: verify the DDI table, since this is mostly an educated guess right now */
static const fsp_ddi_descriptor guybrush_czn_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - HDMI */
		.connector_type = DDI_HDMI,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI4 - DP (type C) */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	if (variant_has_pcie_wwan()) {
		*dxio_descs = guybrush_czn_pci_wwan_descriptors;
		*dxio_num = ARRAY_SIZE(guybrush_czn_pci_wwan_descriptors);
	} else {
		*dxio_descs = guybrush_czn_dxio_descriptors;
		*dxio_num = ARRAY_SIZE(guybrush_czn_dxio_descriptors);
	}

	*ddi_descs = guybrush_czn_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(guybrush_czn_ddi_descriptors);
}
