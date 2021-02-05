/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>

void __weak variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					     size_t *dxio_num,
					     const fsp_ddi_descriptor **ddi_descs,
					     size_t *ddi_num)
{
	*dxio_descs = baseboard_get_dxio_descriptors(dxio_num);
	*ddi_descs = baseboard_get_ddi_descriptors(ddi_num);
}

static const fsp_dxio_descriptor dxio_descriptors[] = {
	{
		// NVME SSD
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 4,
		.end_logical_lane = 5,
		.device_number = 1,
		.function_number = 7,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ2,
	},
	{
		// WLAN
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 0,
		.end_logical_lane = 0,
		.device_number = 1,
		.function_number = 2,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ0,
	},
	{
		// SD Reader
		.port_present = true,
		.engine_type = PCIE_ENGINE,
		.start_logical_lane = 1,
		.end_logical_lane = 1,
		.device_number = 1,
		.function_number = 3,
		.link_aspm = ASPM_L1,
		.link_aspm_L1_1 = true,
		.link_aspm_L1_2 = true,
		.turn_off_unused_lanes = true,
		.clk_req = CLK_REQ1,
	}
};

const fsp_dxio_descriptor *baseboard_get_dxio_descriptors(size_t *num)
{
	*num = ARRAY_SIZE(dxio_descriptors);
	return dxio_descriptors;
}

const fsp_ddi_descriptor *baseboard_get_ddi_descriptors(size_t *num)
{
	/* Different configurations of dalboz have different ddi configurations.
	 * Therefore, don't provide any baseboard defaults. */
	*num = 0;
	return NULL;
}
