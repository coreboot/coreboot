/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * GPIO configuration from vendor firmware dump (inteltool -G).
 *
 * Manually double-checked every pad name based on 3/16 schematic (Daniel Schaefer)
 */

#include <gpio.h>
#include <mainboard/ramstage.h>

static const struct pad_config gpio_table[] = {
	/* ------- GPP_V - Power Management ------- */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),				/* BATLOW#, 10k pull high to 1.8V_PRIM (unused?) */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),				/* AC_PRESENT, 10k pull high to 1.8V_PRIM, input from EC */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),				/* SOC_WAKE# / LAN_WAKE#, 10k pull high to 1.8V_PRIM (unused?) */
	PAD_CFG_NF(GPP_V03, UP_20K, DEEP, NF1),				/* PWRBTN# / PBTN_OUT#, 1k pull high to 1.8_PRIM, input from EC GPIO50 */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),				/* SLP_S3# */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),				/* SLP_S4# */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),				/* SLP_A# */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),				/* SUSCLK (suspend clock, out) */
	PAD_NC(GPP_V08, NONE),
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),				/* SLP_S5# */
	PAD_NC(GPP_V10, NONE),
	PAD_NC(GPP_V11, NONE),						/* SLP_LAN# NC, just TP18 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_V12, NONE, DEEP, NF1),		/* WAKE# / SOC_WL_WAKE# (1.8V level) */
	PAD_NC(GPP_V13, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_V14, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_V15, NONE),						/* Does not exist on schematic */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),				/* VCCST_EN (VCC sustain) */
	PAD_CFG_GPO(GPP_V17, 0, DEEP),					/* RT_GPIO6_CTRL_CPU (pulled high to 1.8VS via 10k and GND via 100k, to Retimer) */

	/* ------- GPP_C - SMBus, CLKREQs, TBT ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C00, NONE, DEEP, NF1),		/* SMBCLK (LPCAMM2) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C01, NONE, DEEP, NF1),		/* SMBDATA (LPCAMM2) */
	PAD_CFG_GPI(GPP_C02, NONE, DEEP),				/* TLS strap (active high, ext pull-up) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C03, NONE, DEEP, NF1),		/* SML0CLK (TBT) */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_C04, NONE, DEEP, NF1),		/* SML0DATA (TBT) */
	PAD_CFG_GPI(GPP_C05, NONE, DEEP),				/* eSPI strap (enable low, ext pull-down) */
	PAD_NC(GPP_C06, NONE),
	PAD_NC(GPP_C07, NONE),
	PAD_CFG_NF(GPP_C08, NONE, DEEP, NF1),				/* SML1_ALERT# (10k to 1.8V_PRIM) */
	PAD_NC(GPP_C09, NONE),
	PAD_CFG_NF(GPP_C10, NONE, DEEP, NF1),				/* SRCCLKREQ1# (SSD) */
	PAD_NC(GPP_C11, NONE),
	PAD_NC(GPP_C12, NONE),
	PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1),				/* SRCCLKREQ4# (WLAN) */
	PAD_NC(GPP_C14, NONE),
	PAD_CFG_GPI(GPP_C15, NONE, DEEP),				/* strap (must not drive, ext pull-down) */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),				/* TBT_LSX0_TXD */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),				/* TBT_LSX0_RXD */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),				/* TBT_LSX1_TXD */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),				/* TBT_LSX1_RXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),				/* TBT_LSX2_TXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),				/* TBT_LSX2_RXD */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),				/* TBT_LSX3_TXD */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),				/* TBT_LSX3_RXD */

	/* ------- GPP_F - CNVi ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),		/* CNV_BRI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),		/* CNV_BRI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),		/* CNV_RGI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),		/* CNV_RGI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),		/* CNV_RF_RESET# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),		/* CLKREQ_CNV */
	PAD_NC(GPP_F06, NONE),
	PAD_NC(GPP_F07, NONE),
	PAD_NC(GPP_F08, NONE),						/* RSVD_GPP_F08 (TP41) */
	PAD_NC(GPP_F09, NONE),
	PAD_NC(GPP_F10, NONE),
	PAD_NC(GPP_F11, NONE),
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF8),				/* SOC_TP_I2C_1_SCL (Touchpad) */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF8),				/* SOC_TP_I2C_1_SDA (Touchpad) */
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_CFG_GPI_INT(GPP_F18, NONE, DEEP, LEVEL),			/* SOC_TP_I2C_1_INT# (Touchpad, GpioInt) */
	PAD_NC(GPP_F19, NONE),						/* GPP_F19_STRAP (1.8V_PRIM) */
	PAD_NC(GPP_F20, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	/* ------- GPP_E ------- */
	PAD_NC(GPP_E00, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_E01, NONE),
	PAD_CFG_NF(GPP_E02, NONE, DEEP, NF2),				/* VRALERT# / H_PROCHOT# (2.2k to 1.8V_PRIM) */
	PAD_CFG_GPO(GPP_E03, 1, DEEP),					/* RTD3_SSD_PLT_RST# */
	PAD_NC(GPP_E04, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_E05, NONE),
	PAD_NC(GPP_E06, NONE),						/* GPP_E06_STRAP (floating) */
	PAD_NC(GPP_E07, NONE),
	PAD_NC(GPP_E08, NONE),
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),				/* USB_OC0# (input from PD) */
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),				/* SOC_TS_I2C_0_CLK (THC0 I2C CLK Touchscreen) */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),				/* SOC_TS_I2C_0_SDA (THC0 I2C SDA Touchscreen) */
	PAD_NC(GPP_E14, NONE),
	PAD_NC(GPP_E15, NONE),
	PAD_CFG_GPO(GPP_E16, 1, DEEP),					/* SOC_TS_0_RST (TS_RST# Touch screen reset, active low, 10k to 1.8VS) */
	PAD_NC(GPP_E17, NONE),
	PAD_CFG_GPI_INT(GPP_E18, NONE, DEEP, LEVEL),			/* SOC_TS_I2C_0_INT# (touchscreen HID interrupt, GpioInt) */
	PAD_NC(GPP_E19, NONE),
	PAD_NC(GPP_E20, NONE),
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),				/* SOC_PD_INT#_R / PMCALERT# */
	PAD_NC(GPP_E22, NONE),

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
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),				/* UART0_RXD (CPU/PCH UART console) */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),				/* UART0_TXD (CPU/PCH UART console) */
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	PAD_NC(GPP_H12, NONE),						/* Does not exist on schematic */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),				/* CPU_C10_GATE# (10k to 1.8V_PRIM) */
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	PAD_NC(GPP_H18, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_H19, NONE),
	PAD_NC(GPP_H20, NONE),
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),				/* SOC_I2C_1_SDA (I2C1_SDA EC HID) */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),				/* SOC_I2C_1_SCL (I2C1_SCL EC HID) */

	/* ------- GPP_A - eSPI ------- */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A00, UP_20K, DEEP, NF1),	/* ESPI_IO0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A01, UP_20K, DEEP, NF1),	/* ESPI_IO1 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A02, UP_20K, DEEP, NF1),	/* ESPI_IO2 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A03, UP_20K, DEEP, NF1),	/* ESPI_IO3 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A04, UP_20K, DEEP, NF1),	/* ESPI_CS0# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A05, UP_20K, DEEP, NF1),	/* ESPI_CLK */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_A06, NONE, DEEP, NF1),		/* ESPI_RESET# */
	PAD_NC(GPP_A07, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_A08, NONE),
	PAD_NC(GPP_A09, NONE),
	PAD_NC(GPP_A10, NONE),
	PAD_CFG_GPO(GPP_A11, 1, DEEP),					/* RTD3_WLAN_PLT_RST# (10k to 1.8VS) */
	PAD_NC(GPP_A12, NONE),
	PAD_NC(GPP_A13, NONE),
	PAD_NC(GPP_A14, NONE),						/* Does not exist on schematic */
	PAD_NC(GPP_A15, NONE),						/* EPD_ON_GCD_OUT (Connected to EPD_ON_GCD_IN) */
	PAD_CFG_GPO(GPP_A16, 1, DEEP),					/* SOC_BT_ON (10k to 1.8V_PRIM) */
	PAD_CFG_GPO(GPP_A17, 1, DEEP),					/* SOC_WL_OFF# (10k to 1.8V_PRIM) */

	/* ------- GPP_S - SoundWire / DMIC ------- */
	/* TODO: Need to check if this is needed, I think didn't stuff these and DMIC is connected to HDA codec only */
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S02, NONE),						/* PCH_DMIC_CLK_R (BT Offload) */
	PAD_NC(GPP_S03, NONE),						/* PCH_DMIC_DATA_R (BT Offload) */
	PAD_NC(GPP_S04, NONE),						/* WLAN_PCM_CLK_R (BT Offload) */
	PAD_NC(GPP_S05, NONE),						/* WLAN_PCM_FRM (BT Offload) */
	PAD_NC(GPP_S06, NONE),						/* WLAN_PCM_OUT_R (BT Offload) */
	PAD_NC(GPP_S07, NONE),						/* WLAN_PCM_IN_R (BT Offload) */

	/* ------- GPP_B - Platform ------- */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),				/* SOC_USBC_SMLCLK (2.2k to 1.8V_PRIM, PD 1/2) */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),				/* SOC_USBC_SMLDATA (2.2k to 1.8V_PRIM, PD 1/2) */
	PAD_NC(GPP_B02, NONE),
	PAD_NC(GPP_B03, NONE),
	PAD_NC(GPP_B04, NONE),						/* ME_EN (high = override flash descriptor security, input from EC) */
	PAD_CFG_GPI_INT(GPP_B05, NONE, DEEP, LEVEL),			/* SOC_EC_INT1# (EC keyboard interrupt, GpioInt) */
	PAD_CFG_GPO(GPP_B06, 0, DEEP),					/* LPCAMM_ERROR (LED) TODO: This should be triggered if memory training failed. */
	PAD_NC(GPP_B07, NONE),
	PAD_NC(GPP_B08, NONE),
	PAD_NC(GPP_B09, NONE),
	PAD_NC(GPP_B10, NONE),
	PAD_NC(GPP_B11, NONE),
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),				/* PM_SLP_S0# / SLP_S0# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),				/* SOC_PLTRST# / PLTRST# */
	PAD_NC(GPP_B14, NONE),						/* TOP_SW AP_EN_STRAP (Inverts an address on access to SPI, so the alternate boot block is fetched instead of the original boot-block) */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),				/* PD2_OC3# / USB_OC3# */
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_CFG_GPO(GPP_B18, 1, DEEP),					/* SOC_TS_0_EN (touch screen enable) */
	PAD_NC(GPP_B19, NONE),
	PAD_NC(GPP_B20, NONE),
	PAD_CFG_GPO(GPP_B21, 0, DEEP),					/* SOC_RT_FORCE_PW (retimer power) TODO: Triggered for retimer firmware update */
	PAD_NC(GPP_B22, NONE),
	PAD_NC(GPP_B23, NONE),						/* GPP_B23_STRAP (Reserved, 1.8V_PRIM) */
	PAD_NC(GPP_B24, NONE),
	PAD_NC(GPP_B25, NONE),

	/* ------- GPP_D - HDA, EC sensors ------- */
	PAD_NC(GPP_D00, NONE),
	PAD_NC(GPP_D01, NONE),
	PAD_NC(GPP_D02, NONE),
	PAD_NC(GPP_D03, NONE),
	PAD_NC(GPP_D04, NONE),
	PAD_CFG_GPO(GPP_D05, 1, DEEP),					/* SOC_SSD_PWR_EN (active high, 100k pull-down) */
	PAD_CFG_GPI_APIC(GPP_D06, NONE, DEEP, LEVEL, INVERT),		/* SOC_EC_INT2# (EC ALS sensor interrupt) */
	PAD_NC(GPP_D07, NONE),
	PAD_NC(GPP_D08, NONE),
	PAD_NC(GPP_D09, NONE),
	PAD_CFG_NF(GPP_D10, NATIVE, DEEP, NF1),				/* HDA_BCLK */
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),				/* HDA_SYNC */
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),				/* HDA_SDO / HDA_SDOUT / GPP_D12_STRAP (1 = no reboot, processor disabled TCO timer system reboot) */
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),				/* HDA_SDI0 / HDA_SDIN0 */
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
	PAD_NC(GPP_D18, NONE),
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_D20, NONE),
	PAD_NC(GPP_D21, NONE),
	PAD_NC(GPP_D22, NONE),
	PAD_NC(GPP_D23, NONE),
	PAD_NC(GPP_D24, NONE),
	PAD_NC(GPP_D25, NONE),
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
