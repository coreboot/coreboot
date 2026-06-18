/* SPDX-License-Identifier: GPL-2.0-only */
#include <console/console.h>
#include <ec/acpi/ec.h>
#include <soc/platform_descriptors.h>
#include <static.h>
#include <soc/aoac_defs.h>
#include <stdint.h>
#include <types.h>

#include "board_config.h"

bool devtree_xgbe_dev_enabled(uint8_t port_num);

#define ECRAM_MACID_OFFSET 0x50
#define MACID_LEN          12
#define JAGUAR_EC_CMD  0x666
#define JAGUAR_EC_DATA 0x662
#define XGBE_PORT_0 0
#define XGBE_PORT_1 1
#define ETHERNET_PORT_COUNT 2

/* Eval slot / PCIe SLOT-0 (CLK_REQ4) */
#define jaguar_mxm_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,				\
	.port_present = true,					\
	.start_logical_lane = 12,				\
	.end_logical_lane = 19,					\
	.device_number = 3,					\
	.function_number = 1,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_aspm_L1_1 = true,					\
	.link_aspm_L1_2 = true,					\
	.link_hotplug = HOTPLUG_ENHANCED,			\
	.clk_req = CLK_REQ4,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/* 2(x4)  Eval slot / PCIe SLOT-0 (CLK_REQ4) */
#define jaguar_mxm_v0_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,				\
	.port_present = true,					\
	.start_logical_lane = 12,				\
	.end_logical_lane = 15,					\
	.device_number = 3,					\
	.function_number = 1,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_hotplug = HOTPLUG_DISABLED,			\
	.clk_req = CLK_REQ4,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/* 4x - 8x  Eval slot / PCIe SLOT-0 (CLK_REQ4) */
#define jaguar_mxm_v1_dxio_descriptor {			\
	.engine_type = PCIE_ENGINE,				\
	.port_present = true,					\
	.start_logical_lane = 16,				\
	.end_logical_lane = 19,					\
	.device_number = 3,					\
	.function_number = 2,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_hotplug = HOTPLUG_DISABLED,			\
	.clk_req = CLK_REQ4,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/* 4x or 2x PCIe M.2 SSD0, muxed with GPP1/Wifi (CLKREQ0) */
#define jaguar_nvme0_dxio_descriptor {					\
	.engine_type = UNUSED_ENGINE,					\
	.port_present = false,						\
	.start_logical_lane = 0,					\
	.end_logical_lane = 3,						\
	.device_number = 2,						\
	.function_number = 1,						\
	.link_speed_capability = GEN_MAX,				\
	.turn_off_unused_lanes = true,					\
	.link_aspm = ASPM_L1,						\
	.link_aspm_L1_1 = true,						\
	.link_aspm_L1_2 = true,						\
	.link_hotplug = HOTPLUG_ENHANCED,				\
	.clk_req = CLK_REQ0,						\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},		\
}

/* 4x or 2x PCIe GPP1, muxed with NVMe0 (CLKREQ1) */
#define jaguar_gpp1_dxio_descriptor {				\
	.engine_type = UNUSED_ENGINE,				\
	.port_present = false,					\
	.start_logical_lane = 0,				\
	.end_logical_lane = 1,					\
	.device_number = 2,					\
	.function_number = 2,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_aspm_L1_1 = true,					\
	.link_aspm_L1_2 = true,					\
	.link_hotplug = HOTPLUG_ENHANCED,			\
	.clk_req = CLK_REQ1,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/* PCIe SLOT-2 muxed with xGBE (CLK_REQ3) */
#define jaguar_gpp2_dxio_descriptor {				\
	.engine_type = PCIE_ENGINE,				\
	.port_present = true,					\
	.start_logical_lane = 4,				\
	.end_logical_lane = 5,					\
	.device_number = 2,					\
	.function_number = 4,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_aspm_L1_1 = true,					\
	.link_aspm_L1_2 = true,					\
	.link_hotplug = HOTPLUG_ENHANCED,			\
	.clk_req = CLK_REQ3,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/*
 * 2x PCIe WLAN (CLK_REQ2)
 * only if CONFIG(ENABLE_NVME_WLAN_2LANES)
 */
#define jaguar_wlan_dxio_descriptor {				\
	.engine_type = UNUSED_ENGINE,				\
	.port_present = false,					\
	.start_logical_lane = 0,				\
	.end_logical_lane = 1,					\
	.device_number = 2,					\
	.function_number = 3,					\
	.link_speed_capability = GEN_MAX,			\
	.turn_off_unused_lanes = true,				\
	.link_aspm = ASPM_L1,					\
	.link_aspm_L1_1 = true,					\
	.link_aspm_L1_2 = true,					\
	.clk_req = CLK_REQ2,					\
	.port_params = {PP_PSPP_AC, 0x144, PP_PSPP_DC, 0x133},	\
}

/*
 * XGBE ETHERNET PORTS Entry Port 0
 * XGBE SGMII interface: Physical lane 4, Logical lane 0
 * NOTE: Ancillary data not yet captured here due to FSP
 * limitations
 */
#define jaguar_xgbe0_dxio_descriptor {		\
	       .port_present = true,		\
	       .engine_type = ETHERNET_ENGINE,	\
	       .start_logical_lane = 4,		\
	       .end_logical_lane = 4,		\
}

/*
 * XGBE ETHERNET PORTS Entry Port 1
 * XGBE SGMII interface: Physical lane 5, Logical lane 1
 * NOTE: Ancillary data not yet captured here due to FSP
 * limitations
 */
#define jaguar_xgbe1_dxio_descriptor {		\
	       .port_present = true,		\
	       .engine_type = ETHERNET_ENGINE,	\
	       .start_logical_lane = 5,		\
	       .end_logical_lane = 5,		\
}

static fsp_ddi_descriptor jaguar_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 - DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP */
		.connector_type = DDI_DP,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - TYPEC */
		.connector_type = CONFIG(DISPLAY_PORT_TYPEC) ? DDI_DP_W_TYPEC : DDI_HDMI,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	}
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	static fsp_dxio_descriptor jaguar_dxio_descriptors[] = {
		jaguar_nvme0_dxio_descriptor,
		jaguar_gpp1_dxio_descriptor,
		jaguar_wlan_dxio_descriptor,
#if CONFIG(DISABLE_FORCE_POWER_GPP0) && CONFIG(PCIE_SLOT0_1X8)
		jaguar_mxm_dxio_descriptor,
#endif
#if CONFIG(ENABLE_EVAL_CARD) && CONFIG(ENABLE_FORCE_POWER_GPP0) && CONFIG(PCIE_SLOT0_2X4)
		jaguar_mxm_v0_dxio_descriptor,
		jaguar_mxm_v1_dxio_descriptor,
#endif
#if CONFIG(XGBE_EN)
		jaguar_xgbe0_dxio_descriptor,
		jaguar_xgbe1_dxio_descriptor,
#else
		jaguar_gpp2_dxio_descriptor,
#endif
	};
	fsp_dxio_descriptor *nvme_desc = &jaguar_dxio_descriptors[0];
	fsp_dxio_descriptor *gpp1_desc = &jaguar_dxio_descriptors[1];
	fsp_dxio_descriptor *wlan_desc = &jaguar_dxio_descriptors[2];

	switch (mb_cfg_pcie_bifurcation()) {
	case EC_PCIE_MUX_NVMEX4:
		nvme_desc->engine_type = PCIE_ENGINE;
		nvme_desc->port_present = true;
		nvme_desc->start_logical_lane = 0;
		break;
	case EC_PCIE_MUX_SLOT1X4:
		gpp1_desc->engine_type = PCIE_ENGINE;
		gpp1_desc->port_present = true;
		gpp1_desc->end_logical_lane = 3;
		break;
	case EC_PCIE_MUX_M2_SLOT_2X2X:
		nvme_desc->engine_type = PCIE_ENGINE;
		nvme_desc->port_present = true;
		nvme_desc->start_logical_lane = 2;

		gpp1_desc->engine_type = PCIE_ENGINE;
		gpp1_desc->port_present = true;
		gpp1_desc->end_logical_lane = 1;
		break;
	case EC_PCIE_MUX_M2_WLAN_2X2X:
		nvme_desc->engine_type = PCIE_ENGINE;
		nvme_desc->port_present = true;
		nvme_desc->start_logical_lane = 2;

		wlan_desc->engine_type = PCIE_ENGINE;
		wlan_desc->port_present = true;
		break;
	default:
		break;
	}

	*dxio_descs = jaguar_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(jaguar_dxio_descriptors);
	*ddi_descs = jaguar_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(jaguar_ddi_descriptors);
}

static void xgbe_init(FSP_M_CONFIG *mcfg)
{
	uint8_t mac_buffer[12];

	ec_set_ports(JAGUAR_EC_CMD, JAGUAR_EC_DATA);
	uint16_t offset = ECRAM_MACID_OFFSET;

	uint16_t index = 0;
	for (index = 0; index < MACID_LEN ; index++) {
		uint8_t reg_value = ec_read(offset);
		printk(BIOS_SPEW, "READ MACID REG 0x%2x value 0x%02x\n", offset, reg_value);
		offset++;
		mac_buffer[index] = reg_value;
	}

	uint64_t value = 0x0;
	for (index = 0; index < 6; index++) {
		value += mac_buffer[index];
		value = value * 0x100;
	}
	uint64_t mac_addr_port0 = value / 0x100;
	printk(BIOS_SPEW, "value : mac_addr_port0 0x%02llx\n", mac_addr_port0);
	value = 0x0;
	for (index = 6; index < 12; index++) {
		value += mac_buffer[index];
		value = value * 0x100;
	}
	uint64_t mac_addr_port1 = value / 0x100;
	printk(BIOS_SPEW, "value : mac_addr_port1 0x%02llx\n", mac_addr_port1);

	for (index = 0; index < MACID_LEN; index++) {
		printk(BIOS_SPEW, " mac_buffer[0x%02x] 0x%02x\n", index, mac_buffer[index]);
	}
	/* MAC can be updated here to pass the same to FSP */
	mcfg->xgbe_port0_mac = mac_addr_port0;
	mcfg->xgbe_port1_mac = mac_addr_port1;

	if (CONFIG(XGBE_EN)) {
		mcfg->xgbe_port0_config_en = is_dev_enabled(DEV_PTR(xgbe_0));
		mcfg->xgbe_port1_config_en = is_dev_enabled(DEV_PTR(xgbe_1));
		mcfg->XgbeDisable = 0;
	} else {
		mcfg->xgbe_port0_config_en = 0;
		mcfg->xgbe_port1_config_en = 0;
		mcfg->XgbeDisable = 1;
	}

	if (mb_cfg_xgbe_leds()) {
		mcfg->xgbe_led_en = 1;
		mcfg->xgbe_led_link_status0 = mb_cfg_xgbe_p0_link_status_leds();
		mcfg->xgbe_led_link_status1 = mb_cfg_xgbe_p1_link_status_leds();
		mcfg->xgbe_led_link_speed0 = mb_cfg_xgbe_p0_link_speed_leds();
		mcfg->xgbe_led_link_speed1 = mb_cfg_xgbe_p1_link_speed_leds();
		mcfg->xgbe_led_tx_rx_blink_rate0 = mb_cfg_xgbe_p0_led_blink_rate();
		mcfg->xgbe_led_tx_rx_blink_rate1 = mb_cfg_xgbe_p1_led_blink_rate();
	}

	static struct xgbe_port_table xgbe_port[2];
	for (int port_idx = 0; port_idx < ETHERNET_PORT_COUNT; port_idx++) {
		if ((mcfg->xgbe_port0_config_en == true) || (mcfg->xgbe_port1_config_en == true)) {
			xgbe_port[port_idx].XgbePortConfig = XGBE_PORT_ENABLE;
			if (CONFIG(XGBE_BACKPLANE_CONNECTION)) {
				xgbe_port[port_idx].XgbePortConnectedType = XGBE_BACKPLANE_CONNECTION;
				if (CONFIG(XGBE_PORT_SGMII_BACKPLANE)) {
					xgbe_port[port_idx].XgbePortPlatformConfig = XGBE_PORT_SGMII_BACKPLANE;
				} else if (CONFIG(XGBE_10G_1G_BACKPLANE)) {
					xgbe_port[port_idx].XgbePortPlatformConfig = XGBE_10G_1G_BACKPLANE;
				} else if (CONFIG(XGBE_2_5G_BACKPLANE)) {
					xgbe_port[port_idx].XgbePortPlatformConfig = XGBE_2_5G_BACKPLANE;
				}
			} else if (CONFIG(XGBE_SFP_PLUS_CONNECTION)) {
				xgbe_port[port_idx].XgbePortConnectedType = XGBE_SFP_PLUS_CONNECTION;
				xgbe_port[port_idx].XgbePortPlatformConfig = XGBE_SFP_PLUS_CONNECTOR;
			}
			xgbe_port[port_idx].XgbePortMdioResetType	= 0x00;
			xgbe_port[port_idx].XgbePortResetGpioNum	= 0x00;
			xgbe_port[port_idx].XgbePortMdioResetI2cAddress = 0x00;
			xgbe_port[port_idx].XgbePortSfpI2cAddress	= 0x01;
			xgbe_port[port_idx].XgbePortSfpGpioMask		= 0x02;
			xgbe_port[port_idx].XgbePortSfpRsGpio		= 0x00;
			xgbe_port[port_idx].XgbaPortRedriverModel	= 0x00;
			xgbe_port[port_idx].XgbaPortRedriverInterface	= 0x01;
			xgbe_port[port_idx].XgbaPortRedriverAddress	= 0x00;
			xgbe_port[port_idx].XgbaPortRedriverLane	= 0x00;
			xgbe_port[port_idx].XgbePortSfpTwiAddress	= 0x1C;
			xgbe_port[port_idx].XgbaPortPadGpio		= 0x00;
			xgbe_port[port_idx].XgbaPortPadI2C		= 0x00;
			xgbe_port[port_idx].Reserve1			= 0x00;
			xgbe_port[port_idx].XgbePortSfpTwiBus		= 0x0E;
			xgbe_port[port_idx].XgbePortSfpModAbsGpio	= 0x0C;
			xgbe_port[port_idx].XgbePortSfpRxLosGpio	= 0x0D;
			xgbe_port[port_idx].XgbePortSfpTwiBus		= 0x01;
			if (CONFIG(XGBE_PORT_SPEED_10_100_1000M)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_10_100_1000M;
			} else if (CONFIG(XGBE_PORT_SPEED_10G)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_10G;
			} else if (CONFIG(XGBE_PORT_SPEED_2500M)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_2500M;
			} else if (CONFIG(XGBE_PORT_SPEED_1G)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_1G;
			} else if (CONFIG(XGBE_PORT_SPEED_100M)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_100M;
			} else if (CONFIG(XGBE_PORT_SPEED_10M)) {
				xgbe_port[port_idx].XgbePortSupportedSpeed = XGBE_PORT_SPEED_10M;
			}
			if (port_idx == 1) {
				xgbe_port[port_idx].XgbePortSfpTwiBus	  = 0xA;
				xgbe_port[port_idx].XgbePortSfpModAbsGpio = 0x8;
				xgbe_port[port_idx].XgbePortSfpRxLosGpio  = 0x9;
				xgbe_port[port_idx].XgbePortSfpTwiBus	  = 0x2;
			}
		} else {
			/* Disable the Xgbe port */
			xgbe_port[port_idx].XgbePortConfig = XGBE_PORT_DISABLE;
		}
		/* Assign pointer of xgbe_port_table, Will be consumed by FSP */
		if (port_idx == 0) {
			mcfg->xgbe_port0_table_ptr = (uint32_t)(uintptr_t)&xgbe_port[0];
		} else {
			mcfg->xgbe_port1_table_ptr = (uint32_t)(uintptr_t)&xgbe_port[1];
		}
	}
}

void mb_pre_fspm(FSP_M_CONFIG *mcfg)
{
	/* fch_rt_device_enable_map is already set by SoC code. Update as needed. */
	if (mb_cfg_uart1_disabled())
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_UART1);
	if (mb_cfg_uart2_disabled())
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_UART2);
	if (mb_cfg_uart3_disabled())
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_UART3);
	if (mb_cfg_uart4_disabled())
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_UART4);

	if (mb_cfg_i2c_enabled()) {
		/* Disable I3C */
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I3C0);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I3C1);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I3C2);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I3C3);
	} else {
		/* Disable I2C */
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I2C0);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I2C1);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I2C2);
		mcfg->fch_rt_device_enable_map &= ~BIT(FCH_AOAC_DEV_I2C3);
	}

	xgbe_init(mcfg);
}
