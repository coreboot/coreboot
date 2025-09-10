/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/usb/snps_usb_phy.h>
#include <soc/addressmap.h>
#include <commonlib/helpers.h>
#include <soc/usb/usb.h>
#include <delay.h>

#define SLEEPM BIT(0)

#define TERMSEL BIT(5)

#define POR BIT(1)

/* Updated FSEL_MASK based on the table feedback (GENMASK(6,4) for 38.4MHz) */
#define FSEL_MASK GENMASK(6, 4)
#define FSEL_DEFAULT (0x3 << 4) /* Original value, may not be used directly */

#define VBUSVLDEXTSEL0 BIT(4)
#define PLLBTUNE BIT(5)

#define VREGBYPASS BIT(0)

#define VBUSVLDEXT0 BIT(0)

/* Updated macros for clarity as per feedback */
#define USB2_SUSPEND_N_BIT BIT(2)
#define USB2_SUSPEND_N_SEL_BIT BIT(3)

#define UTMI_PHY_CMN_CTRL_OVERRIDE_EN BIT(1)

#define REFCLK_SEL_MASK GENMASK(1, 0)
#define REFCLK_SEL_DEFAULT (0x2 << 0)

#define PARAM_OVRD_MASK 0xFF

/* Streamlined macros for common control bits (from previous feedback) */
#define USB_PHY_ENABLE_BIT BIT(0)
#define USB_PHY_RETENABLEN_BIT BIT(3)
#define APB_LOGIC_RESET_BIT BIT(2)
#define TESTBURNIN_BIT BIT(6)
#define FSEL_SEL_BIT BIT(0)

/* New macros from table requirements (to be used in sequence) */
#define FSEL_38_4_MHZ_VAL (0x4 << 4) /* 3'b100 for bits 6:4 -> 0x4, shifted by 4 */
#define SIDDQ_SEL_BIT BIT(1)
#define SIDDQ_DISABLE_BIT BIT(2) /* SIDDQ is set to 0, means power up analog blocks */

#define CFG_CTRL_2_VALUE 0xC8 /* For phy_cfg_pll_fb_div_7_0 */

#define CFG_CTRL_3_FB_DIV_MASK GENMASK(3, 0)
#define CFG_CTRL_3_FB_DIV_VALUE 0x0 /* 4'b0000 */

#define CFG_CTRL_3_REF_DIV_MASK GENMASK(7, 4)
#define CFG_CTRL_3_REF_DIV_VALUE (0x0 << 4) /* 4'b0000 shifted to bits 7:4 */

#define CFG_CTRL_1_PLL_CPBIAS_MASK GENMASK(7, 1)
#define CFG_CTRL_1_PLL_CPBIAS_VALUE (0x0 << 1) /* 7'b0000000 shifted to bits 7:1 */

#define CFG_CTRL_4_PLL_VCO_GAIN_MASK GENMASK(1, 0)
#define CFG_CTRL_4_PLL_VCO_GAIN_VALUE 0x1 /* 2'b01 */

#define CFG_CTRL_4_PLL_INT_GAIN_MASK GENMASK(7, 2)
#define CFG_CTRL_4_PLL_INT_GAIN_VALUE (0x8 << 2) /* 6'b001000 shifted to bits 7:2 */

#define CFG_CTRL_5_PLL_PROP_GAIN_MASK GENMASK(5, 0)
#define CFG_CTRL_5_PLL_PROP_GAIN_VALUE 0x10 /* 6'b010000 */

#define CFG_CTRL_6_PLL_VCO_CFG_MASK GENMASK(2, 0)
#define CFG_CTRL_6_PLL_VCO_CFG_VALUE 0x0 /* 3'b000 */

#define CFG_CTRL_5_PLL_VREF_TUNE_MASK GENMASK(7, 6)
#define CFG_CTRL_5_PLL_VREF_TUNE_VALUE (0x1 << 6) /* 2'b01 shifted to bits 7:6 */

#define VBUS_DET_EXT_SEL_BIT BIT(4)
#define VBUS_VALID_EXT_BIT BIT(0)

#define CFG_CTRL_9_TX_PREEMP_MASK GENMASK(2, 0)
#define CFG_CTRL_9_TX_PREEMP_VALUE 0x0 /* 3'b000 */

#define CFG_CTRL_8_TX_HS_VREF_MASK GENMASK(5, 3)
#define CFG_CTRL_8_TX_HS_VREF_VALUE (0x3 << 3) /* 3'b011 shifted to bits 5:3 */

#define CFG_CTRL_9_TX_RISE_TUNE_MASK GENMASK(6, 5)
#define CFG_CTRL_9_TX_RISE_TUNE_VALUE (0x2 << 5) /* 2'b10 shifted to bits 6:5 */

#define CFG_CTRL_8_TX_HS_XV_TUNE_MASK GENMASK(7, 6)
#define CFG_CTRL_8_TX_HS_XV_TUNE_VALUE (0x0 << 6) /* 2'b00 shifted to bits 7:6 */

#define CFG_CTRL_9_TX_RES_TUNE_MASK GENMASK(4, 3)
#define CFG_CTRL_9_TX_RES_TUNE_VALUE (0x1 << 3) /* 2'b01 shifted to bits 4:3 */


struct hs_usb_phy_reg *hs_phy_reg = NULL;
void hs_usb_phy_init(int index)
{
	if (index == 0)
		hs_phy_reg = (void *)HS_USB_MP0_PHY_BASE;
	else
		hs_phy_reg = (void *)HS_USB_MP1_PHY_BASE;

	/*
	 * This sequence initializes the USB HS PHY.
	 */

	/* Step 1: USB_PHY_CFG0[1] = 1'b1 (UTMI_PHY_CMN_CNTRL_OVERRIDE_EN=1) */
	/* This is a MUX select signal to enable software override of UTMI PHY common control. */
	clrsetbits32(&hs_phy_reg->cfg0, UTMI_PHY_CMN_CTRL_OVERRIDE_EN,
		UTMI_PHY_CMN_CTRL_OVERRIDE_EN);

	/* Step 2: USB_PHY_UTMI_CTRL5[1] = 1'b1 (POR asserted) */
	/* Assert the PHY reset. This reset must be held for at least 10us after all supplies ramp up. */
	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, POR);
	udelay(10); /* Hold POR for 10us */

	/* Step 3: USB_PHY_HS_PHY_CTRL_COMMON0[0] = 1'b1 (phy_enable='1') */
	/* Asynchronous enable for the PHY. If low during POR de-assertion, PHY remains inactive. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, USB_PHY_ENABLE_BIT, USB_PHY_ENABLE_BIT);

	/* Step 4: USB_PHY_HS_PHY_CTRL_COMMON0[3] = 1'b1 (Retention Mode Enable) */
	/* Set retenable_n = 1'b1. This signal must be high when VDD supply is powered. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, USB_PHY_RETENABLEN_BIT, USB_PHY_RETENABLEN_BIT);

	/* Step 5: USB_PHY_APB_ACCESS_CMD[2] = 1'b1 (APB Logic Reset) */
	/* Reset all APB related logic, including SNPS APB FSM but not the APB registers. */
	clrsetbits32(&hs_phy_reg->usb_phy_apb_access_cmd, APB_LOGIC_RESET_BIT, APB_LOGIC_RESET_BIT);

	/* Step 6: UTMI_PHY_CMN_CTRL0[6] = 1'b0 (Burn-in Test Enable - clear the bit) */
	/* De-assert test_burnin. If asserted, PHY performs power-on-reset and enters HS Loopback mode. */
	clrsetbits32(&hs_phy_reg->utmi_phy_cmn_ctrl0, TESTBURNIN_BIT, 0);

	/* Step 7: USB_PHY_FSEL_SEL = 1'b1 (FSEL MUX select for SW override) */
	/* Select software override for frequency selection. */
	clrsetbits32(&hs_phy_reg->usb_phy_fsel_sel, FSEL_SEL_BIT, FSEL_SEL_BIT);

	/* Step 8: USB_PHY_HS_PHY_CTRL_COMMON0[6:4] = 3'b100 (Sets refclk frequency to 38.4 MHz) */
	/* Configure the reference clock frequency. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, FSEL_MASK, FSEL_38_4_MHZ_VAL);

	/* Step 9: USB_PHY_CFG_CTRL_2[7:0] = 8'b11001000 (Control of the feedback multiplication ratio) */
	/* Configure phy_cfg_pll_fb_div_7_0. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_2, 0xFF, CFG_CTRL_2_VALUE); /* Using 0xFF for full 8-bit mask */

	/* Step 10: USB_PHY_CFG_CTRL_3[3:0] = 4'b0000 (Control of the feedback multiplication ratio) */
	/* Configure phy_cfg_pll_fb_div_11_8. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_3, CFG_CTRL_3_FB_DIV_MASK, CFG_CTRL_3_FB_DIV_VALUE);

	/* Step 11: USB_PHY_CFG_CTRL_3[7:4] = 4'b0000 (Control of the input frequency division ratio) */
	/* Configure phy_cfg_pll_ref_div. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_3, CFG_CTRL_3_REF_DIV_MASK, CFG_CTRL_3_REF_DIV_VALUE);

	/* Step 12: USB_PHY_CFG_CTRL_1[7:1] = 7'b0000000 (PLL loop bias configuration) */
	/* Configure phy_cfg_pll_cpbias_cntrl. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_1, CFG_CTRL_1_PLL_CPBIAS_MASK, CFG_CTRL_1_PLL_CPBIAS_VALUE);

	/* Step 13: USB_PHY_CFG_CTRL_4[1:0] = 2'b01 (PLL vco source gain) */
	/* Configure phy_cfg_pll_gmp_cntrl. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_4, CFG_CTRL_4_PLL_VCO_GAIN_MASK, CFG_CTRL_4_PLL_VCO_GAIN_VALUE);

	/* Step 14: USB_PHY_CFG_CTRL_4[7:2] = 6'b001000 (Gain of PLL Integral Charge Pump) */
	/* Configure phy_cfg_pll_int_cntrl. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_4, CFG_CTRL_4_PLL_INT_GAIN_MASK, CFG_CTRL_4_PLL_INT_GAIN_VALUE);

	/* Step 15: USB_PHY_CFG_CTRL_5[5:0] = 6'b010000 (Gain of PLL Proportional Charge Pump) */
	/* Configure phy_cfg_pll_prop_cntrl. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_5, CFG_CTRL_5_PLL_PROP_GAIN_MASK, CFG_CTRL_5_PLL_PROP_GAIN_VALUE);

	/* Step 16: USB_PHY_CFG_CTRL_6[2:0] = 3'b000 (PLL VCO configuration) */
	/* Configure phy_cfg_pll_vco_cntrl. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_6, CFG_CTRL_6_PLL_VCO_CFG_MASK, CFG_CTRL_6_PLL_VCO_CFG_VALUE);

	/* Step 17: USB_PHY_CFG_CTRL_5[7:6] = 2'b01 (Configuration for PLL voltage reference level) */
	/* Configure phy_cfg_pll_vref_tune. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_5, CFG_CTRL_5_PLL_VREF_TUNE_MASK, CFG_CTRL_5_PLL_VREF_TUNE_VALUE);

	/* Step 18: Selects UTMI data bus width. Wordinterface<#> = 0; for 8 bits interface (60 MHz)
	 * No register control, tied at chip top.
	 */

	/* Step 19: USB_PHY_HS_PHY_CTRL2[4] = 1'b1 (VBUS MUX Selection) */
	/* Select software control for VBUS detection. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, VBUS_DET_EXT_SEL_BIT, VBUS_DET_EXT_SEL_BIT);

	/* Step 20: USB_PHY_HS_PHY_CTRL2[0] = 1'b1 (VBUS valid indication) */
	/* Software writes this bit to indicate VBUS status. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, VBUS_VALID_EXT_BIT, VBUS_VALID_EXT_BIT);

	/* Step 21: USB_PHY_CFG_CTRL_9[2:0] = 3'b000 (HS Transmitter Pre-Emphasis Control) */
	/* Set HS Transmitter pre-emphasis to off (design default). */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_9, CFG_CTRL_9_TX_PREEMP_MASK, CFG_CTRL_9_TX_PREEMP_VALUE);

	/* Step 22: USB_PHY_CFG_CTRL_8[5:3] = 3'b011 (HS DC Voltage Level Adjustment) */
	/* Set HS DC Voltage Level to default (increased). */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_8, CFG_CTRL_8_TX_HS_VREF_MASK, CFG_CTRL_8_TX_HS_VREF_VALUE);

	/* Step 23: USB_PHY_CFG_CTRL_9[6:5] = 2'b10 (HS Transmitter Rise/Fall Time Adjustment) */
	/* Set HS Transmitter Rise/Fall Time Adjustment to default. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_9, CFG_CTRL_9_TX_RISE_TUNE_MASK, CFG_CTRL_9_TX_RISE_TUNE_VALUE);

	/* Step 24: USB_PHY_CFG_CTRL_8[7:6] = 2'b00 (Transmitter High-Speed Crossover Adjustment) */
	/* Set Transmitter High-Speed Crossover Adjustment to default (0V). */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_8, CFG_CTRL_8_TX_HS_XV_TUNE_MASK, CFG_CTRL_8_TX_HS_XV_TUNE_VALUE);

	/* Step 25: USB_PHY_CFG_CTRL_9[4:3] = 2'b01 (USB Source Impedance Adjustment) */
	/* Set USB Source Impedance Adjustment to default. */
	clrsetbits32(&hs_phy_reg->cfg_ctrl_9, CFG_CTRL_9_TX_RES_TUNE_MASK, CFG_CTRL_9_TX_RES_TUNE_VALUE);

	/* Step 26: USB_PHY_HS_PHY_CTRL2[3] = 1'b1 (USB2_SUSPEND_N_SEL) */
	/* Select SW option for suspend_n signal. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, USB2_SUSPEND_N_SEL_BIT, USB2_SUSPEND_N_SEL_BIT);

	/* Step 27: USB_PHY_HS_PHY_CTRL2[2] = 1'b1 (USB2_SUSPEND_N) */
	/* Assert USB2_SUSPEND_N. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, USB2_SUSPEND_N_BIT, USB2_SUSPEND_N_BIT);

	/* Step 28: USB_PHY_UTMI_CTRL0[0] = 1'b1 (SLEEPM) */
	/* Set sleepm = 1; until PHYCLOCK is available. */
	clrsetbits32(&hs_phy_reg->utmi_ctrl0, SLEEPM, SLEEPM);

	/* Step 29: USB_PHY_HS_PHY_CTRL_COMMON0[1] = 1'b1 (SIDDQ MUX Selection) */
	/* Select internal CSR register (SIDDQ) for SIDDQ control. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, SIDDQ_SEL_BIT, SIDDQ_SEL_BIT);

	/* Step 30: USB_PHY_HS_PHY_CTRL_COMMON0[2] = 1'b0 (SIDDQ set to 0) */
	/* Power up the analog blocks by setting SIDDQ to 0. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl_common0, SIDDQ_DISABLE_BIT, 0);

	/* Step 31: USB_PHY_UTMI_CTRL5[1] = 1'b0 (POR release) */
	/* Release the POR from high to low. */
	clrsetbits32(&hs_phy_reg->utmi_ctrl5, POR, 0);

	/* Step 32: USB_PHY_HS_PHY_CTRL2[3] = 1'b0 (USB2_SUSPEND_N_SEL de-selects SW override) */
	/* De-select SW override for suspend_n, allowing HW control. */
	clrsetbits32(&hs_phy_reg->hs_phy_ctrl2, USB2_SUSPEND_N_SEL_BIT, 0);

	udelay(20);
	printk(BIOS_DEBUG, "USB HS PHY initialized for index %d\n", index);
}
