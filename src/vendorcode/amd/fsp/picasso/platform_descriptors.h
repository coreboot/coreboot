/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe PCIe bifurcation and display physical
 * connector types connected to the SOC.
 */

#ifndef __PI_PICASSO_PLATFORM_DESCRIPTORS_H__
#define __PI_PICASSO_PLATFORM_DESCRIPTORS_H__

/* Engine descriptor type */
typedef enum {
	UNUSED_ENGINE	= 0x00,	// Unused descriptor
	PCIE_ENGINE	= 0x01,	// PCIe port
	USB_ENGINE	= 0x02,	// USB port
	SATA_ENGINE	= 0x03,	// SATA
	DP_ENGINE	= 0x08,	// Digital Display
	ETHERNET_ENGINE	= 0x10,	// Ethernet (GBe, XGBe)
	MAX_ENGINE		// Max engine type for boundary check.
} dxio_engine_type;

/* PCIe link capability/speed */
typedef enum {
	GEN_MAX = 0,		// Maximum supported
	GEN1,
	GEN2,
	GEN3,
	GEN_INVALID		// Max Gen for boundary check
} dxio_link_speed_cap;

/* Upstream Auto Speed Change Allowed */
typedef enum {
	SPDC_DEFAULT = 0,      // Enabled for Gen2 and Gen3
	SPDC_DISBLED,
	SPDC_ENABLED,
	SPDC_INVALID
} dxio_upstream_auto_speed_change;

/* SATA ChannelType initialization */
typedef enum {
	SATA_CHANNEL_OTHER = 0,	// Default Channel Type
	SATA_CHANNEL_SHORT,	// Short Trace Channel Type
	SATA_CHANNEL_LONG	// Long Trace Channel Type
} dxio_sata_channel_type;

/* CLKREQ for PCIe type descriptors */
typedef enum {
	CLK_DISABLE = 0x00,
	CLK_REQ0,
	CLK_REQ1,
	CLK_REQ2,
	CLK_REQ3,
	CLK_REQ4,
	CLK_REQ5,
	CLK_REQ6,
	CLK_REQ7,
	CLK_REQ8,
	CLK_REQGFX = 0x0c,
} cpm_clk_req;

/* PCIe link ASPM initialization */
typedef enum {
	ASPM_DISABLED = 0,	// Disabled
	ASPM_L0s,		// PCIe L0s link state
	ASPM_L1,		// PCIe L1 link state
	ASPM_L0sL1,		// PCIe L0s & L1 link state
	ASPM_MAX		// Not valid value, used to verify input
} dxio_aspm_type;

/* DDI Aux channel */
typedef enum {
	AUX1 = 0,
	AUX2,
	AUX3,
	AUX4,
	AUX5,
	AUX6,
	AUX_MAX			// Not valid value, used to verify input
} ddi_aux_type;

/* DDI Hdp Index */
typedef enum {
	HDP1 = 0,
	HDP2,
	HDP3,
	HDP4,
	HDP5,
	HDP6,
	HDP_MAX			// Not valid value, used to verify input
} ddi_hdp_type;

/* DDI display connector type */
typedef enum {
	DP = 0,			// DP
	EDP,			// eDP
	SINGLE_LINK_DVI,	// Single Link DVI-D
	DUAL_LINK_DVI,		// Dual  Link DVI-D
	HDMI,			// HDMI
	DP_TO_VGA,		// DP-to-VGA
	DP_TO_LVDS,		// DP-to-LVDS
	NUTMEG_DP_TO_VGA,	// Hudson-2 NutMeg DP-to-VGA
	SINGLE_LINK_DVI_I,	// Single Link DVI-I
	CRT,			// CRT (VGA)
	LVDS,			// LVDS
	EDP_TO_LVDS,		// eDP-to-LVDS translator chip without AMD SW init
	EDP_TO_LVDS_SW,		// eDP-to-LVDS translator which requires AMD SW init
	AUTO_DETECT,		// VBIOS auto detect connector type
	UNUSED_PTYPE,		// UnusedType
	MAX_CONNECTOR_TYPE	// Not valid value, used to verify input
} ddi_connector_type;

/* Picasso DDI Descriptor: used for configuring display outputs */
typedef struct __packed {
	uint8_t		connector_type;
	uint8_t		aux_index;
	uint8_t		hdp_index;
	uint8_t		reserved;
} fsp_ddi_descriptor;

/*
 * Picasso DXIO Descriptor: Used for assigning lanes to PCIe/SATA/XGBE engines, configure
 * bifurcation and other settings. Beware that the lane numbers in here are the logical and not
 * the physical lane numbers!
 *
 * Picasso DXIO lane mapping:
 *
 * physical | logical | protocol
 * ---------|---------|-----------
 * GFX[7:0] | [15:8]  | PCIe
 * GPP[3:0] |  [7:4]  | PCIe
 * GPP[5:4] |  [1:0]  | PCIe, XGBE
 * GPP[7:6] |  [3:2]  | PCIe, SATA
 *
 * Picasso supports up to 7 PCIe ports. The 8 GFX PCIe lanes can either be used as an x8 port
 * or split into two x4 ports. The GPP general purpose lanes can be used as PCIe x4, x2 and x1
 * ports. The ports can only start at logical lane numbers that are integer multiples of the
 * lane width, so for example an x4 port can only start with the logical lane 0, 4, 8 or 12.
 * Different ports mustn't overlap or be assigned to the same lane(s). Within ports with the
 * same width the one with a higher start logical lane number needs to be assigned to a higher
 * PCIe root port number; ports of the same size don't have to be assigned to consecutive PCIe
 * root ports though.
 *
 * Dali only supports up to 5 PCIe ports and has less DXIO connectivity than Picasso:
 *
 * physical | logical | protocol
 * ---------|---------|-----------
 * GFX[3:0] | [11:8]  | PCIe
 * GPP[1:0] |  [5:4]  | PCIe
 * GPP[5:4] |  [1:0]  | PCIe, XGBE
 * GPP[7:6] |  [3:2]  | SATA
 *
 * Pollock has even less DXIO lanes and the mapping of GPP lane numbers to the logical lane
 * numbers differs to Picasso/Dali:
 *
 * physical | logical | protocol
 * ---------|---------|----------
 * GPP[1:0] |  [1:0]  | PCIe
 * GPP[3:2] |  [5:4]  | PCIe
 */
typedef struct __packed {
	uint8_t		engine_type;			// See dxio_engine_type
	uint8_t		start_logical_lane;		// Start lane of the pci device
	uint8_t		end_logical_lane;		// End lane of the pci device
	uint8_t		gpio_group_id;			// Currently unused by FSP
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
} fsp_dxio_descriptor;

#endif /* __PI_PICASSO_PLATFORM_DESCRIPTORS_H__ */
