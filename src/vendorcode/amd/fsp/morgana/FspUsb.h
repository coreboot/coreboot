#ifndef __FSPUSB_H__
#define __FSPUSB_H__

/* TODO: Update for Morgana */

#include <FspUpd.h>

#define FSP_USB_STRUCT_MAJOR_VERSION	0xd
#define FSP_USB_STRUCT_MINOR_VERSION	0xe

#define USB2_PORT_COUNT 6
#define USB3_PORT_COUNT 3
#define USBC_COMBO_PHY_COUNT 2

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
} __packed;

struct fch_usb3_phy {
	uint8_t tx_term_ctrl;		///< tx_term_ctrl
	uint8_t rx_term_ctrl;		///< rx_term_ctrl
	uint8_t tx_vboost_lvl_en;	///< TX_VBOOST_LVL_EN
	uint8_t tx_vboost_lvl;		///< TX_VBOOST_LVL
} __packed;

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
	uint8_t			Version_Major;                     ///< USB IP version
	uint8_t			Version_Minor;                     ///< USB IP version
	uint8_t			TableLength;                       ///< TableLength
	uint8_t			Reserved0;
	struct fch_usb2_phy	Usb2PhyPort[USB2_PORT_COUNT];      ///< USB 2.0 Driving Strength
	struct fch_usb3_phy	Usb3PhyPort[USB3_PORT_COUNT];      ///< USB3 PHY Adjustment
	uint8_t			BatteryChargerEnable;              ///< bit[1:0]-Usb0 Port[1:0], bit[3:2]-Usb1 Port[1:0]
	uint8_t			PhyP3CpmP4Support;                 ///< bit[1:0]-Usb0 Port[1:0], bit[3:2]-Usb1 Port[1:0]
	uint8_t			ComboPhyStaticConfig[USBC_COMBO_PHY_COUNT]; ///< 0-Type C, 1- USB only mode, 2- DP only mode, 3- USB + DP
	uint8_t			Reserved2[4];
} __packed;

#endif
