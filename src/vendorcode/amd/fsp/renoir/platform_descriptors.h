/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe PCIe bifurcation and display physical
 * connector types connected to the SOC.
 */

#ifndef PI_CEZANNE_PLATFORM_DESCRIPTORS_H
#define PI_CEZANNE_PLATFORM_DESCRIPTORS_H

#define NUM_DXIO_PHY_PARAMS	6
#define NUM_DXIO_PORT_PARAMS	6

/* Engine descriptor type */
enum dxio_engine_type {
	UNUSED_ENGINE	= 0x00,	// Unused descriptor
	PCIE_ENGINE	= 0x01,	// PCIe port
	USB_ENGINE	= 0x02,	// USB port
	SATA_ENGINE	= 0x03,	// SATA
	DP_ENGINE	= 0x08,	// Digital Display
	ETHERNET_ENGINE	= 0x10,	// Ethernet (GBe, XGBe)
	MAX_ENGINE		// Max engine type for boundary check.
};

/* PCIe link capability/speed */
enum dxio_link_speed_cap {
	GEN_MAX = 0,		// Maximum supported
	GEN1,
	GEN2,
	GEN3,
	GEN_INVALID		// Max Gen for boundary check
};

/* Upstream Auto Speed Change Allowed */
enum dxio_upstream_auto_speed_change {
	SPDC_DEFAULT = 0,      // Enabled for Gen2 and Gen3
	SPDC_DISABLED,
	SPDC_ENABLED,
	SPDC_INVALID
};

/* SATA ChannelType initialization */
enum dxio_sata_channel_type {
	SATA_CHANNEL_OTHER = 0,	// Default Channel Type
	SATA_CHANNEL_SHORT,	// Short Trace Channel Type
	SATA_CHANNEL_LONG	// Long Trace Channel Type
};

/* CLKREQ for PCIe type descriptors */
enum cpm_clk_req {
	CLK_DISABLE = 0x00,
	CLK_REQ0,
	CLK_REQ1,
	CLK_REQ2,
	CLK_REQ3,
	CLK_REQ4_GFX,
	CLK_REQ5,
	CLK_REQ6,
	CLK_ENABLE = 0xff,
};

/* PCIe link ASPM initialization */
enum dxio_aspm_type {
	ASPM_DISABLED = 0,	// Disabled
	ASPM_L0s,		// PCIe L0s link state
	ASPM_L1,		// PCIe L1 link state
	ASPM_L0sL1,		// PCIe L0s & L1 link state
	ASPM_MAX		// Not valid value, used to verify input
};

/* PCIe link hotplug */
enum dxio_link_hotplug_type {
	HOTPLUG_DISABLED = 0,
	HOTPLUG_BASIC,
	HOTPLUG_SERVER,
	HOTPLUG_ENHANCED,
	HOTPLUG_INBOARD,
	HOTPLUG_SERVER_SSD,
};

enum dxio_port_param_type {
	PP_DEVICE = 1,
	PP_FUNCTION,
	PP_PORT_PRESENT,
	PP_LINK_SPEED_CAP,
	PP_LINK_ASPM,
	PP_HOTPLUG_TYPE,
	PP_CLKREQ,
	PP_ASPM_L1_1,
	PP_ASPM_L1_2,
	PP_COMPLIANCE,
	PP_SAFE_MODE,
	PP_CHIPSET_LINK,
	PP_CLOCK_PM,
	PP_CHANNELTYPE,
	PP_TURN_OFF_UNUSED_LANES,
	PP_APIC_GROUPMAP,
	PP_APIC_SWIZZLE,
	PP_APIC_BRIDGEINT,
	PP_MASTER_PLL,
	PP_SLOT_NUM,
	PP_PHY_PARAM,
	PP_ESM,
	PP_CCIX,
	PP_GEN3_DS_TX_PRESET,
	PP_GEN3_DS_RX_PRESET_HINT,
	PP_GEN3_US_TX_PRESET,
	PP_GEN3_US_RX_PRESET_HINT,
	PP_GEN4_DS_TX_PRESET,
	PP_GEN4_US_TX_PRESET,
	PP_GEN3_FIXED_PRESET,
	PP_GEN4_FIXED_PRESET,
	PP_PSPP_DC,
	PP_PSPP_AC,
	PP_GEN2_DEEMPHASIS,
	PP_INVERT_POLARITY,
	PP_TARGET_LINK_SPEED,
	PP_GEN4_DLF_CAP_DISABLE,
	PP_GEN4_DLF_EXCHG_DISABLE
};

/* DDI Aux channel */
enum ddi_aux_type {
	DDI_AUX1 = 0,
	DDI_AUX2,
	DDI_AUX3,
	DDI_AUX4,
	DDI_AUX5,
	DDI_AUX6,
	DDI_AUX_MAX		// Not valid value, used to verify input
};

/* DDI Hdp Index */
enum ddi_hdp_type {
	DDI_HDP1 = 0,
	DDI_HDP2,
	DDI_HDP3,
	DDI_HDP4,
	DDI_HDP5,
	DDI_HDP6,
	DDI_HDP_MAX		// Not valid value, used to verify input
};

/* DDI display connector type */
enum ddi_connector_type {
	DDI_DP = 0,		// DP
	DDI_EDP,		// eDP
	DDI_SINGLE_LINK_DVI,	// Single Link DVI-D
	DDI_DUAL_LINK_DVI,	// Dual Link DVI-D
	DDI_HDMI,		// HDMI
	DDI_DP_TO_VGA,		// DP-to-VGA
	DDI_DP_TO_LVDS,		// DP-to-LVDS
	DDI_NUTMEG_DP_TO_VGA,	// Hudson-2 NutMeg DP-to-VGA
	DDI_SINGLE_LINK_DVI_I,	// Single Link DVI-I
	DDI_CRT,		// CRT (VGA)
	DDI_LVDS,		// LVDS
	DDI_EDP_TO_LVDS,	// eDP-to-LVDS translator chip without AMD SW init
	DDI_EDP_TO_LVDS_SW,	// eDP-to-LVDS translator which requires AMD SW init
	DDI_AUTO_DETECT,	// VBIOS auto detect connector type
	DDI_UNUSED_TYPE,	// UnusedType
	DDI_MAX_CONNECTOR_TYPE	// Not valid value, used to verify input
};

/* DDI Descriptor: used for configuring display outputs */
typedef struct __packed {
	uint8_t		connector_type; // see ddi_connector_type
	uint8_t		aux_index; // see ddi_aux_type
	uint8_t		hdp_index; // see ddi_hdp_type
	uint8_t		reserved;
} fsp_ddi_descriptor;

/*
 * Cezanne DXIO Descriptor: Used for assigning lanes to PCIe/SATA/XGBE engines,
 * configure bifurcation and other settings. Beware that the lane numbers in
 * here are the logical and not the physical lane numbers!
 *
 * Cezanne DXIO logical lane to physical PCIe lane mapping:
 *
 * logical |   FT6      |   AM4
 * --------|------------|----------------------
 * [00:03] | GPP[00:03] | GPP[00:03]
 * [04:07] | GPP[04:07] | GPP[04:07]/HUB[00:03]
 * [08:11] | GPP[08:11] | GFX[15:12]
 * [12:15] |        n/a | GFX[11:08]
 * [16:23] | GFX[00:07] | GFX[07:0]
 *
 * Different ports mustn't overlap or be assigned to the same lane(s). Within
 * ports with the same width the one with a higher start logical lane number
 * needs to be assigned to a higher PCIe root port number; ports of the same
 * size don't have to be assigned to consecutive PCIe root ports though.
 *
 * Lanes 2 and 3 can be mapped to the SATA controller on all packages; the FT6
 * platform additionally supports mapping lanes 8 and 9 to a SATA controller.
 * On embedded SKUs lanes 0 and 1 can be mapped to the Gigabit Ethernet
 * controllers.
 */
typedef struct __packed {
	uint8_t		engine_type;			// See dxio_engine_type
	uint8_t		start_logical_lane;		// Start lane of the pci device
	uint8_t		end_logical_lane;		// End lane of the pci device
	uint8_t		gpio_group_id;			// GPIO number used as reset
	uint32_t	port_present		:1;	// Should be TRUE if train link
	uint32_t	reserved_3		:7;
	uint32_t	device_number		:5;	// Desired root port device number
	uint32_t	function_number		:3;	// Desired root port function number
	uint32_t	link_speed_capability	:2;	// See dxio_link_speed_cap
	uint32_t	auto_spd_change		:2;	// See dxio_upstream_auto_speed_change
	uint32_t	eq_preset		:4;	// Gen3 equalization preset
	uint32_t	link_aspm		:2;	// See dxio_aspm_type
	uint32_t	link_aspm_L1_1		:1;	// En/Dis root port capabilities for L1.1
	uint32_t	link_aspm_L1_2		:1;	// En/Dis root port capabilities for L1.2
	uint32_t	clk_req			:4;	// See cpm_clk_req
	uint8_t		link_hotplug;			// See dxio_link_hotplug_type
	uint8_t		slot_power_limit;		// Currently unused by FSP
	uint32_t	slot_power_limit_scale	:2;	// Currently unused by FSP
	uint32_t	reserved_4		:6;
	uint32_t	link_compliance_mode	:1;	// Currently unused by FSP
	uint32_t	link_safe_mode		:1;	// Currently unused by FSP
	uint32_t	sb_link			:1;	// Currently unused by FSP
	uint32_t	clk_pm_support		:1;	// Currently unused by FSP
	uint32_t	channel_type		:3;	// See dxio_sata_channel_type
	uint32_t	turn_off_unused_lanes	:1;	// Power down lanes if device not present
	uint8_t		reserved[4];
	uint8_t		phy_params[NUM_DXIO_PHY_PARAMS*2];
	uint16_t	port_params[NUM_DXIO_PORT_PARAMS*2];	// key-value parameters. see dxio_port_param_type
} fsp_dxio_descriptor;

typedef enum {
	XGBE_PORT_DISABLE,
	XGBE_PORT_ENABLE,
} xgbe_port_enable;

typedef enum {
	XGBE_PHY_MODE_RJ45,
	XGBE_PHY_MODE_SFP_PLUS,
	XGBE_PHY_MODE_BACKPLANE
} xgbe_port_phy_modes;

typedef enum {
	XGBE_RESERVED,
	XGBE_10G_1G_BACKPLANE,
	XGBE_2_5G_BACKPLANE,
	XGBE_SOLDERED_DOWN_1000BASE_T,
	XGBE_SOLDERED_DOWN_1000BASE_X,
	XGBE_SOLDERED_DOWN_NBASE_T,
	XGBE_SOLDERED_DOWN_10GBASE_T,
	XGBE_SOLDERED_DOWN_10GBASE_R,
	XGBE_SFP_PLUS_CONNECTOR,
	BACKPLANE_AUTONEG_OFF
} xgbe_port_platform_config;

typedef enum {
	XGBE_PORT_SPEED_10M = 0x1,
	XGBE_PORT_SPEED_100M = 0x2,
	XGBE_PORT_SPEED_1G = 0x4,
	XGBE_PORT_SPEED_10_100_1000M = 0x7,
} xgbe_port_speed_config;

typedef enum {
	XGBE_PORT_NOT_USED = 0x0,
	XGBE_SFP_PLUS_CONNECTION = 0x1,
	XGBE_CONNECTION_MDIO_PHY = 0x2,
	XGBE_BACKPLANE_CONNECTION = 0x4,
} xgbe_port_connection_type;

struct __packed xgbe_port_table {
	uint8_t xgbe_port_config;               ///< XGbE controller Port Config Enable/disable
	uint8_t xgbe_port_platform_config;      ///< Platform Config
	                                        ///  @li   <b>0000</b> - Reserved
	                                        ///  @li   <b>0001</b> - 10G/1G Backplane
	                                        ///  @li   <b>0010</b> - 2.5G Backplane
	                                        ///  @li   <b>0011</b> - 1000Base-T
	                                        ///  @li   <b>0100</b> - 1000Base-X
	                                        ///  @li   <b>0101</b> - NBase-T
	                                        ///  @li   <b>0110</b> - 10GBase-T
	                                        ///  @li   <b>0111</b> - 10GBase-X
	                                        ///  @li   <b>1000</b> - SFP+
	uint8_t xgbe_port_supported_speed;      ///< Indicated Ethernet speeds supported on platform
	                                        ///  @li   <b>x111</b> - 10/100/1000M
	                                        ///  @li   <b>x1xx</b> - 1G
	                                        ///  @li   <b>xx1x</b> - 100M
	                                        ///  @li   <b>xxx1</b> - 10M
	uint8_t xgbe_port_connected_type;       ///< PHY connected type
	                                        ///  @li   <b>000</b> - Port not used
	                                        ///  @li   <b>001</b> - SFP+
	                                        ///  @li   <b>010</b> - MDIO
	                                        ///  @li   <b>100</b> - Backplane connection
	uint8_t xgbe_port_mdio_id;              ///< MDIO ID of the PHY associated with this port
	uint8_t xgbe_port_mdio_reset_type;      ///< MDIO PHY reset type
	                                        ///  @li   <b>00</b> - None
	                                        ///  @li   <b>01</b> - I2C GPIO
	                                        ///  @li   <b>10</b> - Integrated GPIO
	                                        ///  @li   <b>11</b> - Reserved
	uint8_t xgbe_port_reset_gpio_num;       ///< GPIO used to control the reset
	uint8_t xgbe_port_mdio_reset_i2c_address; ///< I2C address of PCA9535 MDIO reset GPIO
	uint8_t xgbe_port_sfp_i2c_address;      ///< I2C address of PCA9535 for SFP
	uint8_t xgbe_port_sfp_tx_fault_gpio;    ///< GPIO number for SFP+ TX_FAULT
	uint8_t xgbe_port_sfp_rs_gpio;          ///< GPIO number for SFP+ RS
	uint8_t xgbe_port_sfp_mod_abs_gpio;     ///< GPIO number for SFP+ Mod_ABS
	uint8_t xgbe_port_sfp_rx_los_gpio;      ///< GPIO number for SFP+ Rx_LOS
	uint8_t xgbe_port_sfp_gpio_mask;        ///< GPIO Mask for SFP+
	                                        ///  @li   <b>1xxx</b> - Rx_LOS not supported
	                                        ///  @li   <b>x1xx</b> - Mod_ABS not supported
	                                        ///  @li   <b>xx1x</b> - RS not supported
	                                        ///  @li   <b>xxx1</b> - TX_FAULT not supported
	uint8_t xgbe_port_sfp_twi_address;      ///< Address of PCA9545 I2C multiplexor
	uint8_t xgbe_port_sfp_twi_bus;          ///< Downstream channel of PCA9545
	uint8_t xgba_port_redriver_present;     ///< Redriver Present or not
	uint8_t reserve0[3];                    ///< Reserved
	uint8_t xgba_port_redriver_model;       ///< Redriver Model
	                                        ///  @li   <b>00</b> - InPhi 4223
	                                        ///  @li   <b>01</b> - InPhi 4227
	uint8_t xgba_port_redriver_interface;   ///< Redriver Interface
	                                        ///  @li   <b>00</b> - MDIO
	                                        ///  @li   <b>01</b> - I2C
	uint8_t xgba_port_redriver_address;     ///< Redriver Address
	uint8_t xgba_port_redriver_lane;        ///< Redriver Lane
	uint8_t xgba_port_pad_gpio;             ///< Portx_GPIO Pad selection
	                                        ///  @li   <b>001</b> - MDIO0 pin
	                                        ///  @li   <b>010</b> - MDIO1 pin
	                                        ///  @li   <b>100</b> - SFP pin
	uint8_t xgba_port_pad_mdio;             ///< Portx_Mdio Pad selection
	                                        ///  @li   <b>001</b> - MDIO0 pin
	                                        ///  @li   <b>010</b> - MDIO1 pin
	                                        ///  @li   <b>100</b> - SFP pin
	uint8_t xgba_port_pad_i2c;              ///< Portx_I2C Pad selection
	                                        ///  @li   <b>001</b> - MDIO0 pin
	                                        ///  @li   <b>010</b> - MDIO1 pin
	                                        ///  @li   <b>100</b> - SFP pin
	uint8_t reserve1;                       ///< Reserved
};

#endif /* PI_CEZANNE_PLATFORM_DESCRIPTORS_H */
