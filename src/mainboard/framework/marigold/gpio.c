/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * GPIO configuration from vendor firmware dump (inteltool -G)
 * and schematic review.
 */

#include <gpio.h>
#include <mainboard/ramstage.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPP_V - Power Management ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_V00, NONE, DEEP, NF1),		/* BATLOW# / PCH_BATLOW# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_V01, NONE, DEEP, NF1),		/* ACPRESENT / AC_PRESENT */
	PAD_NC(GPP_V02, NONE),						/* SOC_WAK# / LAN_WAKE# (unused) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_V03, UP_20K, DEEP, NF1),	/* PWRBTN# / PBTN_OUT# (from EC) */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),				/* SLP_S3# */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),				/* SLP_S4# */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),				/* SLP_A# */
	PAD_NC(GPP_V07, NONE),						/* Does not exist */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),				/* SUSCLK (Suspend Clock) */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),				/* SLP_WLAN# / PM_SLP_WLAN# */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),				/* SLP_S5# */
	PAD_NC(GPP_V11, NONE),						/* LANPHYPC (not connected) */
	PAD_NC(GPP_V12, NONE),						/* SLP_LAN# (only testpoint TP3) */
	PAD_NC(GPP_V13, NONE),						/* Does not exist */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),				/* WAKE# / SOC_WL_WAKE# */
	PAD_NC(GPP_V15, NONE),						/* Does not exist */
	PAD_NC(GPP_V16, NONE),						/* Does not exist */
	PAD_NC(GPP_V17, NONE),						/* Does not exist */
	PAD_NC(GPP_V18, NONE),						/* Does not exist */
	PAD_NC(GPP_V22, NONE),
	PAD_NC(GPP_V23, NONE),

	/* ------- GPP_C - SMBus, SML, CLKREQs, TBT ------- */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),				/* SMBCLK / SOC_SMBCLK (DDR) */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),				/* SMBDATA /SOC_SMBDATA (DDR) */
	PAD_CFG_GPI(GPP_C02, NONE, DEEP),				/* GPP_C2_STRAP (TLS strap, has 4.7k pullup to 1.8V_PRIM) */
	PAD_CFG_NF(GPP_C03, NONE, DEEP, NF1),				/* SML0CLK (TBT, has 2.2k pullup to 1.8V_PRIM) */
	PAD_CFG_NF(GPP_C04, NONE, DEEP, NF1),				/* SML0DATA (TBT, has 2.2k pullup to 1.8V_PRIM) */
	PAD_CFG_GPI(GPP_C05, NONE, DEEP),				/* GPP_C5_STRAP (eSPI strap) */
	PAD_CFG_NF(GPP_C06, NONE, RSMRST, NF1),				/* SML1CLK (PD 1/2) */
	PAD_CFG_NF(GPP_C07, NONE, RSMRST, NF1),				/* SML1DATA (PD 1/2) */
	PAD_CFG_NF(GPP_C08, NONE, DEEP, NF1),				/* SML1ALERT# */
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1),				/* SRCCLKREQ0# (SSD) */
	PAD_NC(GPP_C10, NONE),
	PAD_CFG_NF(GPP_C11, NONE, DEEP, NF1),				/* SRCCLKREQ2# (WLAN) */
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),						/* Does not exist */
	PAD_NC(GPP_C15, NONE),						/* GPP_C15_STRAP (Reserved) */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),				/* TBT_LSX0_TXD */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),				/* TBT_LSX0_RXD */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),				/* TBT_LSX1_TXD */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),				/* TBT_LSX1_RXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),				/* TBT_LSX2_TXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),				/* TBT_LSX2_RXD */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),				/* TBT_LSX3_TXD */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),				/* TBT_LSX3_RXD */

	/* ------- GPP_A - eSPI ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A00, UP_20K, DEEP, NF1),	/* ESPI_IO0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A01, UP_20K, DEEP, NF1),	/* ESPI_IO1 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A02, UP_20K, DEEP, NF1),	/* ESPI_IO2 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A03, UP_20K, DEEP, NF1),	/* ESPI_IO3 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A04, UP_20K, DEEP, NF1),	/* ESPI_CS0# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A05, UP_20K, DEEP, NF1),	/* ESPI_CLK */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A06, NONE, DEEP, NF1),		/* ESPI_RESET# */
	PAD_NC(GPP_A07, NONE),						/* Does not exist */
	PAD_NC(GPP_A08, NONE),						/* Does not exist */
	PAD_NC(GPP_A09, NONE),						/* Does not exist */
	PAD_NC(GPP_A10, NONE),						/* Does not exist */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),					/* RTD3_WLAN_PLT_RST# (10k to 1.8VS) */
	PAD_NC(GPP_A12, NONE),
	PAD_NC(GPP_A13, NONE),
	PAD_NC(GPP_A14, NONE),
	PAD_NC(GPP_A15, NONE),
	PAD_NC(GPP_A16, NONE),
	PAD_NC(GPP_A17, NONE),
	PAD_NC(GPP_A18, NONE),
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A21, NONE, DEEP, NF1),		/* PMCALERT# / SOC_PD_INT# */
	PAD_NC(GPP_A22, NONE),						/* Does not exist */
	PAD_NC(GPP_A23, NONE),						/* Does not exist */

	/* ------- GPP_E ------- */
	PAD_NC(GPP_E00, NONE),
	PAD_NC(GPP_E01, NONE),                                          /* SOC_WL_OFF# */
	PAD_NC(GPP_E02, NONE),
	PAD_CFG_GPI(GPP_E03, NONE, DEEP),				/* TPM_PIRQ# */
	PAD_NC(GPP_E04, NONE),
	PAD_NC(GPP_E05, NONE),						/* TS_INT# (Unused) */
	PAD_NC(GPP_E06, NONE),						/* GPP_E6_STRAP (JTAG ODT Enable) */
	PAD_NC(GPP_E07, NONE),
	PAD_NC(GPP_E08, NONE),
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),				/* USB_OC0# / CCG6_PD2_OC3# */
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E13, NONE),
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),				/* EDP_HPD / SOC_EDP_HPD# */
	PAD_NC(GPP_E15, NONE),
	PAD_CFG_NF(GPP_E16, NONE, DEEP, NF2),				/* VRALERT# connected to H_PROCHOT# */
	PAD_NC(GPP_E17, NONE),
	PAD_NC(GPP_E18, NONE),						/* Does not exist */
	PAD_NC(GPP_E19, NONE),						/* Does not exist */
	PAD_NC(GPP_E20, NONE),						/* Does not exist */
	PAD_NC(GPP_E21, NONE),						/* Does not exist */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF2),				/* DNX_FORCE_RELOAD (NP resistor RC1 to 1.8V+PRIM) */
	PAD_NC(GPP_E23, NONE),						/* Does not exist */

	/* ------- GPP_H - UART, I2C1 ------- */
	/* TODO: Maybe explicitly drive these low instead of relying in "weak internal PD 20K" */
	PAD_NC(GPP_H00, NONE),						/* GPP_H00_STRAP (eSPI flash sharing mode, no external pull) */
	PAD_NC(GPP_H01, NONE),						/* GPP_H01_STRAP (Flash descriptor recovery, no external pull) */
	PAD_NC(GPP_H02, NONE),						/* GPP_H02_STRAP (Reserved, no external pull) */
	PAD_NC(GPP_H03, NONE),						/* Does not exist */
	PAD_NC(GPP_H04, NONE),
	PAD_NC(GPP_H05, NONE),
	PAD_NC(GPP_H06, NONE),						/* I2C_3_SDA (Touchscreen, unused) */
	PAD_NC(GPP_H07, NONE),						/* I2C_3_SCL (Touchscreen, unused) */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),				/* UART0_RXD (BIOS, OS UART) */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),				/* UART0_TXD (BIOS, OS UART) */
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H12, NONE),						/* Does not exist */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),				/* CPU_C10_GATE# (unused) */
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_NC(GPP_H18, NONE),
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),				/* I2C1_SDA (EC HID) */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),				/* I2C1_SCL (EC HID) */
	PAD_NC(GPP_H23, NONE),						/* Does not exist */

	/* ------- GPP_F - CNVi ------- */
	PAD_CFG_NF(GPP_F00, NONE, DEEP, NF1),				/* CNV_BRI_DT */
	PAD_CFG_NF(GPP_F01, NONE, DEEP, NF1),				/* CNV_BRI_RSP */
	PAD_CFG_NF(GPP_F02, NONE, DEEP, NF1),				/* CNV_RGI_DT */
	PAD_CFG_NF(GPP_F03, NONE, DEEP, NF1),				/* CNV_RGI_RSP */
	PAD_CFG_NF(GPP_F04, NONE, DEEP, NF1),				/* CNV_RF_RESET# */
	PAD_CFG_NF(GPP_F05, NONE, DEEP, NF3),				/* MODEM_CLKREQ / CLKREQ_CNV_1P8 */
	PAD_NC(GPP_F06, NONE),
	PAD_NC(GPP_F07, NONE),
	PAD_NC(GPP_F08, NONE),
	PAD_NC(GPP_F09, NONE),
	PAD_NC(GPP_F10, NONE),
	PAD_NC(GPP_F11, NONE),
	PAD_NC(GPP_F12, NONE),
	PAD_NC(GPP_F13, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),						/* TS_EN (Touchscreen, unused) */
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_NC(GPP_F19, NONE),						/* GPP_F19_STRAP (Reserved) */
	PAD_NC(GPP_F20, NONE),						/* GPP_F20_STRAP# (SVID VR) */
	PAD_NC(GPP_F21, NONE),						/* GPP_F21_STRAP (Low: BSSB-LS CCD, High: Baltic Peak I3C CCD) */
	PAD_NC(GPP_F22, NONE),						/* Testpoint TP4 */
	PAD_NC(GPP_F23, NONE),						/* Testpoint TP5 */

	/* ------- GPP_S - DMIC ------- */
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S02, NONE),						/* DMIC_CLK_A0 - TODO: I don't think this is connected to the PCH but instead the Codec */
	PAD_NC(GPP_S03, NONE),						/* DMIC_DATA0 - TODO: I don't think this is connected to the PCH but instead the Codec */
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),

	/* ------- GPP_B - Platform, I2C5 ------- */
	PAD_CFG_GPI_INT(GPP_B00, NONE, DEEP, LEVEL),			/* SOC_TP_INT# (Touchpad) */
	PAD_NC(GPP_B01, NONE),
	PAD_NC(GPP_B02, NONE),
	PAD_NC(GPP_B03, NONE),
	PAD_NC(GPP_B04, NONE),						/* GPP_B04_STRAP# (No Reboot) */
	PAD_CFG_GPI_INT(GPP_B05, NONE, DEEP, LEVEL),			/* SOC_EC_INT# */
	PAD_CFG_GPO(GPP_B06, 1, DEEP),					/* RTD3_SSD_PLT_RST# */
	PAD_NC(GPP_B07, NONE),						/* TS_RESET (Touchscreen, unused) */
	PAD_NC(GPP_B08, NONE),
	PAD_NC(GPP_B09, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B11, NONE),
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),				/* SLP_S0# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),				/* PLTRST# (Default is F1, so reset type doesn't matter) */
	PAD_NC(GPP_B14, NONE),						/* TOP_SWAP_EN_STRAP */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),				/* USB_OC3# / CCG6_PD1_OC0# */
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),						/* RT_GPIO6_CTRL_CPU (To turn off the retimer) */
	PAD_CFG_GPO(GPP_B18, 1, DEEP),					/* SOC_BT_ON */
	PAD_CFG_GPO(GPP_B19, 0, DEEP),					/* SOC_DG_BB_FORCE_PWR (Force Retimer Power for firmware update) */
	PAD_CFG_NF(GPP_B20, NONE, DEEP, NF2),				/* I2C5A_SDA (Touchpad) */
	PAD_CFG_NF(GPP_B21, NONE, DEEP, NF2),				/* I2C5A_SCL (Touchpad) */
	PAD_NC(GPP_B22, NONE),
	PAD_NC(GPP_B23, NONE),

	/* ------- GPP_D - HDA ------- */
	PAD_NC(GPP_D00, NONE),
	PAD_NC(GPP_D01, NONE),
	PAD_NC(GPP_D02, NONE),
	PAD_NC(GPP_D03, NONE),
	PAD_NC(GPP_D04, NONE),
	PAD_NC(GPP_D05, NONE),
	PAD_CFG_GPI_APIC(GPP_D06, NONE, DEEP, LEVEL, INVERT),		/* SOC_EC_INT2# */
	PAD_NC(GPP_D07, NONE),
	PAD_NC(GPP_D08, NONE),						/* SML0B_ALERT# (TODO: This may need to be NF. It just has a pullup to +1.8V_PRIM) */
	PAD_NC(GPP_D09, NONE),						/* Does not exist */
	PAD_CFG_NF(GPP_D10, NATIVE, DEEP, NF1),				/* HDA_BCLK */
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),				/* HDA_SYNC */
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),				/* HDA_SDO */
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),				/* HDA_SDI0 */
	/* TODO: Check if these are required */
	PAD_NC(GPP_D14, NONE),						/* WLAN_PCM_CLK */
	PAD_NC(GPP_D15, NONE),						/* WLAN_PCM_FRM */
	PAD_NC(GPP_D16, NONE),						/* WLAN_PCM_IN  */
	PAD_NC(GPP_D17, NONE),						/* WLAN_PCM_OUT  */
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_D20, NONE),
	PAD_NC(GPP_D21, NONE),
	PAD_NC(GPP_D22, NONE),
	PAD_NC(GPP_D23, NONE),

	/* ------- VGPIO3 - USB Type-C mux control ------- */
	PAD_CFG_NF(GPP_VGPIO3_USB0, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB1, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB2, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB3, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB4, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB5, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB6, NONE, DEEP, NF1),
	PAD_CFG_NF(GPP_VGPIO3_USB7, NONE, DEEP, NF1),
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
