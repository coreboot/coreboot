/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe PCIe bifurcation and display physical
 * connector types connected to the SOC.
 */

/* TODO: Update for Morgana */

#ifndef PI_PLATFORM_DESCRIPTORS_H
#define PI_PLATFORM_DESCRIPTORS_H

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

/* Cezanne DDI Descriptor: used for configuring display outputs */
typedef struct __packed {
	uint8_t		connector_type;	// see ddi_connector_type
	uint8_t		aux_index;	// see ddi_aux_type
	uint8_t		hdp_index;	// see ddi_hdp_type
	uint8_t		reserved;
} fsp_ddi_descriptor;

/*
 * Mendocino DXIO Descriptor: Used for assigning lanes to PCIe engines, configure
 * bifurcation and other settings. Beware that the lane numbers in here are the
 * logical and not the physical lane numbers!
 *
 * Mendocino DXIO logical lane to physical PCIe lane mapping:
 *
 * logical | physical
 * --------|------------
 * [00:03] | GPP[03:00]
 *
 * Different ports mustn't overlap or be assigned to the same lane(s). Within
 * ports with the same width the one with a higher start logical lane number
 * needs to be assigned to a higher PCIe root port number; ports of the same
 * size don't have to be assigned to consecutive PCIe root ports though.
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
	uint8_t		link_hotplug;			// Currently unused by FSP
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

#endif /* PI_PLATFORM_DESCRIPTORS_H */
