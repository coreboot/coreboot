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

#include <static.h>
#include "ec.h"

#define ECRAM_BOARDID_OFFSET 0x93

#define CRATER_EC_CMD   0x666
#define CRATER_EC_DATA  0x662

#define mxm_dxio_descriptor { \
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

#define wwan_dxio_descriptor { \
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
	uint8_t board_rev = crater_ec_get_board_revision();

	if ((get_cpu_count() == 4 && get_threads_per_core() == 2) || get_cpu_count() == 2)
		crater_ddi_descriptors[1].connector_type = DDI_UNUSED_TYPE;

	if (CONFIG(ENABLE_EDP)) {
		crater_ddi_descriptors[1].connector_type = DDI_EDP;
	} else {
		if (board_rev == CRATER_REVB)
			crater_ddi_descriptors[1].connector_type = DDI_DP;
		else
			crater_ddi_descriptors[1].connector_type = DDI_HDMI;
	}

	static const fsp_dxio_descriptor crater_dxio_descriptors[] = {
		mxm_dxio_descriptor,
		ssd_dxio_descriptor,
		tb_dxio_descriptor,
#if CONFIG(PCIE_DT_SLOT)
		dt_dxio_descriptor, // GPP 0~3
#elif CONFIG(XGBE_WWAN_WLAN)
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

#if CONFIG(XGBE_PATH_SELECT_BACKPLANE)

#define xgbe_port0_table_descriptor { \
	.xgbe_port_config          = XGBE_PORT_DISABLE, \
	.xgbe_port_connected_type  = XGBE_BACKPLANE_CONNECTION, \
	.xgbe_port_platform_config = BACKPLANE_AUTONEG_OFF, \
	.xgbe_port_supported_speed = XGBE_PORT_SPEED_1G, \
	.xgba_port_pad_mdio           = 0x0, \
	.xgba_port_pad_i2c            = 0x1, \
	.xgbe_port_sfp_tx_fault_gpio  = 0xE, \
	.xgbe_port_sfp_rx_los_gpio    = 0xD, \
	.xgbe_port_sfp_mod_abs_gpio   = 0xC, \
	.xgbe_port_sfp_twi_bus        = 0x1, \
	.xgbe_port_mdio_reset_type    = 0x0, \
	.xgbe_port_reset_gpio_num     = 0x0, \
	.xgbe_port_mdio_reset_i2c_address = 0x0, \
	.xgbe_port_sfp_i2c_address    = 0x1, \
	.xgbe_port_sfp_gpio_mask      = 0x2, \
	.xgbe_port_sfp_rs_gpio        = 0x0, \
	.xgba_port_redriver_model     = 0x0, \
	.xgba_port_redriver_interface = 0x1, \
	.xgba_port_redriver_address   = 0x0, \
	.xgba_port_redriver_lane      = 0x0, \
	.xgbe_port_sfp_twi_address    = 0x1C, \
	.xgba_port_pad_gpio           = 0x0, \
	.reserve1                     = 0x0, \
	.xgbe_port_mdio_id            = 0x0, \
}
#define xgbe_port1_table_descriptor { \
	.xgbe_port_config          = XGBE_PORT_DISABLE, \
	.xgbe_port_connected_type  = XGBE_BACKPLANE_CONNECTION, \
	.xgbe_port_platform_config = BACKPLANE_AUTONEG_OFF, \
	.xgbe_port_supported_speed = XGBE_PORT_SPEED_1G, \
	.xgba_port_pad_mdio           = 0x0, \
	.xgbe_port_mdio_id            = 0x0, \
	.xgba_port_pad_i2c            = 0x1, \
	.xgbe_port_sfp_tx_fault_gpio  = 0xA, \
	.xgbe_port_sfp_rx_los_gpio    = 0x9, \
	.xgbe_port_sfp_mod_abs_gpio   = 0x8, \
	.xgbe_port_sfp_twi_bus        = 0x2, \
	.xgbe_port_mdio_reset_type    = 0x0, \
	.xgbe_port_reset_gpio_num     = 0x0, \
	.xgbe_port_mdio_reset_i2c_address = 0x0, \
	.xgbe_port_sfp_i2c_address    = 0x1, \
	.xgbe_port_sfp_gpio_mask      = 0x2, \
	.xgbe_port_sfp_rs_gpio        = 0x0, \
	.xgba_port_redriver_model     = 0x0, \
	.xgba_port_redriver_interface = 0x1, \
	.xgba_port_redriver_address   = 0x0, \
	.xgba_port_redriver_lane      = 0x0, \
	.xgbe_port_sfp_twi_address    = 0x1C, \
	.xgba_port_pad_gpio           = 0x0, \
	.reserve1                     = 0x0, \
}

#else

#define xgbe_port0_table_descriptor { \
	.xgbe_port_config          = XGBE_PORT_DISABLE, \
	.xgbe_port_connected_type  = XGBE_CONNECTION_MDIO_PHY, \
	.xgbe_port_platform_config = XGBE_SOLDERED_DOWN_1000BASE_T, \
	.xgbe_port_supported_speed = CONFIG(XGBE_1G_SPEED)    ? XGBE_PORT_SPEED_1G : \
				     CONFIG(XGBE_100MB_SPEED) ? XGBE_PORT_SPEED_100M : \
				     CONFIG(XGBE_10MB_SPEED)  ? XGBE_PORT_SPEED_10M : XGBE_PORT_SPEED_10_100_1000M, \
	.xgba_port_pad_mdio           = 0x1, \
	.xgba_port_pad_i2c            = 0x0, \
	.xgbe_port_sfp_tx_fault_gpio  = 0xB, \
	.xgbe_port_sfp_rx_los_gpio    = 0xD, \
	.xgbe_port_sfp_mod_abs_gpio   = 0xC, \
	.xgbe_port_sfp_twi_bus        = 0x0, \
	.xgbe_port_mdio_reset_type    = 0x0, \
	.xgbe_port_reset_gpio_num     = 0x0, \
	.xgbe_port_mdio_reset_i2c_address = 0x0, \
	.xgbe_port_sfp_i2c_address     = 0x1, \
	.xgbe_port_sfp_gpio_mask       = 0x2, \
	.xgbe_port_sfp_rs_gpio         = 0x0, \
	.xgba_port_redriver_model      = 0x0, \
	.xgba_port_redriver_interface  = 0x1, \
	.xgba_port_redriver_address    = 0x0, \
	.xgba_port_redriver_lane       = 0x0, \
	.xgbe_port_sfp_twi_address     = 0x1C, \
	.xgba_port_pad_gpio            = 0x0, \
	.reserve1                      = 0x0, \
	.xgbe_port_mdio_id             = 0x0, \
}
#define xgbe_port1_table_descriptor { \
	.xgbe_port_config = XGBE_PORT_DISABLE, \
	.xgbe_port_connected_type = XGBE_CONNECTION_MDIO_PHY, \
	.xgbe_port_platform_config = XGBE_SOLDERED_DOWN_1000BASE_T, \
	.xgbe_port_supported_speed = CONFIG(XGBE_1G_SPEED)    ? XGBE_PORT_SPEED_1G : \
				     CONFIG(XGBE_100MB_SPEED) ? XGBE_PORT_SPEED_100M : \
				     CONFIG(XGBE_10MB_SPEED)  ? XGBE_PORT_SPEED_10M : XGBE_PORT_SPEED_10_100_1000M, \
	.xgba_port_pad_mdio          = CONFIG(GBE_PATH_SELECT_AIC3) ? 0x2 : CONFIG(GBE_PATH_SELECT_AIC3) ? 0x01 : 0x0, \
	.xgbe_port_mdio_id           = CONFIG(GBE_PATH_SELECT_AIC3) ? 0x0 : CONFIG(GBE_PATH_SELECT_AIC3) ? 0x01 : 0x0, \
	.xgba_port_pad_i2c            = 0x0, \
	.xgbe_port_sfp_tx_fault_gpio  = 0xA, \
	.xgbe_port_sfp_rx_los_gpio    = 0x9, \
	.xgbe_port_sfp_mod_abs_gpio   = 0x8, \
	.xgbe_port_sfp_twi_bus        = 0x1, \
	.xgbe_port_mdio_reset_type    = 0x0, \
	.xgbe_port_reset_gpio_num     = 0x0, \
	.xgbe_port_mdio_reset_i2c_address = 0x0, \
	.xgbe_port_sfp_i2c_address    = 0x1, \
	.xgbe_port_sfp_gpio_mask      = 0x2, \
	.xgbe_port_sfp_rs_gpio        = 0x0, \
	.xgba_port_redriver_model     = 0x0, \
	.xgba_port_redriver_interface = 0x1, \
	.xgba_port_redriver_address   = 0x0, \
	.xgba_port_redriver_lane      = 0x0, \
	.xgbe_port_sfp_twi_address    = 0x1C, \
	.xgba_port_pad_gpio           = 0x0, \
	.reserve1                     = 0x0, \
}

#endif

static void xgbe_init(FSP_M_CONFIG *mcfg)
{
	static struct xgbe_port_table xgbe_port[2] = {
		xgbe_port0_table_descriptor,
		xgbe_port1_table_descriptor
	};

	/* MAC can be updated here to pass the same to FSP */
	crater_ec_get_mac_addresses(&mcfg->xgbe_port0_mac, &mcfg->xgbe_port1_mac);
	printk(BIOS_SPEW, "MAC Address XGBE port0: 0x%02llx\n", mcfg->xgbe_port0_mac);
	printk(BIOS_SPEW, "MAC Address XGBE port1: 0x%02llx\n", mcfg->xgbe_port1_mac);

	mcfg->xgbe_port0_config_en = is_dev_enabled(DEV_PTR(xgbe_0));
	mcfg->xgbe_port1_config_en = is_dev_enabled(DEV_PTR(xgbe_1));

	if (mcfg->xgbe_port0_config_en) {
		xgbe_port[0].xgbe_port_config = XGBE_PORT_ENABLE;
		mcfg->xgbe_port0_table = (uint32_t)(uintptr_t)&xgbe_port[0];
	}

	if (mcfg->xgbe_port1_config_en) {
		xgbe_port[1].xgbe_port_config = XGBE_PORT_ENABLE;
		mcfg->xgbe_port1_table = (uint32_t)(uintptr_t)&xgbe_port[1];
	}
}

void mb_pre_fspm(FSP_M_CONFIG *mcfg)
{
	xgbe_init(mcfg);
}
