/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/usb/snps_usb_phy.h>
#include <soc/usb/usb.h>

#define SNPS_PHY_POR_ASSERT_DELAY_US	10
#define SNPS_PHY_STABILIZE_DELAY_US	20

/* Table 2: Power on Reset Sequence for ATE and Software (SW) */

/* cfg0 bits */
#define UTMI_PHY_CMN_CTRL_OVERRIDE_EN	BIT(1)	/* USB_PHY_CFG0[1] */

/* utmi_ctrl5 bits */
#define POR				BIT(1)	/* USB_PHY_UTMI_CTRL5[1] */

/* hs_phy_ctrl_common0 bits */
#define USB_PHY_ENABLE_BIT		BIT(0)	/* [0] PHY_ENABLE */
#define SIDDQ_SEL_BIT			BIT(1)	/* [1] SIDDQ MUX Selection */
#define SIDDQ_BIT			BIT(2)	/* [2] SIDDQ */
#define FSEL_MASK			GENMASK(6, 4)	/* [6:4] FSEL */
#define FSEL_38_4_MHZ_VAL		(0x6 << 4)	/* 3'b110 = 38.4 MHz (default) */

/* cfg1 bits (offset 0x154) */
#define PLL_EN_BIT			BIT(0)	/* USB_PHY_CFG1[0] PLL_EN */

/* xcfgi_199_192 bits (offset 0x1bc) */
#define XCFGI_HS_DISCONNECT_EN_TX	BIT(4)	/* XCFGI[196] XCfg_HS_DISCONNECT_EN_TX */

/* xcfgi_159_152 bits (offset 0x1a8) */
#define XCFGI_SE0_COUNTER		BIT(7)	/* XCFGI[159] SE0 counter enable */

/* xcfgi_39_32 bits (offset 0x16c) */
#define XCFGI_HSTX_DEEMPH_MASK		GENMASK(3, 2)	/* [3:2] HSTX de-emphasis */
/* 2'b00 = 0dB (default) */

/* xcfgi_71_64 bits (offset 0x17c) */
#define XCFGI_HSTX_SWING_MASK		GENMASK(3, 0)	/* [3:0] HSTX swing */
#define XCFGI_HSTX_SWING_216MV		0x7		/* 4'b0111 = 216.65mV */

/* xcfgi_31_24 bits (offset 0x168) */
#define XCFGI_HSTX_SLEW_MASK		GENMASK(2, 0)	/* [2:0] HSTX slew rate */
/* 3'b000 = 148ps (default) */

/* xcfgi_7_0 bits (offset 0x15c) */
#define XCFGI_PLL_LOCK_MASK		GENMASK(1, 0)	/* [1:0] PLL lock time */
/* 2'b00 = 35us (default) */

/* usb_phy_fsel_sel bits */
#define FSEL_SEL_BIT			BIT(0)	/* USB_PHY_FSEL_SEL[0] */

/* hs_phy_ctrl2 bits */
#define USB2_SUSPEND_N_SEL_BIT		BIT(3)	/* [3] USB2_SUSPEND_N_SEL */
#define USB2_SUSPEND_N_BIT		BIT(2)	/* [2] USB2_SUSPEND_N */

/* utmi_ctrl0 bits */
#define SLEEPM				BIT(0)	/* [0] SLEEPM */

void hs_usb_phy_init(int index)
{
	struct hs_usb_phy_reg *hs_phy_reg;

	switch (index) {
	case 0:
		hs_phy_reg = (void *)HS_USB_MP0_PHY_BASE;
		break;
	case 1:
		hs_phy_reg = (void *)HS_USB_MP1_PHY_BASE;
		break;
	default:
		printk(BIOS_ERR, "%s: invalid PHY index %d\n", __func__, index);
		return;
	}

	/*
	 * eUSB2 PHY (M31) power-on reset sequence (Table 2: ATE/SW).
	 * Sequence: SW override -> assert POR -> configure -> release POR -> restore HW.
	 */

	/* Enable SW override for common control MUX */
	clrsetbits32(&hs_phy_reg->cfg0,
		UTMI_PHY_CMN_CTRL_OVERRIDE_EN, UTMI_PHY_CMN_CTRL_OVERRIDE_EN);

	/* Assert POR; hold >=10us after supplies ramp */
	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, POR);
	udelay(SNPS_PHY_POR_ASSERT_DELAY_US);

	/* Enable PHY and configure refclk/interface */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0,
		USB_PHY_ENABLE_BIT, USB_PHY_ENABLE_BIT);
	clrsetbits32(&hs_phy_reg->cfg1, PLL_EN_BIT, 0);
	clrsetbits32(&hs_phy_reg->usb_phy_fsel_sel, FSEL_SEL_BIT, FSEL_SEL_BIT);
	/* Set refclk to 38.4 MHz */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, FSEL_MASK, FSEL_38_4_MHZ_VAL);

	/* XCFGI defaults: HSTX de-emphasis=0dB, swing=216.65mV, slew=148ps, PLL lock=35us */
	clrsetbits32(&hs_phy_reg->xcfgi_39_32, XCFGI_HSTX_DEEMPH_MASK, 0);
	clrsetbits32(&hs_phy_reg->xcfgi_71_64, XCFGI_HSTX_SWING_MASK, XCFGI_HSTX_SWING_216MV);
	clrsetbits32(&hs_phy_reg->xcfgi_31_24, XCFGI_HSTX_SLEW_MASK, 0);
	clrsetbits32(&hs_phy_reg->xcfgi_7_0, XCFGI_PLL_LOCK_MASK, 0);

	/* Select SW suspend_n and assert SLEEPM */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2,
		USB2_SUSPEND_N_SEL_BIT, USB2_SUSPEND_N_SEL_BIT);
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2,
		USB2_SUSPEND_N_BIT, USB2_SUSPEND_N_BIT);
	clrsetbits32(&hs_phy_reg->utmi_ctrl0, SLEEPM, SLEEPM);

	/* Power up analog blocks: SIDDQ_SEL=1, SIDDQ=0 */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, SIDDQ_SEL_BIT, SIDDQ_SEL_BIT);
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, SIDDQ_BIT, 0);

	/*
	 * Write XCFGI after SIDDQ=0 (analog powered), before POR release.
	 * XCFGI[196]=1: disable HS disconnect (host mode compliance fix).
	 * XCFGI[159]=1: SE0 counter, resets state machine on SE0 > 2.5ms.
	 */
	clrsetbits32(&hs_phy_reg->xcfgi_199_192,
		XCFGI_HS_DISCONNECT_EN_TX, XCFGI_HS_DISCONNECT_EN_TX);
	clrsetbits32(&hs_phy_reg->xcfgi_159_152,
		XCFGI_SE0_COUNTER, XCFGI_SE0_COUNTER);

	/* Release POR and restore HW control */
	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, 0);
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, USB2_SUSPEND_N_SEL_BIT, 0);
	clrsetbits32(&hs_phy_reg->cfg0, UTMI_PHY_CMN_CTRL_OVERRIDE_EN, 0);

	/* Wait for PHY to stabilize after POR release */
	udelay(SNPS_PHY_STABILIZE_DELAY_US);

	printk(BIOS_DEBUG, "USB HS PHY %d initialized\n", index);
}
