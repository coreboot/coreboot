/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <console/console.h>
#include <gpio.h>
#include <option.h>
#include <soc/gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

#define NVME_DET_GPIO GPIO_4

#define STARBOOK_WIFI_DXIO_DESCRIPTOR {					\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 6,					\
	.end_logical_lane	= 6,					\
	.link_speed_capability	= GEN_MAX,				\
	.device_number		= 2,					\
	.function_number	= 4,					\
	.link_aspm		= ASPM_L1,				\
	.link_aspm_L1_1		= false,				\
	.link_aspm_L1_2		= false,				\
	.turn_off_unused_lanes	= true,					\
	.clk_req		= CLK_REQ6,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define STARBOOK_M2_SATA_DXIO_DESCRIPTOR {				\
	.engine_type		= SATA_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 8,					\
	.end_logical_lane	= 9,					\
	.channel_type		= SATA_CHANNEL_LONG,			\
}

#define STARBOOK_M2_NVME_DXIO_DESCRIPTOR {				\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 8,					\
	.end_logical_lane	= 11,					\
	.link_speed_capability	= GEN_MAX,				\
	.device_number		= 2,					\
	.function_number	= 1,					\
	.link_aspm		= ASPM_L1,				\
	.link_aspm_L1_1		= true,					\
	.link_aspm_L1_2		= true,					\
	.turn_off_unused_lanes	= true,					\
	.clk_req		= CLK_REQ1,				\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

#define STARBOOK_DUMMY_MXM_DXIO_DESCRIPTOR {				\
	.engine_type		= PCIE_ENGINE,				\
	.port_present		= true,					\
	.start_logical_lane	= 16,					\
	.end_logical_lane	= 23,					\
	.turn_off_unused_lanes	= true,					\
	.port_params		= { PP_PSPP_AC, 0x133, PP_PSPP_DC, 0x122 },	\
}

enum {
	STARLABS_CFR_ASPM_DISABLE = 1,
	STARLABS_CFR_ASPM_L0S,
	STARLABS_CFR_ASPM_L1,
	STARLABS_CFR_ASPM_L0S_L1,
	STARLABS_CFR_ASPM_AUTO,
};

enum {
	STARLABS_CFR_L1SS_DISABLED = 1,
	STARLABS_CFR_L1SS_L1_1,
	STARLABS_CFR_L1SS_L1_2,
};

enum starbook_dxio_port_idx {
	STARBOOK_DXIO_WIFI,
	STARBOOK_DXIO_M2_STORAGE,
	STARBOOK_DXIO_DUMMY_MXM,
};

static dxio_descriptor starbook_dxio_descriptors[] = {
	[STARBOOK_DXIO_WIFI]		= STARBOOK_WIFI_DXIO_DESCRIPTOR,
	[STARBOOK_DXIO_M2_STORAGE]	= STARBOOK_M2_NVME_DXIO_DESCRIPTOR,
	[STARBOOK_DXIO_DUMMY_MXM]	= STARBOOK_DUMMY_MXM_DXIO_DESCRIPTOR,
};

static void starbook_set_dxio_aspm(dxio_descriptor *desc, unsigned int aspm)
{
	desc->link_aspm = ASPM_L1;

	switch (aspm) {
	case STARLABS_CFR_ASPM_DISABLE:
		desc->link_aspm = ASPM_DISABLED;
		break;
	case STARLABS_CFR_ASPM_L0S:
		desc->link_aspm = ASPM_L0s;
		break;
	case STARLABS_CFR_ASPM_L1:
		desc->link_aspm = ASPM_L1;
		break;
	case STARLABS_CFR_ASPM_L0S_L1:
		desc->link_aspm = ASPM_L0sL1;
		break;
	case STARLABS_CFR_ASPM_AUTO:
	default:
		break;
	}
}

static void starbook_set_dxio_l1ss(dxio_descriptor *desc, unsigned int l1ss)
{
	desc->link_aspm_L1_1 = true;
	desc->link_aspm_L1_2 = true;

	switch (l1ss) {
	case STARLABS_CFR_L1SS_DISABLED:
		desc->link_aspm_L1_1 = false;
		desc->link_aspm_L1_2 = false;
		break;
	case STARLABS_CFR_L1SS_L1_1:
		desc->link_aspm_L1_1 = true;
		desc->link_aspm_L1_2 = false;
		break;
	case STARLABS_CFR_L1SS_L1_2:
	default:
		break;
	}
}

static void starbook_update_dxio_power_management(void)
{
	dxio_descriptor *wifi = &starbook_dxio_descriptors[STARBOOK_DXIO_WIFI];
	dxio_descriptor *ssd = &starbook_dxio_descriptors[STARBOOK_DXIO_M2_STORAGE];

	if (get_uint_option("wifi", 1) == 0) {
		wifi->engine_type = UNUSED_ENGINE;
		wifi->port_present = false;
	}

	wifi->clk_req = get_uint_option("pciexp_wifi_clk_pm", 1) ? CLK_REQ6 : CLK_ENABLE;
	if (ssd->engine_type == PCIE_ENGINE)
		ssd->clk_req = get_uint_option("pciexp_ssd_clk_pm", 1) ? CLK_REQ1 : CLK_ENABLE;

	starbook_set_dxio_aspm(wifi, get_uint_option("pciexp_wifi_aspm",
						     STARLABS_CFR_ASPM_L1));
	if (ssd->engine_type == PCIE_ENGINE)
		starbook_set_dxio_aspm(ssd, get_uint_option("pciexp_ssd_aspm",
							    STARLABS_CFR_ASPM_L1));

	starbook_set_dxio_l1ss(wifi, STARLABS_CFR_L1SS_DISABLED);
	if (ssd->engine_type == PCIE_ENGINE)
		starbook_set_dxio_l1ss(ssd, get_uint_option("pciexp_ssd_l1ss",
							   STARLABS_CFR_L1SS_L1_2));
}

static void starbook_select_m2_storage_dxio(void)
{
	gpio_input(NVME_DET_GPIO);

	if (gpio_get(NVME_DET_GPIO)) {
		printk(BIOS_INFO, "DXIO: detected PCIe SSD on lanes 8-11\n");
	} else {
		printk(BIOS_INFO, "DXIO: detected SATA SSD; routing lanes 8-9 to SATA\n");
		starbook_dxio_descriptors[STARBOOK_DXIO_M2_STORAGE] =
			(dxio_descriptor)STARBOOK_M2_SATA_DXIO_DESCRIPTOR;
	}
}

static ddi_descriptor starbook_ddi_descriptors[] = {
	/* DDI0:	eDP */
	{
		.connector_type		= DDI_EDP,
		.aux_index		= DDI_AUX1,
		.hdp_index		= DDI_HDP1
	},
	/* DDI1:	HDMI */
	{
		.connector_type		= DDI_HDMI,
		.aux_index		= DDI_AUX2,
		.hdp_index		= DDI_HDP2
	},
	/* DDI2:	Not Used */
	{
		.connector_type		= DDI_UNUSED_TYPE,
		.aux_index		= DDI_AUX3,
		.hdp_index		= DDI_HDP3,
	},
	/* DDI3:	Not Used */
	{
		.connector_type		= DDI_UNUSED_TYPE,
		.aux_index		= DDI_AUX3,
		.hdp_index		= DDI_HDP3,
	},
	/* DDI4:	Display Port (via Type-C) */
	{
		.connector_type		= DDI_DP,
		.aux_index		= DDI_AUX4,
		.hdp_index		= DDI_HDP4,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const dxio_descriptor **dxio_descs, size_t *dxio_num,
		const ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	starbook_select_m2_storage_dxio();
	starbook_update_dxio_power_management();

	*dxio_descs = starbook_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(starbook_dxio_descriptors);
	*ddi_descs = starbook_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(starbook_ddi_descriptors);
}
