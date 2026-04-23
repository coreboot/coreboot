/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

#define BYTE_DUMMY_DXIO_DESCRIPTOR {					\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 16,					\
	.end_logical_lane	= 23,					\
	.device_number		= 1,					\
	.function_number	= 1,					\
	.clk_req		= CLK_REQ0,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define BYTE_M2_NVME_DXIO_DESCRIPTOR {					\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 0,					\
	.end_logical_lane	= 3,					\
	.link_speed_capability	= GEN_MAX,				\
	.device_number		= 2,					\
	.function_number	= 4,					\
	.link_aspm		= ASPM_DISABLED,			\
	.link_aspm_L1_1		= false,				\
	.link_aspm_L1_2		= false,				\
	.turn_off_unused_lanes	= true,					\
	.clk_req		= CLK_REQ5,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define BYTE_M2_SATA_DXIO_DESCRIPTOR {					\
	.engine_type		= SATA_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 2,					\
	.end_logical_lane	= 3,					\
	.gpio_group_id		= 1,					\
	.channel_type		= SATA_CHANNEL_LONG,			\
}

#define BYTE_LAN_DXIO_DESCRIPTOR {					\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 6,					\
	.end_logical_lane	= 6,					\
	.link_speed_capability	= GEN_MAX,				\
	.device_number		= 2,					\
	.function_number	= 1,					\
	.link_aspm		= ASPM_DISABLED,			\
	.link_aspm_L1_1		= false,				\
	.link_aspm_L1_2		= false,				\
	.turn_off_unused_lanes	= true,					\
	.clk_req		= CLK_REQ1,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define BYTE_WIFI_DXIO_DESCRIPTOR {					\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 7,					\
	.end_logical_lane	= 7,					\
	.link_speed_capability	= GEN_MAX,				\
	.device_number		= 2,					\
	.function_number	= 2,					\
	.link_aspm		= ASPM_DISABLED,			\
	.link_aspm_L1_1		= false,				\
	.link_aspm_L1_2		= false,				\
	.turn_off_unused_lanes	= true,					\
	.clk_req		= CLK_REQ6,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define BYTE_SATA_DXIO_DESCRIPTOR {					\
	.engine_type		= SATA_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 8,					\
	.end_logical_lane	= 9,					\
	.gpio_group_id		= 1,					\
	.channel_type		= SATA_CHANNEL_LONG,			\
}

enum byte_dxio_port_idx {
	BYTE_DXIO_DUMMY,
	BYTE_DXIO_M2_SSD_DESC = 1,
	BYTE_DXIO_LAN,
	BYTE_DXIO_WIFI,
	BYTE_DXIO_SATA_SSD,
};

static fsp_dxio_descriptor byte_dxio_descriptors[] = {
	[BYTE_DXIO_DUMMY]		= BYTE_DUMMY_DXIO_DESCRIPTOR,
	[BYTE_DXIO_M2_SSD_DESC]		= BYTE_M2_NVME_DXIO_DESCRIPTOR,
	[BYTE_DXIO_LAN]			= BYTE_LAN_DXIO_DESCRIPTOR,
	[BYTE_DXIO_WIFI]		= BYTE_WIFI_DXIO_DESCRIPTOR,
	[BYTE_DXIO_SATA_SSD]		= BYTE_SATA_DXIO_DESCRIPTOR,
};

static void byte_select_ssd_dxio_descriptor(void)
{
	fsp_dxio_descriptor *m2_ssd = &byte_dxio_descriptors[BYTE_DXIO_M2_SSD_DESC];

	gpio_input(GPIO_40);

	/*
	 * The Y1 M.2 socket exposes PEDET as "OC-PCIe" with the socket note:
	 * "PCIe SSD: NC / SATA SSD: GND". With the board-side pull-up, PCIe
	 * devices leave the line high while SATA devices pull it low.
	 */
	if (gpio_get(GPIO_40)) {
		printk(BIOS_INFO, "DXIO: detected PCIe SSD on lanes 0-3\n");
		return;
	}

	printk(BIOS_INFO, "DXIO: detected SATA SSD; routing lanes 2-3 to SATA\n");
	*m2_ssd = (fsp_dxio_descriptor)BYTE_M2_SATA_DXIO_DESCRIPTOR;
}

static fsp_ddi_descriptor byte_ddi_descriptors[] = {
	/* DDI0:	HDMI */
	{
		.connector_type		= DDI_HDMI,
		.aux_index		= DDI_AUX1,
		.hdp_index		= DDI_HDP1,
	},
	/* DDI1:	HDMI */
	{
		.connector_type		= DDI_HDMI,
		.aux_index		= DDI_AUX2,
		.hdp_index		= DDI_HDP2,
	},
	/* DDI2:	Not Used */
	{
		.connector_type		= DDI_UNUSED_TYPE,
		.aux_index		= DDI_AUX3,
		.hdp_index		= DDI_HDP3,
	},
	/* DDI3:	DisplayPort over USB-C */
	{
		.connector_type		= DDI_DP,
		.aux_index		= DDI_AUX3,
		.hdp_index		= DDI_HDP3,
	},
	/* DDI4:	Not Used */
	{
		.connector_type		= DDI_UNUSED_TYPE,
		.aux_index		= DDI_AUX4,
		.hdp_index		= DDI_HDP4,
	},
};

void mainboard_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					size_t *dxio_num, const fsp_ddi_descriptor **ddi_descs,
					size_t *ddi_num)
{
	byte_select_ssd_dxio_descriptor();

	*dxio_descs = byte_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(byte_dxio_descriptors);
	*ddi_descs = byte_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(byte_ddi_descriptors);
}
