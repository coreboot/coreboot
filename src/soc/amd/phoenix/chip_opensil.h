/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#define DDI_DESCRIPTOR_COUNT	5

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
	DDI_DP_W_TYPEC,		// DP with USB type C
	DDI_DP_WO_TYPEC,	// DP without USB type C
	DDI_EDP_TO_LVDS,	// eDP-to-LVDS translator chip without AMD SW init
	DDI_EDP_TO_LVDS_SW,	// eDP-to-LVDS translator which requires AMD SW init
	DDI_AUTO_DETECT,	// VBIOS auto detect connector type
	DDI_UNUSED_TYPE,	// UnusedType
	DDI_MAX_CONNECTOR_TYPE	// Not valid value, used to verify input
};

/* DDI Descriptor: used for configuring display outputs */
struct ddi_descriptor {
	uint8_t		connector_type;	// see ddi_connector_type
	uint8_t		aux_index;
	uint8_t		hdp_index;
};

#define USB2_PORT_COUNT 8
#define USB3_PORT_COUNT 3
#define USBC_COMBO_PHY_COUNT 3

struct fch_usb2_phy {
	uint8_t compdistune;		///< COMPDISTUNE
	uint8_t pllbtune;		///< PLLBTUNE
	uint8_t pllitune;		///< PLLITUNE
	uint8_t pllptune;		///< PLLPTUNE
	uint8_t sqrxtune;		///< SQRXTUNE
	uint8_t txfslstune;		///< TXFSLSTUNE
	uint8_t txpreempamptune;	///< TXPREEMPAMPTUNE
	uint8_t txpreemppulsetune;	///< TXPREEMPPULSETUNE
	uint8_t txrisetune;		///< TXRISETUNE
	uint8_t txvreftune;		///< TXVREFTUNE
	uint8_t txhsxvtune;		///< TXHSXVTUNE
	uint8_t txrestune;		///< TXRESTUNE
};

struct fch_usb3_phy {
	uint8_t tx_term_ctrl;		///< tx_term_ctrl
	uint8_t rx_term_ctrl;		///< rx_term_ctrl
	uint8_t tx_vboost_lvl_en;	///< TX_VBOOST_LVL_EN
	uint8_t tx_vboost_lvl;		///< TX_VBOOST_LVL
};

#define USB0_PORT0	0
#define USB0_PORT1	1
#define USB0_PORT2	1
#define USB0_PORT3	3
#define USB1_PORT0	(0<<2)
#define USB1_PORT1	(1<<2)
#define USB1_PORT2	(1<<2)
#define USB1_PORT3	(3<<2)

#define USB_COMBO_PHY_MODE_USB_C	0
#define USB_COMBO_PHY_MODE_USB_ONLY	1
#define USB_COMBO_PHY_MODE_USB_DPM	2
#define USB_COMBO_PHY_MODE_USB_DPP	3

struct usb_phy_config {
	struct fch_usb2_phy	Usb2PhyPort[USB2_PORT_COUNT];      ///< USB 2.0 Driving Strength
	struct fch_usb3_phy	Usb3PhyPort[USB3_PORT_COUNT];      ///< USB3 PHY Adjustment
	uint8_t			BatteryChargerEnable;              ///< bit[1:0]-Usb0 Port[1:0], bit[3:2]-Usb1 Port[1:0]
	uint8_t			PhyP3CpmP4Support;                 ///< bit[1:0]-Usb0 Port[1:0], bit[3:2]-Usb1 Port[1:0]
	uint8_t			ComboPhyStaticConfig[USBC_COMBO_PHY_COUNT]; ///< 0-Type C, 1- USB only mode, 2- DP only mode, 3- USB + DP
};
