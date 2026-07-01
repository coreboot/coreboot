/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/ramstage.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPP_A - eSPI ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A00, UP_20K, DEEP, NF1),	/* ESPI_IO0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A01, UP_20K, DEEP, NF1),	/* ESPI_IO1 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A02, UP_20K, DEEP, NF1),	/* ESPI_IO2 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A03, UP_20K, DEEP, NF1),	/* ESPI_IO3 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A04, UP_20K, DEEP, NF1),	/* ESPI_CS0# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A05, UP_20K, DEEP, NF1),	/* ESPI_CLK */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A06, NONE, DEEP, NF1),		/* ESPI_RESET# */
	/* Asserted in bootblock (gpio_early.c); released here after memory training. */
	PAD_CFG_GPO(GPP_A08, 1, DEEP),					/* RTD3_SSD_PLT_RST# */
	PAD_CFG_GPI_INT(GPP_A09, NONE, DEEP, LEVEL),			/* EC I2C HID mediakeys interrupt (SOC_EC_INT#) */
	PAD_NC(GPP_A10, NONE),						/* SOC_EC_INT2# (unused, no EC ALS on dahlia) */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),					/* RTD3_WLAN_PLT_RST# */

	/* ------- GPP_B - Platform ------- */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),				/* SOC_USBC_SMLCLK (to PD) */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),				/* SOC_USBC_SMLDATA (to PD) */
	PAD_NC(GPP_B02, NONE),						/* G-sensor ISH (unpopulated, reserved) */
	PAD_NC(GPP_B03, NONE),						/* G-sensor ISH (unpopulated, reserved) */
	PAD_NC(GPP_B04, NONE),						/* ME_EN (high = override flash descriptor security, input from EC) */
	PAD_NC(GPP_B05, NONE),
	PAD_NC(GPP_B06, NONE),
	PAD_NC(GPP_B07, NONE),
	PAD_NC(GPP_B08, NONE),
	PAD_NC(GPP_B09, NONE),						/* SOC_USBC0_HPD# (unused: PD-managed, notified via I2C; native DDSP_HPD0# would be NF2) */
	PAD_NC(GPP_B10, NONE),						/* SOC_USBC1_HPD# (unused: PD-managed, notified via I2C; native DDSP_HPD1# would be NF2) */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),				/* PM_SLP_S0# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),				/* SOC_PLTRST# */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),				/* PD2_OC1# (USB_OC1# is NF1 on WCL, unlike PTL) */
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B18, NONE),
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_CFG_GPO(GPP_B21, 0, DEEP),					/* SOC_RT_FORCE_PWR (retimer force power) */
	PAD_NC(GPP_B22, NONE),
	PAD_NC(GPP_B23, NONE),						/* SOC_GPP_B23 (Reserved, floating strap) */
	PAD_NC(GPP_B24, NONE),
	PAD_NC(GPP_B25, NONE),

	/* ------- GPP_C - SMBus, CLKREQs, TBT ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C00, NONE, DEEP, NF1),		/* SMBCLK (SODIMM) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C01, NONE, DEEP, NF1),		/* SMBDATA (SODIMM) */
	PAD_NC(GPP_C02, NONE),						/* TLS strap (active high, ext pull-up) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C03, NONE, DEEP, NF1),		/* SML0CLK (TBT) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C04, NONE, DEEP, NF1),		/* SML0DATA (TBT) */
	PAD_NC(GPP_C05, NONE),						/* eSPI strap (enable low, ext pull-down) */
	PAD_NC(GPP_C06, NONE),
	PAD_NC(GPP_C07, NONE),
	PAD_NC(GPP_C08, NONE),
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1),				/* SRCCLKREQ0# / CLKREQ_PCIE#0 (WiFi) */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),					/* SOC_WL_OFF# (WiFi disable) */
	PAD_NC(GPP_C11, NONE),
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),				/* SRCCLKREQ3# / CLKREQ_PCIE#3 (SSD) */
	PAD_NC(GPP_C13, NONE),
	PAD_NC(GPP_C14, NONE),
	PAD_NC(GPP_C15, NONE),						/* strap (must not drive, ext pull-down) */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),				/* TBT_LSX0_TXD */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),				/* TBT_LSX0_RXD */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),				/* TBT_LSX1_TXD */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),				/* TBT_LSX1_RXD */
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	/* ------- GPP_D - HDA, EC sensors ------- */
	PAD_NC(GPP_D00, NONE),
	PAD_NC(GPP_D01, NONE),
	PAD_NC(GPP_D02, NONE),
	PAD_NC(GPP_D03, NONE),
	PAD_NC(GPP_D05, NONE),
	PAD_NC(GPP_D06, NONE),
	PAD_NC(GPP_D07, NONE),						/* SOC_ALS_INT# (unused, no ALS sensor) */
	PAD_NC(GPP_D08, NONE),
	PAD_NC(GPP_D09, NONE),
	PAD_CFG_NF(GPP_D10, NATIVE, DEEP, NF1),				/* HDA_BCLK */
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),				/* HDA_SYNC */
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),				/* HDA_SDO / HDA_SDOUT / GPP_D12_STRAP (1 = no reboot, processor disabled TCO timer system reboot) */
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),				/* HDA_SDI0 / HDA_SDIN0 */
	PAD_NC(GPP_D16, NONE),						/* SOC_DMIC_CLK1 NC */
	PAD_NC(GPP_D17, NONE),						/* SOC_DMIC_DATA1 NC */
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_D21, NONE),

	/* ------- GPP_E ------- */
	PAD_NC(GPP_E01, NONE),
	PAD_CFG_NF(GPP_E02, NONE, DEEP, NF2),				/* VR_ALERT#_R / H_PROCHOT# */
	PAD_NC(GPP_E03, NONE),
	PAD_NC(GPP_E05, NONE),						/* EC_SCI#_R (NC) */
	PAD_NC(GPP_E06, NONE),						/* GPP_E06_STRAP (JTAG ODT Enable, NC) */
	PAD_NC(GPP_E07, NONE),
	PAD_CFG_GPO(GPP_E08, 1, DEEP),					/* SOC_BT_ON */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),				/* PD1_OC0# */
	PAD_NC(GPP_E10, NONE),						/* RT_GPIO6_CTRL_CPU (NC, controlled by EC) */
	PAD_NC(GPP_E11, NONE),
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),				/* SOC_TS_I2C_0_CLK (THC0/I2C CLK Touchscreen) */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),				/* SOC_TS_I2C_0_SDA (THC0/I2C SDA Touchscreen) */
	/* Already enabled in bootblock (gpio_early.c); kept on here. */
	PAD_CFG_GPO(GPP_E14, 1, PLTRST),				/* TCHSCR_PWR_EN (Touchscreen) */
	PAD_NC(GPP_E15, NONE),
	/* Asserted in bootblock (gpio_early.c); released here after memory training. */
	PAD_CFG_GPO(GPP_E16, 1, PLTRST),				/* SOC_TCHSCR_RST# (Touchscreen) */
	PAD_NC(GPP_E17, NONE),
	PAD_CFG_GPI_INT(GPP_E18, UP_5K, DEEP, LEVEL),			/* SOC_TCHSCR_INT# (touchscreen) */
	PAD_CFG_GPI_INT(GPP_E19, UP_20K, PLTRST, EDGE_BOTH),		/* Tablet-mode notify from EC (Level-shifted to 1.8V) */
	PAD_NC(GPP_E20, NONE),
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),				/* SOC_I2C_PD_INT# / PMCALERT# */
	PAD_NC(GPP_E22, NONE),						/* Testpoint TP1447 */

	/* ------- GPP_F - CNVi ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),		/* CNV_BRI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),		/* CNV_BRI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),		/* CNV_RGI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),		/* CNV_RGI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),		/* CNV_RF_RESET# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),		/* CLKREQ_CNV */
	PAD_NC(GPP_F06, NONE),
	PAD_NC(GPP_F07, NONE),
	PAD_NC(GPP_F08, NONE),
	PAD_NC(GPP_F09, NONE),
	/* Already enabled in bootblock (gpio_early.c); kept on here. */
	PAD_CFG_GPO(GPP_F10, 1, DEEP),					/* SOC_TCHSCR_EN (touchscreen enable, GPIO via eDP connector) */
	PAD_CFG_GPI_INT(GPP_F11, UP_20K, PLTRST, EDGE_BOTH),		/* EC_PAD_MODE - tablet-mode notify from EC (open-drain): high=tablet, low=clamshell. OS reads it via INT33D3/PNP0C60 (see tablet.asl). Pull-up needed for the open-drain high state */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF8),				/* SOC_THC_I2C_1_SCL (Touchpad) */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF8),				/* SOC_THC_I2C_1_SDA (Touchpad) */
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_CFG_GPI_INT(GPP_F18, UP_5K, DEEP, LEVEL),
	PAD_NC(GPP_F19, NONE),						/* SOC_GPP_F19_STRAP (NC) */
	PAD_NC(GPP_F20, NONE),						/* SOC_ACC_MU_INT# (unpopulated) */
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),						/* IPCM_PRSNT (10k pulldown to GND, MCHP power monitor present) */

	/* ------- GPP_H - UART, I2C ------- */
	/* MAFS = Host attached Flash Sharing, SAFS = Device Attached Flash Sharing (SAFS) */
	PAD_NC(GPP_H00, NONE),						/* GPP_H00_STRAP (internal pulldown, low = MAFS, high = SAFS) */
	PAD_NC(GPP_H01, NONE),						/* GPP_H01_STRAP (internal pulldown, high = flash descriptor recovery) */
	PAD_NC(GPP_H02, NONE),						/* GPP_H02_STRAP (reserved)*/
	PAD_NC(GPP_H03, NONE),
	PAD_NC(GPP_H04, NONE),
	PAD_NC(GPP_H05, NONE),
	PAD_CFG_NF(GPP_H06, NONE, DEEP, NF1),				/* I2C3_SDA (PAC1954 Power Monitor, NP in MP, 4.7k to 1.8VS) */
	PAD_CFG_NF(GPP_H07, NONE, DEEP, NF1),				/* I2C3_SCL (PAC1954 Power Monitor, NP in MP, 4.7k to 1.8VS) */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),				/* UART0_RXD (console) */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),				/* UART0_TXD (console) */
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H17, NONE),
	/* Already enabled in bootblock (gpio_early.c); kept on here. */
	PAD_CFG_GPO(GPP_H18, 1, DEEP),					/* SOC_SSD_EN */
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),				/* SOC_I2C_1_SDA (EC HID) */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),				/* SOC_I2C_1_SCL (EC HID) */

	/* ------- GPP_V - Power Management ------- */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),				/* BATLOW#, 100k pull high to 1.8V_PRIM (unused?) */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),				/* AC_PRESENT, 10k pull high to 1.8V_PRIM, input from EC */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),				/* SOC_WAKE# (10k pull high to 1.8V_PRIM unused?) */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),				/* PM_SLP_S3# */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),				/* PM_SLP_S4# */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),				/* PM_SLP_A# */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),				/* SUSCLK */
	PAD_NC(GPP_V08, NONE),						/* GPP_V08 is RSVD on WCL (no SLP_WLAN#, has 100k pulldown on MB) */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),				/* PM_SLP_S5# */
	PAD_NC(GPP_V10, NONE),
	PAD_NC(GPP_V11, NONE),						/* PM_SLP_LAN# 100k pulldown */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_V12, NONE, DEEP, NF1),		/* SOC_WL_WAKE# */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),				/* SOC_VCCST_EN */
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
