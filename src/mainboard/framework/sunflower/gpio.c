/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/ramstage.h>

/*
 * Comments follow the form:
 *
 * Pad function / net name (notes)
 */
static const struct pad_config gpio_table[] = {
	/* ------- GPIO Community 0 ------- */

	/* ------- GPIO Group GPP_B ------- */
	PAD_CFG_NF(GPP_B0, NONE, PLTRST, NF1),				/* CORE_VID0 / VCCIN_AUX_CORE_VID0 */
	PAD_CFG_NF(GPP_B1, NONE, PLTRST, NF1),				/* CORE_VID1 / VCCIN_AUX_CORE_VID1 */
	PAD_CFG_NF(GPP_B2, NONE, PLTRST, NF1),				/* VRALERT# / VCCIN_AUX_CORE_ALERT# */
	PAD_CFG_GPI_INT(GPP_B3, UP_5K, PLTRST, LEVEL),			/* GPP_B3 / SOC_TP_INT# (Touchpad; internal pull-up required - no external pull on board or module) */
	PAD_NC(GPP_B4, NONE),
	PAD_NC(GPP_B5, NONE),						/* ISH_I2C0_SDA / ISH_I2C_0_SDA (NC) */
	PAD_NC(GPP_B6, NONE),						/* ISH_I2C0_SCL / ISH_I2C_0_SCL (NC) */
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),				/* SOC_PD_INT# (CYP6228) - PMCALERT# */
	PAD_CFG_NF(GPP_B12, NONE, PLTRST, NF1),				/* SLP_S0# / PM_SLP_S0# */
	PAD_CFG_NF(GPP_B13, NONE, PLTRST, NF1),				/* PLTRST# / SOC_PLTRST# */
	PAD_NC(GPP_B14, NONE),						/* SPKR - Connected to PCBEEP */
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B18, NONE),						/* ADR_COMPLETE (Strap, NC) */
	PAD_NC(GPP_B23, NONE),						/* SOC_GPP_B23 (Strap, NC) */

	/* ------- GPIO Group GPP_T ------- */
	PAD_NC(GPP_T2, NONE),						/* Testpoint T58 */
	PAD_NC(GPP_T3, NONE),						/* Testpoint T57 */

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A0, UP_20K, DEEP, NF1),		/* ESPI_IO0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A1, UP_20K, DEEP, NF1),		/* ESPI_IO1 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A2, UP_20K, DEEP, NF1),		/* ESPI_IO2 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A3, UP_20K, DEEP, NF1),		/* ESPI_IO3 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A4, UP_20K, DEEP, NF1),		/* ESPI_CS0# */
	PAD_NC(GPP_A5, NONE),
	PAD_NC(GPP_A6, NONE),
	PAD_NC(GPP_A7, NONE),
	PAD_NC(GPP_A8, NONE),
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A9, DN_20K, DEEP, NF1),		/* ESPI_CLK */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A10, NONE, DEEP, NF1),		/* ESPI_RESET# */
	PAD_NC(GPP_A11, NONE),                  			/* SOC_WL_OFF# - Not hooked up to M.2 connector */
	PAD_NC(GPP_A12, NONE),
	PAD_CFG_GPO(GPP_A13, 1, DEEP),					/* GPP_A13 / SOC_BT_ON */
	PAD_NC(GPP_A14, NONE),						/* USBC3_HPD - From PD controller to CPU and Redriver. Not hooked up. Notified through I2C */
	PAD_NC(GPP_A15, NONE),						/* USBC4_HPD - Same as USBC3_HPD, see above */
	PAD_CFG_NF(GPP_A16, NONE, PLTRST, NF1),				/* USB_OC3# / CCG6_PD2_OC3# */
	PAD_NC(GPP_A17, NONE),
	PAD_NC(GPP_A18, NONE),
	PAD_NC(GPP_A19, NONE),						/* USBC1_HPD - Same as USBC3_HPD, see above */
	PAD_NC(GPP_A20, NONE),						/* USBC2_HPD - Same as USBC3_HPD, see above */
	PAD_NC(GPP_A21, NONE),
	PAD_NC(GPP_A22, NONE),
	PAD_NC(GPP_A23, NONE),

	/* ------- GPIO Community 1 ------- */

	/* ------- GPIO Group GPP_S ------- */
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_NC(GPP_S6, NONE),						/* DMIC_CLK_A1 (NC, audio via HDA codec) */
	PAD_NC(GPP_S7, NONE),						/* DMIC_DATA1 (NC, audio via HDA codec) */

	/* ------- GPIO Group GPP_H ------- */
	PAD_NC(GPP_H0, NONE),						/* SOC_GPP_H0 (Strap, NC) */
	PAD_NC(GPP_H1, NONE),						/* Boot Strap 2 (NC) */
	PAD_NC(GPP_H2, DN_20K),						/* RTD3_WLAN_PLT_RST# - left as input like vendor; onboard 10k pull-up holds reset deasserted */
	PAD_NC(GPP_H3, NONE),
	PAD_CFG_NF(GPP_H4, NONE, PLTRST, NF1),				/* I2C0_SDA (Touchscreen) */
	PAD_CFG_NF(GPP_H5, NONE, PLTRST, NF1),				/* I2C0_SCL (Touchscreen) */
	PAD_CFG_NF(GPP_H6, NONE, PLTRST, NF1),				/* I2C1_SDA (EC HID) */
	PAD_CFG_NF(GPP_H7, NONE, PLTRST, NF1),				/* I2C1_SCL (EC HID) */
	PAD_CFG_NF(GPP_H8, NONE, PLTRST, NF1),				/* I2C4_SDA (Touchpad) */
	PAD_CFG_NF(GPP_H9, NONE, PLTRST, NF1),				/* I2C4_SCL (Touchpad) */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),				/* UART0_RXD / UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),				/* UART0_TXD / UART_0_CTXD_DRXD */
	PAD_NC(GPP_H12, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_CFG_NF(GPP_H18, NONE, PLTRST, NF1),				/* CPU_C10_GATE# */
	PAD_CFG_NF(GPP_H19, NONE, PLTRST, NF1),				/* CLKREQ_PCIE#4 */
	PAD_NC(GPP_H20, NONE),
	PAD_NC(GPP_H21, NONE),
	PAD_NC(GPP_H22, NONE),
	PAD_NC(GPP_H23, NONE),

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, NONE),						/* SOC_ACC_INT# (NC, ACC connected to EC, not ISH) */
	PAD_NC(GPP_D1, NONE),						/* SOC_ALS_INT# (NC, ALS connected to EC, not ISH) */
	PAD_NC(GPP_D2, NONE),						/* SOC_EC_INT2# (NC, no ALS on sunflower) */
	PAD_CFG_GPI_INT(GPP_D3, NONE, DEEP, LEVEL),			/* SOC_EC_INT# (EC Mediakeys, GpioInt; vendor pin 163) */
	PAD_NC(GPP_D4, NONE),
	PAD_NC(GPP_D5, NONE),
	PAD_NC(GPP_D6, NONE),
	PAD_NC(GPP_D7, NONE),
	PAD_CFG_NF(GPP_D8, NONE, PLTRST, NF1),				/* CLKREQ_PCIE#3 (SSD) */
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE),						/* TBT_2_LSX_RX_STRAP (Floating as far as I can tell) */
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, NONE),						/* TBT_3_LSX_RX_STRAP (Floating as far as I can tell) */
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),						/* SOC_ACC_MB_INT# (NC, ACC connected to EC, not ISH) */
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_CFG_GPO(GPP_D18, 1, DEEP),					/* TCHSCR_EN (100k pulldown) */
	PAD_CFG_GPI_SCI_HIGH(GPP_D19, UP_20K, PLTRST, EDGE_SINGLE),	/* GPP_D19 / EC_SCI# (matches vendor: rising-edge SCI, internal pull-up) */

	/* ------- GPIO Community 2 ------- */

	/* ------- GPIO Group GPP_GPD ------- */
	PAD_CFG_NF(GPD0, NONE, PWROK, NF1),				/* BATLOW# */
	PAD_CFG_NF(GPD1, NONE, PWROK, NF1),				/* ACPRESENT */
	PAD_NC(GPD2, NONE),						/* LAN_WAKE# (NC, no LAN) */
	PAD_CFG_NF(GPD3, NONE, PWROK, NF1),				/* PWRBTN# */
	PAD_CFG_NF(GPD4, NONE, PWROK, NF1),				/* SLP_S3# */
	PAD_CFG_NF(GPD5, NONE, PWROK, NF1),				/* SLP_S4# */
	PAD_CFG_NF(GPD6, NONE, PWROK, NF1),				/* SLP_A# */
	PAD_NC(GPD7, NONE),						/* TBT_RETIMER_RESET#_R (NC) */
	PAD_CFG_NF(GPD8, NONE, PWROK, NF1),				/* SUSCLK */
	PAD_CFG_NF(GPD9, NONE, PWROK, NF1),				/* SLP_WLAN# */
	PAD_CFG_NF(GPD10, NONE, PWROK, NF1),				/* SLP_S5# */
	PAD_NC(GPD11, NONE),

	/* ------- GPIO Community 4 ------- */

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, PLTRST, NF1),				/* SMBCLK / SOC_SMBCLK */
	PAD_CFG_NF(GPP_C1, NONE, PLTRST, NF1),				/* SMBDATA / SOC_SMBDATA */
	PAD_NC(GPP_C2, NONE),						/* SOC_GPP_C2 (TLS CONFIDENTIALITY Strap, 4.7k up to +3V_PRIM) */
	PAD_NC(GPP_C3, NONE),						/* SML0CLK (NC) */
	PAD_NC(GPP_C4, NONE),						/* SML0DATA (NC) */
	PAD_NC(GPP_C5, NONE),						/* SOC_GPP_C5 (Strap, NC) */
	PAD_CFG_NF(GPP_C6, NONE, RSMRST, NF1),				/* SML1CLK */
	PAD_CFG_NF(GPP_C7, NONE, RSMRST, NF1),				/* SML1DATA */

	/* ------- GPIO Group GPP_F ------- */
	PAD_CFG_NF(GPP_F0, NONE, PLTRST, NF1),				/* CNV_BRI_DT / CNV_BRI_CTX_DRX */
	PAD_CFG_NF(GPP_F1, NONE, PLTRST, NF1),				/* CNV_BRI_RSP / CNV_BRI_CRX_DTX */
	PAD_CFG_NF(GPP_F2, NONE, PLTRST, NF1),				/* CNV_RGI_DT / CNV_RGI_CTX_DRX */
	PAD_CFG_NF(GPP_F3, NONE, PLTRST, NF1),				/* CNV_RGI_RSP / CNV_RGI_CRX_DTX */
	PAD_CFG_NF(GPP_F4, NONE, PLTRST, NF1),				/* CNV_RF_RESET# / CNV_RF_RESET# */
	PAD_CFG_NF(GPP_F5, NONE, PLTRST, NF2),				/* MODEM_CLKREQ / CLKREQ_CNV# */
	PAD_NC(GPP_F6, NONE),
	PAD_NC(GPP_F7, NONE),						/* GPP_F7_STRAP (NC) */
	PAD_NC(GPP_F9, NONE),						/* BOOTMPC (Strap, NC) */
	PAD_NC(GPP_F10, NONE),						/* GPP_F10_STRAP (NC) */
	PAD_NC(GPP_F11, NONE),						/* TPM_PIRQ# (NC, fTPM) */
	PAD_CFG_GPI_INT(GPP_F12, UP_20K, PLTRST, EDGE_BOTH),		/* EC_PAD_MODE - tablet-mode notify from EC (GPIO07, open-drain): high=tablet, low=clamshell. OS reads it via INT33D3/PNP0C60 (see tablet.asl). Pull-up needed for the open-drain high state */
	PAD_CFG_GPI_INT(GPP_F13, NONE, PLTRST, LEVEL),			/* TS_INT# (Touchscreen, GpioInt; vendor pin 301) */
	PAD_CFG_GPO(GPP_F14, 1, PLTRST),				/* TOUCH_SCREEN_RST# */
	PAD_CFG_GPO(GPP_F15, 1, PLTRST),				/* TS_EN */
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_NC(GPP_F19, NONE),
	PAD_CFG_GPO(GPP_F20, 1, PLTRST),				/* RTD3_SSD_PLT_RST# */
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),
	PAD_CFG_NF(GPP_F23, NONE, PLTRST, NF1),				/* V1P05_CTRL */

	/* ------- GPIO Group GPP_E ------- */
	PAD_NC(GPP_E0, NONE),
	PAD_NC(GPP_E1, NONE),
	PAD_NC(GPP_E2, NONE),
	PAD_NC(GPP_E3, NONE),
	PAD_NC(GPP_E4, NONE),
	PAD_NC(GPP_E5, NONE),						/* TABLET_MODE#, pullup to +3V_PRIM (Input from lid hall sensor) */
	PAD_NC(GPP_E6, NONE), /* GPP_E6 / GPP_E6 (Strap) */
	PAD_NC(GPP_E7, NONE),
	PAD_NC(GPP_E8, NONE),						/* SLP_DRAM# (100k pulldown, drive low; no NF on E8) */
	PAD_CFG_NF(GPP_E9, NONE, PLTRST, NF1),				/* USB_OC0# / CCG6_PD1_OC0# */
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E13, NONE),
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),				/* DDSP_HPDA / EDP_HPD */
	PAD_NC(GPP_E15, NONE),
	PAD_NC(GPP_E16, NONE),
	PAD_NC(GPP_E17, NONE),
	PAD_NC(GPP_E18, NONE),
	PAD_NC(GPP_E19, NONE),						/* TBT_0_LSX_RX / TBT_0_LSX_RX_STRAP (NC, no TBT/USB4) */
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_E21, NONE),						/* TBT_1_LSX_RX / TBT_1_LSX_RX_STRAP (NC, no TBT/USB4) */
	PAD_NC(GPP_E22, NONE),
	PAD_NC(GPP_E23, NONE),

	/* ------- GPIO Community 5 ------- */

	/* ------- GPIO Group GPP_R ------- */
	PAD_CFG_NF(GPP_R0, NONE, PLTRST, NF1),				/* HDA_BCLK */
	PAD_CFG_NF(GPP_R1, NONE, PLTRST, NF1),				/* HDA_SYNC */
	PAD_CFG_NF(GPP_R2, NONE, PLTRST, NF1),				/* HDA_SDO */
	PAD_CFG_NF(GPP_R3, NONE, PLTRST, NF1),				/* HDA_SDI0 */
	PAD_NC(GPP_R4, NONE),						/* WLAN_PCM_CLK (BT Offload, unused) */
	PAD_NC(GPP_R5, NONE),						/* WLAN_PCM_FRM (BT Offload, unused) */
	PAD_NC(GPP_R6, NONE),						/* WLAN_PCM_IN  (BT Offload, unused) */
	PAD_NC(GPP_R7, NONE),						/* WLAN_PCM_OUT (BT Offload, unused) */
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
