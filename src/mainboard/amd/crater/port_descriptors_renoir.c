/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <soc/cpu.h>
#include <types.h>
#include <amdblocks/cpu.h>
#include <soc/pci_devs.h>
#include <console/console.h>
#include <ec/acpi/ec.h>
#include <stdint.h>

#define ECRAM_MACID_OFFSET 0x54
#define MACID_LEN 12

#define ECRAM_BOARDID_OFFSET 0x93

#define CRATER_EC_CMD	0x666
#define CRATER_EC_DATA	0x662

#define CRATER_REVB     0x42

#define mxm_dxio_descriptor {	\
	.engine_type = PCIE_ENGINE, \
	.port_present = true, \
	.start_logical_lane = 16, \
	.end_logical_lane = 23, \
	.device_number = 1, \
	.function_number = 1, \
	.link_aspm = ASPM_L1, \
	.link_aspm_L1_1 = true, \
	.link_aspm_L1_2 = true, \
	.turn_off_unused_lanes = false, \
	.clk_req = CLK_REQ0, \
	.gpio_group_id = GPIO_4, \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}, \
}

#define ssd_dxio_descriptor { \
	.engine_type = PCIE_ENGINE, \
	.port_present = true, \
	.start_logical_lane = 8, \
	.end_logical_lane = 11, \
	.device_number = 2, \
	.function_number = 4, \
	.link_aspm = ASPM_DISABLED, \
	.link_aspm_L1_1 = false, \
	.link_aspm_L1_2 = false, \
	.turn_off_unused_lanes = false, \
	.clk_req = CLK_REQ3, \
	.gpio_group_id = GPIO_27, \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122}, \
}

#define dt_dxio_descriptor {  \
	.engine_type = PCIE_ENGINE,  \
	.port_present = true,  \
	.start_logical_lane = 0,  \
	.end_logical_lane = 3,  \
	.device_number = 1,  \
	.function_number = 2,  \
	.link_aspm = ASPM_L1,  \
	.link_aspm_L1_1 = true,  \
	.link_aspm_L1_2 = true,  \
	.turn_off_unused_lanes = false,  \
	.clk_req = CLK_REQ5,  \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122},  \
}

#define wwan_dxio_descriptor {	\
	.engine_type = PCIE_ENGINE,  \
	.port_present = true,  \
	.start_logical_lane = 2,  \
	.end_logical_lane = 2,  \
	.device_number = 1,  \
	.function_number = 3,  \
	.link_aspm = ASPM_L1,  \
	.link_aspm_L1_1 = true,  \
	.link_aspm_L1_2 = true,  \
	.turn_off_unused_lanes = false,  \
	.clk_req = CLK_REQ2,  \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122},  \
}

#define wlan_dxio_descriptor {  \
	.engine_type = PCIE_ENGINE,  \
	.port_present = true,  \
	.start_logical_lane = 3,  \
	.end_logical_lane = 3,  \
	.device_number = 2,  \
	.function_number = 2,  \
	.link_aspm = ASPM_L1,  \
	.link_aspm_L1_1 = true,  \
	.link_aspm_L1_2 = true,  \
	.turn_off_unused_lanes = false,  \
	.clk_req = CLK_REQ6,  \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122},  \
}

#define tb_dxio_descriptor {  \
	.engine_type = PCIE_ENGINE,  \
	.port_present = true,  \
	.start_logical_lane = 4,  \
	.end_logical_lane = 7,  \
	.device_number = 2,  \
	.function_number = 3,  \
	.link_aspm = ASPM_L1,  \
	.link_aspm_L1_1 = true,  \
	.link_aspm_L1_2 = true,  \
	.turn_off_unused_lanes = false,  \
	.clk_req = CLK_REQ4_GFX,  \
	.port_params = {PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122},  \
}

/* XGBE ETHERNET PORTS Entry Port 0 */
// XGBE SGMII interface: Physical lane 4, Logical lane 0
// NOTE: Ancillary data not yet captured here due to FSP limitations
#define xgbe_port0_dxio_descriptor {  \
	.port_present = true,  \
	.engine_type = ETHERNET_ENGINE,  \
	.start_logical_lane = 0,  \
	.end_logical_lane = 0,  \
}

/* XGBE ETHERNET PORTS Entry Port 1 */
// XGBE SGMII interface: Physical lane 5, Logical lane 1
// NOTE: Ancillary data not yet captured here due to FSP limitations
#define xgbe_port1_dxio_descriptor {  \
	.port_present = true,  \
	.engine_type = ETHERNET_ENGINE,  \
	.start_logical_lane = 1,  \
	.end_logical_lane = 1,  \
}

static fsp_ddi_descriptor crater_ddi_descriptors[] = {
	{ /* DDI0 - DP */
		.connector_type = DDI_DP,
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
	uint8_t BoardRev;

	if ((get_cpu_count() == 4 && get_threads_per_core() == 2) || get_cpu_count() == 2)
		crater_ddi_descriptors[1].connector_type = DDI_UNUSED_TYPE;

	ec_set_ports(CRATER_EC_CMD, CRATER_EC_DATA);
	BoardRev = ec_read(ECRAM_BOARDID_OFFSET + 0x3);

	if (CONFIG(ENABLE_EDP)) {
		crater_ddi_descriptors[1].connector_type = DDI_EDP;
	} else {
		if (BoardRev == CRATER_REVB)
			crater_ddi_descriptors[1].connector_type = DDI_DP;
		else
			crater_ddi_descriptors[1].connector_type = DDI_HDMI;
	}

	static const fsp_dxio_descriptor crater_dxio_descriptors[] = {
		mxm_dxio_descriptor,
		ssd_dxio_descriptor,
		tb_dxio_descriptor,
#if CONFIG(ETH_AIC_SLOT_ONLY)
		dt_dxio_descriptor, // GPP 0~3
#endif
#if CONFIG(XGBE_WWAN_WLAN)
		xgbe_port0_dxio_descriptor, // GPP 0
		xgbe_port1_dxio_descriptor, // GPP 1
		wwan_dxio_descriptor, // GPP 2
		wlan_dxio_descriptor // GPP 3
#endif
	};

	*dxio_descs = crater_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(crater_dxio_descriptors);
	*ddi_descs = crater_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(crater_ddi_descriptors);
}
