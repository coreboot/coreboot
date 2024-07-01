/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>

struct usb_board_data {
	/*Register values going to override from the boardfile*/
	u8 parameter_override_x0;
	u8 parameter_override_x1;
	u8 parameter_override_x2;
	u8 parameter_override_x3;
};

struct hs_usb_phy_reg {
	u8 rsvd1[60];
	u32 utmi_ctrl0;
	u32 utmi_ctrl1;
	u8 rsvd2[12];
	u32 utmi_ctrl5;
	u32 hs_phy_ctrl_common0;
	u32 hs_phy_ctrl_common1;
	u32 hs_phy_ctrl_common2;
	u32 hs_phy_ctrl1;
	u32 hs_phy_ctrl2;
	u8 rsvd3[4];
	u32 hs_phy_override_x0;
	u32 hs_phy_override_x1;
	u32 hs_phy_override_x2;
	u32 hs_phy_override_x3;
	u8 rsvd4[24];
	u32 cfg0;
	u8 rsvd5[8];
	u32 refclk_ctrl;
};
check_member(hs_usb_phy_reg, utmi_ctrl0, 0x03c);
check_member(hs_usb_phy_reg, utmi_ctrl1, 0x040);
check_member(hs_usb_phy_reg, utmi_ctrl5, 0x050);
check_member(hs_usb_phy_reg, hs_phy_ctrl2, 0x064);
check_member(hs_usb_phy_reg, hs_phy_override_x0, 0x06c);
check_member(hs_usb_phy_reg, hs_phy_override_x3, 0x078);
check_member(hs_usb_phy_reg, cfg0, 0x094);
check_member(hs_usb_phy_reg, refclk_ctrl, 0x0a0);
