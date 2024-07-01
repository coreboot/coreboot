/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

#define PORT_TUNE1_MASK				0xf0

/* QUSB2PHY_PWR_CTRL1 register related bits */
#define POWER_DOWN				BIT(0)

/* DEBUG_CTRL2 register value to program VSTATUS MUX for PHY status */
#define DEBUG_CTRL2_MUX_PLL_LOCK_STATUS		0x4

/* STAT5 register bits */
#define VSTATUS_PLL_LOCK_STATUS_MASK		BIT(0)

/* QUSB PHY register values */
#define QUSB2PHY_PLL_ANALOG_CONTROLS_TWO	0x03
#define QUSB2PHY_PLL_CLOCK_INVERTERS		0x7c
#define QUSB2PHY_PLL_CMODE			0x80
#define QUSB2PHY_PLL_LOCK_DELAY			0x0a
#define QUSB2PHY_PLL_DIGITAL_TIMERS_TWO		0x19
#define QUSB2PHY_PLL_BIAS_CONTROL_1		0x40
#define QUSB2PHY_PLL_BIAS_CONTROL_2		0x22
#define QUSB2PHY_PWR_CTRL2			0x21
#define QUSB2PHY_IMP_CTRL1			0x08
#define QUSB2PHY_IMP_CTRL2			0x58
#define QUSB2PHY_PORT_TUNE1			0xc5
#define QUSB2PHY_PORT_TUNE2			0x29
#define QUSB2PHY_PORT_TUNE3			0xca
#define QUSB2PHY_PORT_TUNE4			0x04
#define QUSB2PHY_PORT_TUNE5			0x03
#define QUSB2PHY_CHG_CTRL2			0x30


#define QFPROM_BASE			0x00780000
#define QUSB_PRIM_PHY_BASE		0x088e3000
#define QUSB_PRIM_PHY_DIG_BASE		0x088e3200

#define HS_USB_PRIM_PHY_BASE  QUSB_PRIM_PHY_BASE

struct usb_board_data {
	/* Register values going to override from the boardfile */
	u32 pll_bias_control_2;
	u32 imp_ctrl1;
	u32 port_tune1;
};

struct usb_qusb_phy_dig {
	u8 rsvd1[16];
	u32 pwr_ctrl1;
	u32 pwr_ctrl2;
	u8 rsvd2[8];
	u32 imp_ctrl1;
	u32 imp_ctrl2;
	u8 rsvd3[20];
	u32 chg_ctrl2;
	u32 tune1;
	u32 tune2;
	u32 tune3;
	u32 tune4;
	u32 tune5;
	u8 rsvd4[44];
	u32 debug_ctrl2;
	u8 rsvd5[28];
	u32 debug_stat5;
};
check_member(usb_qusb_phy_dig, tune5, 0x50);
check_member(usb_qusb_phy_dig, debug_ctrl2, 0x80);
check_member(usb_qusb_phy_dig, debug_stat5, 0xA0);

struct usb_qusb_phy_pll {
	u8 rsvd0[4];
	u32 analog_controls_two;
	u8 rsvd1[36];
	u32 cmode;
	u8 rsvd2[132];
	u32 dig_tim;
	u8 rsvd3[204];
	u32 lock_delay;
	u8 rsvd4[4];
	u32 clock_inverters;
	u8 rsvd5[4];
	u32 bias_ctrl_1;
	u32 bias_ctrl_2;
};
check_member(usb_qusb_phy_pll, cmode, 0x2C);
check_member(usb_qusb_phy_pll, bias_ctrl_2, 0x198);
check_member(usb_qusb_phy_pll, dig_tim, 0xB4);

struct hs_usb_phy_reg {
	struct usb_qusb_phy_pll *phy_pll;
	struct usb_qusb_phy_dig *phy_dig;
	struct usb_board_data *board_data;
	u32 efuse_offset;
};
