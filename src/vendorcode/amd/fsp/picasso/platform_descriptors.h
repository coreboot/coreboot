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
} pcie_aux_type;

/* DDI Hdp Index */
typedef enum {
	HDP1 = 0,
	HDP2,
	HDP3,
	HDP4,
	HDP5,
	HDP6,
	HDP_MAX			// Not valid value, used to verify input
} pcie_hdp_type;

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
} pcie_connector_type;

/* DDI Descriptor: used for configuring display outputs */
typedef struct __packed {
	uint8_t		connector_type;
	uint8_t		aux_index;
	uint8_t		hdp_index;
	uint8_t		reserved;
} fsp_ddi_descriptor;

/* PCIe Descriptor: used for assigning lanes, bifurcation and other settings */
/* Since the code will always be compiled as little endian, using a bitfield struct should be
   safe here. */
typedef struct __packed {
	uint8_t		engine_type;
	uint8_t		start_lane;			// Start lane of the pci device
	uint8_t		end_lane;			// End lane of the pci device
	uint8_t		gpio_group_id;			// FCH reset number. 0 is global reset
	unsigned int	port_present		:1;	// Should be TRUE if train link
	unsigned int	reserved_3		:7;
	unsigned int	device_number		:5;	// Desired root port device number
	unsigned int	function_number		:3;	// Desired root port function number
	unsigned int	link_speed_capability	:2;
	unsigned int	auto_spd_change		:2;
	unsigned int	eq_preset		:4;
	unsigned int	link_aspm		:2;
	unsigned int	link_aspm_L1_1		:1;
	unsigned int	link_aspm_L1_2		:1;
	unsigned int	clk_req			:4;
	uint8_t		link_hotplug;
	uint8_t		slot_power_limit;
	unsigned int	slot_power_limit_scale	:2;
	unsigned int	reserved_4		:6;
	unsigned int	link_compliance_mode	:1;
	unsigned int	link_safe_mode		:1;
	unsigned int	sb_link			:1;
	unsigned int	clk_pm_support		:1;
	unsigned int	channel_type		:3;
	unsigned int	turn_off_unused_lanes	:1;
	uint8_t		reserved[4];
} fsp_pcie_descriptor;

#endif /* __PI_PICASSO_PLATFORM_DESCRIPTORS_H__ */
