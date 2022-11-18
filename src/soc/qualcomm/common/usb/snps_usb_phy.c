/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/usb/snps_usb_phy.h>
#include <soc/addressmap.h>
#include <commonlib/helpers.h>
#include <soc/usb/usb_common.h>

#define SLEEPM					BIT(0)

#define TERMSEL					BIT(5)

#define POR					BIT(1)

#define FSEL_MASK				GENMASK(7, 5)
#define FSEL_DEFAULT				(0x3 << 4)

#define VBUSVLDEXTSEL0				BIT(4)
#define PLLBTUNE				BIT(5)

#define VREGBYPASS				BIT(0)

#define VBUSVLDEXT0				BIT(0)

#define USB2_SUSPEND_N				BIT(2)
#define USB2_SUSPEND_N_SEL			BIT(3)

#define UTMI_PHY_CMN_CTRL_OVERRIDE_EN		BIT(1)

#define REFCLK_SEL_MASK				GENMASK(1, 0)
#define REFCLK_SEL_DEFAULT			(0x2 << 0)

#define PARAM_OVRD_MASK			0xFF

struct hs_usb_phy_reg *hs_phy_reg = (void *)HS_USB_PRIM_PHY_BASE;

void hs_usb_phy_init(void *board_data)
{
	struct usb_board_data *override_data =
				(struct usb_board_data *)board_data;

	clrsetbits32(&hs_phy_reg->cfg0, UTMI_PHY_CMN_CTRL_OVERRIDE_EN,
					UTMI_PHY_CMN_CTRL_OVERRIDE_EN);

	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, POR);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, FSEL_MASK, 0);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common1, PLLBTUNE, PLLBTUNE);

	clrsetbits32(&hs_phy_reg->refclk_ctrl, REFCLK_SEL_MASK,
					REFCLK_SEL_DEFAULT);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common1, VBUSVLDEXTSEL0,
					VBUSVLDEXTSEL0);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl1, VBUSVLDEXT0, VBUSVLDEXT0);

	if (board_data)	{
		clrsetbits32(&hs_phy_reg->hs_phy_override_x0,
			PARAM_OVRD_MASK, override_data->parameter_override_x0);

		clrsetbits32(&hs_phy_reg->hs_phy_override_x1,
			PARAM_OVRD_MASK, override_data->parameter_override_x1);

		clrsetbits32(&hs_phy_reg->hs_phy_override_x2,
			PARAM_OVRD_MASK, override_data->parameter_override_x2);

		clrsetbits32(&hs_phy_reg->hs_phy_override_x3,
			PARAM_OVRD_MASK, override_data->parameter_override_x3);
	}
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common2, VREGBYPASS, VREGBYPASS);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2,
					USB2_SUSPEND_N_SEL | USB2_SUSPEND_N,
					USB2_SUSPEND_N_SEL | USB2_SUSPEND_N);

	clrsetbits32(&hs_phy_reg->utmi_ctrl0, SLEEPM, SLEEPM);

	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, 0);

	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, USB2_SUSPEND_N_SEL, 0);

	clrsetbits32(&hs_phy_reg->cfg0, UTMI_PHY_CMN_CTRL_OVERRIDE_EN, 0);

	printk(BIOS_DEBUG, "USB HS PHY initialized\n");
}
