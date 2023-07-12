/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

static fsp_dxio_descriptor myst_dxio_descriptors[] = {
	[DXIO_WWAN] = {
		.engine_type = UNUSED_ENGINE,
		.port_present = true,
		.start_lane = 13,
		.end_lane = 13,
		.device_number = PCI_SLOT(WWAN_DEVFN),
		.function_number = PCI_FUNC(WWAN_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2,
	},
	[DXIO_WLAN] = {
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 14,
		.end_lane = 14,
		.device_number = PCI_SLOT(WLAN_DEVFN),
		.function_number = PCI_FUNC(WLAN_DEVFN),
		.link_speed_capability = GEN3,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
	},
	[DXIO_SD] = {
		.engine_type = PCIE_ENGINE,
		.port_present = true,
		.start_lane = 15,
		.end_lane = 15,
		.device_number = PCI_SLOT(SD_DEVFN),
		.function_number = PCI_FUNC(SD_DEVFN),
		.link_speed_capability = GEN1,
		.turn_off_unused_lanes = true,
		.link_hotplug = HOTPLUG_ENHANCED,
		.clk_req = CLK_REQ1,
	},
	[DXIO_STORAGE] = { 0 },
};

static const fsp_dxio_descriptor emmc_descriptor = {
	.engine_type = PCIE_ENGINE,
	.port_present = true,
	.start_lane = 16,
	.end_lane = 16,
	.device_number = PCI_SLOT(NVME_DEVFN),
	.function_number = PCI_FUNC(NVME_DEVFN),
	.link_speed_capability = GEN_MAX,
	.turn_off_unused_lanes = true,
	.clk_req = CLK_REQ3,
};

static const fsp_dxio_descriptor nvme_descriptor = {
	.engine_type = PCIE_ENGINE,
	.port_present = true,
	.start_lane = 16,
	.end_lane = 19,
	.device_number = PCI_SLOT(NVME_DEVFN),
	.function_number = PCI_FUNC(NVME_DEVFN),
	.link_speed_capability = GEN_MAX,
	.turn_off_unused_lanes = true,
	.clk_req = CLK_REQ3,
};

static const fsp_ddi_descriptor myst_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - HDMI/DP */
		.connector_type = DDI_HDMI,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - Unused */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	},
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	if (fw_config_is_provisioned() && fw_config_probe(FW_CONFIG(STORAGE, NVME))) {
		printk(BIOS_DEBUG, "Enabling NVMe.\n");
		myst_dxio_descriptors[DXIO_STORAGE] = nvme_descriptor;
	} else {
		printk(BIOS_DEBUG, "Enabling eMMC.\n");
		myst_dxio_descriptors[DXIO_STORAGE] = emmc_descriptor;
	}

	*dxio_descs = myst_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(myst_dxio_descriptors);
	*ddi_descs = myst_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(myst_ddi_descriptors);
}
