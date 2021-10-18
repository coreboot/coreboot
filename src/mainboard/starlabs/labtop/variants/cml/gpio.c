/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/*
 * All definitions are taken from a comparison of the output of "inteltool -a"
 * using the stock BIOS and with coreboot.
 */

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* C20:	 UART2_RXD			*/
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* C21:	 UART2_TXD			*/
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* E22:	BRD_ID0				*/
	PAD_CFG_GPO(GPP_E22, 1, PLTRST),
	/* E23:	BRD_ID1				*/
	PAD_CFG_GPO(GPP_E23, 1, PLTRST),
	/* H6:	BRD_ID2				*/
	PAD_CFG_GPI(GPP_H6, NONE, PLTRST),
	/* H7:	BRD_ID3				*/
	PAD_CFG_GPI(GPP_H7, NONE, PLTRST),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
const struct pad_config gpio_table[] = {
	/* REFERENCE:	EP PER SCHEMATIC	*/

	/* GPD0:	PCH_BATLOW#		*/
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
	/* GPD1:	AC_PRESENT		*/
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),
	/* GPD2:	LAN_WAKE#		*/
	PAD_NC(GPD2, NONE),
	/* GPD3:	SIO_PWRBTN#		*/
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),
	/* GPD4:	SIO_SLP_S3#		*/
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
	/* GPD5:	SIO_SLP_S4#		*/
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
	/* GPD6:	SIO_SLP_A#		*/
	PAD_CFG_NF(GPD6, NONE, DEEP, NF1),
	/* GPD7:	PCH_TBT_PERST#		*/
	PAD_CFG_GPO(GPD7, 0, PLTRST),
	/* GPD8:	SUSCLK			*/
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
	/* GPD9:	SIO_SLP_WLAN#		*/
	PAD_CFG_NF(GPD9, NONE, DEEP, NF1),
	/* GPD10:	SIO_SLP_S5#		*/
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1),
	/* GPD11:	PM_LANPHY_EN		*/
	PAD_CFG_NF(GPD11, NONE, DEEP, NF1),

	/* A0:	KBRST_N			*/
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),
	/* A1:	LPC_AD0				*/
	PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),
	/* A2:	LPC_AD1				*/
	PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),
	/* A3:	LPC_AD2				*/
	PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),
	/* A4:	LPC_AD3				*/
	PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),
	/* A5:	LPC_FRAME_N			*/
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),
	/* A6:	LPC_SERIRQ			*/
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),
	/* A7:	Not Connected			*/
	PAD_NC(GPP_A7, NONE),
	/* A8:	 GPPC_A8_CLKRUN_N		*/
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),
	/* A9:	 ESPI_CLK			*/
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),
	/* A10:	Not Connected			*/
	PAD_NC(GPP_A10, NONE),
	/* A11:	Not Connected			*/
	PAD_NC(GPP_A11, NONE),
	/* A12:	 TABLET_MODE_CTRL		*/
	PAD_NC(GPP_A12, NONE),
	/* A13:	 SUSPWRDNACK			*/
	PAD_CFG_GPO(GPP_A13, 1, PLTRST),
	/* A14:	 PM_SUS_STAT_N			*/
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* A15:	 SPK_PD_N			*/
	PAD_CFG_GPO(GPP_A15, 1, PLTRST),
	/* A16:	Not Connected			*/
	PAD_NC(GPP_A16, NONE),
	/* A17:	 SAR_DPR_PCH			*/
	PAD_NC(GPP_A17, NONE),
	/* A18:	 ACCEL1_INT1			*/
	PAD_NC(GPP_A18, UP_20K),
	/* A19:	 ACCEL2_INT1			*/
	PAD_NC(GPP_A19, UP_20K),
	/* A20:	 HUMAN_PRESENCE_INT_N		*/
	PAD_NC(GPP_A20, UP_20K),
	/* A21:	 HALL_SENSOR_INT		*/
	PAD_NC(GPP_A21, UP_20K),
	/* A22:	 SAR_NIRQ_PCH			*/
	PAD_NC(GPP_A22, UP_20K),
	/* A23:	 INT_SHARED			*/
	PAD_NC(GPP_A23, UP_20K),

	/* B0:	Not Connected			*/
	PAD_NC(GPP_B0, NONE),
	/* B1:	Not Connected			*/
	PAD_NC(GPP_B1, NONE),
	/* B2:	Not Connected			*/
	PAD_NC(GPP_B2, NONE),
	/* B3:	CLICK_PAD_INT_R_N		*/
	PAD_CFG_GPI_APIC_LOW(GPP_B3, NONE, PLTRST),
	/* B4:	BT_RF_KILL_N			*/
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* B5:	 WLAN_CLKREQ#			*/
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
	/* B6:	 CLKREQ1_SSD_N			*/
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),
	/* B7:	 LAN_CLKREQ#			*/
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF1),
	/* B8:	Not Connected			*/
	PAD_NC(GPP_B8, NONE),
	/* B9:	Not Connected			*/
	PAD_NC(GPP_B9, NONE),
	/* B10:	Not Connected			*/
	PAD_NC(GPP_B10, NONE),
	/* B11:	 EXT_PWR_GATE_N			*/
	PAD_CFG_GPO(GPP_B11, 1, PLTRST),
	/* B12:	 PM_SLP_S0_N			*/
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* B13:	 PLT_RST_N			*/
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* B14:	 TCH_PNL_PWR_EN			*/
	PAD_CFG_GPO(GPP_B14, 1, PLTRST),
	/* B15:	Not Connected			*/
	PAD_NC(GPP_B15, NONE),
	/* B16:	 FPS_INT			*/
	PAD_CFG_GPI_APIC(GPP_B16, NONE, PLTRST, LEVEL, NONE),
	/* B17:	 FPS_RST_N			*/
	PAD_CFG_GPO(GPP_B17, 1, PLTRST),
	/* B18:	 AR_CIO_PWR_EN			*/
	PAD_CFG_GPO(GPP_B18, 0, DEEP),
	/* B19:	 GSPI1_CS0_FPS_N		*/
	PAD_NC(GPP_B19, NONE),
	/* B20:	 GSPI1_CLK_FPS			*/
	PAD_NC(GPP_B20, NONE),
	/* B21:	 GSPI1_MISO_FPS			*/
	PAD_NC(GPP_B21, NONE),
	/* B22:	 GSPI1_MOSI_FPS			*/
	PAD_CFG_GPO(GPP_B22, 0, DEEP),
	/* B23:	 EC_SLP_S0IX_N			*/
	PAD_NC(GPP_B23, NONE),

	/* C0:	 SMB_CLK			*/
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* C1:	 SMB_DATA			*/
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* C2:	 WIFI_RF_KILL_N			*/
	PAD_CFG_GPO(GPP_C2, 1, DEEP),
	/* C3:	Not Connected			*/
	PAD_NC(GPP_C3, NONE),
	/* C4:	Not Connected			*/
	PAD_NC(GPP_C4, NONE),
	/* C5:	 SML0ALERT			*/
	PAD_CFG_GPO(GPP_C5, 0, DEEP),
	/* C6:	Not Connected			*/
	PAD_NC(GPP_C6, NONE),
	/* C7:	Not Connected			*/
	PAD_NC(GPP_C7, NONE),
	/* C8:	 CODEC_INT_N			*/
	PAD_CFG_GPI_APIC_LOW(GPP_C8, UP_20K, PLTRST),
	/* C9:	Not Connected			*/
	PAD_NC(GPP_C9, NONE),
	/* C10:	Not Connected			*/
	PAD_NC(GPP_C10, NONE),
	/* C11:	Not Connected			*/
	PAD_NC(GPP_C11, NONE),
	/* C12:	 PCIE_NAND_RST_R_N		*/
	PAD_CFG_GPO(GPP_C12, 1, PLTRST),
	/* C13:	 M2_SSD_PWREN			*/
	PAD_NC(GPP_C13, NONE),
	/* C14:	 TBT_WAKE_MUX_SEL_N		*/
	PAD_NC(GPP_C14, NONE),
	/* C15:	 TBT_RST_N			*/
	PAD_NC(GPP_C15, NONE),
	/* C16:	 I2C0_SDA			*/
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* C17:	 I2C0_SCL			*/
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* C18:	 TOUCH_I2C_SDA			*/
	PAD_NC(GPP_C18, NONE),
	/* C19:	 TOUCH_I2C_SDL			*/
	PAD_NC(GPP_C19, NONE),
	/* C22:	 AR1_USB_PWR_EN			*/
	PAD_NC(GPP_C22, NONE),
	/* C23:	Not Connected			*/
	PAD_NC(GPP_C23, NONE),

	/* D0:	 TPM_IRQ			*/
	PAD_NC(GPP_D0, NONE),
	/* D1:	Not Connected			*/
	PAD_NC(GPP_D1, NONE),
	/* D2:	Not Connected			*/
	PAD_NC(GPP_D2, NONE),
	/* D3:	Not Connected			*/
	PAD_NC(GPP_D3, NONE),
	/* D4:	Not Connected			*/
	PAD_NC(GPP_D4, NONE),
	/* D5:	Not Connected			*/
	PAD_NC(GPP_D5, NONE),
	/* D6:	Not Connected			*/
	PAD_NC(GPP_D6, NONE),
	/* D7:	 ISH_I2C1_SDA			*/
	PAD_NC(GPP_D7, NONE),
	/* D8:	 ISH_I2C1_SDL			*/
	PAD_NC(GPP_D8, NONE),
	/* D9:	 TCH_PNL2_RST_R_N		*/
	PAD_CFG_GPO(GPP_D9, 1, PLTRST),
	/* D10:	 TCH_PNL2_INT_R_N		*/
	PAD_NC(GPP_D10, NATIVE),
	/* D11:	Not Connected			*/
	PAD_NC(GPP_D11, NATIVE),
	/* D12:	 GPPC_D_12			*/
	PAD_CFG_GPO(GPP_D12, 0, DEEP),
	/* D13:	 WWAN_FCP_OFF_N			*/
	PAD_NC(GPP_D13, NONE),
	/* D14:	 TCH_PNL1_RST_N			*/
	PAD_CFG_GPO(GPP_D14, 1, PLTRST),
	/* D15:	Not Connected			*/
	PAD_NC(GPP_D15, NONE),
	/* D16:	 GPIO_2_EC			*/
	PAD_CFG_GPO(GPP_D16, 0, PWROK),
	/* D17:	 DMIC_CLK1_SNDW3_CLK		*/
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* D18:	 DMIC_DATA1_SNDW3_DATA		*/
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	/* D19:	 DMIC_CLK_0			*/
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
	/* D20:	 DMIC_DATA_0			*/
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	/* D21:	Not Connected			*/
	PAD_NC(GPP_D21, NONE),
	/* D22:	 TPM_RST			*/
	PAD_NC(GPP_D22, NONE),
	/* D23:	 TPM_IRQ			*/
	PAD_NC(GPP_D23, NONE),

	/* E0:	Not Connected			*/
	PAD_NC(GPP_E0, NONE),
	/* E1:	 GPPC_E1_SATAXPCIE_1_SATAGP_1	*/
	PAD_CFG_NF(GPP_E1, UP_20K, DEEP, NF1),
	/* E2:	Not Connected			*/
	PAD_NC(GPP_E2, NONE),
	/* E3:	Not Connected			*/
	PAD_NC(GPP_E3, NONE),
	/* E4:	Not Connected			*/
	PAD_NC(GPP_E4, NONE),
	/* E5:	 GPPC_E5_SATA_DEVSLP_1		*/
	PAD_NC(GPP_E5, NONE),
	/* E6:	Not Connected			*/
	PAD_NC(GPP_E6, NONE),
	/* E7:	Not Connected			*/
	PAD_CFG_GPI(GPP_E7, NONE, PLTRST),
	/* E8:	Not Connected			*/
	PAD_NC(GPP_E8, NONE),
	/* E9:	 GPPC_E9_USB2_OCB_0		*/
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* E10:	 GPPC_E10_USB2_OCB_1		*/
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),
	/* E11:	 USB2_P3_WP2_OC_N		*/
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),
	/* E12:	 GPPC_E12_USB2_OCB_3		*/
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* E13:	 DDI1_HPD			*/
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* E14:	 DDI2_HPD			*/
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15:	 SMC_SMI_N			*/
	PAD_CFG_GPI_SMI_LOW(GPP_E15, NONE, DEEP, EDGE_SINGLE),
	/* E16:	 SMC_SCI_N			*/
	PAD_CFG_GPI_SCI_LOW(GPP_E16, NONE, PLTRST, LEVEL),
	/* E17:	 EDP_HPD			*/
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF1),
	/* E18:	 DDI1_DDC_SCL			*/
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),
	/* E19:	 DDI1_DDC_SDA			*/
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),
	/* E20:	 DDI2_CTRL_CLK			*/
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
	/* E21:	 DDI2_CTRL_DATA			*/
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),

	/* F0:	Not Connected			*/
	PAD_NC(GPP_F0, NONE),
	/* F1:	Not Connected			*/
	PAD_NC(GPP_F1, NONE),
	/* F2:	Not Connected			*/
	PAD_NC(GPP_F2, NONE),
	/* F3:	Not Connected			*/
	PAD_NC(GPP_F3, NONE),
	/* F4:	CNV_BRI_DT_R			*/
	PAD_CFG_NF(GPP_F4, UP_20K, DEEP, NF1),
	/* F5:	CNV_BRI_RSP			*/
	PAD_CFG_NF(GPP_F5, UP_20K, DEEP, NF1),
	/* F6:	CNV_RGI_DT_R			*/
	PAD_CFG_NF(GPP_F6, UP_20K, DEEP, NF1),
	/* F7:	CNV_RGI_RSP			*/
	PAD_CFG_NF(GPP_F7, UP_20K, DEEP, NF1),
	/* F8:	Not Connected			*/
	PAD_NC(GPP_F8, NONE),
	/* F9:	Not Connected			*/
	PAD_NC(GPP_F9, NONE),
	/* F10:	Not Connected			*/
	PAD_NC(GPP_F10, NONE),
	/* F11:	EMMC_CMD			*/
	PAD_NC(GPP_F11, NONE),
	/* F12:	EMMC_DATA_0			*/
	PAD_NC(GPP_F12, NONE),
	/* F13:	EMMC_DATA_1			*/
	PAD_NC(GPP_F13, NONE),
	/* F14:	EMMC_DATA_2			*/
	PAD_NC(GPP_F14, NONE),
	/* F15:	EMMC_DATA_3			*/
	PAD_NC(GPP_F15, NONE),
	/* F16:	EMMC_DATA_4			*/
	PAD_NC(GPP_F16, NONE),
	/* F17:	EMMC_DATA_5			*/
	PAD_NC(GPP_F17, NONE),
	/* F18:	EMMC_DATA_6			*/
	PAD_NC(GPP_F18, NONE),
	/* F19:	EMMC_DATA_7			*/
	PAD_NC(GPP_F19, NONE),
	/* F20:	EMMC_STROBE			*/
	PAD_NC(GPP_F20, NONE),
	/* F21:	EMMC_CLK			*/
	PAD_NC(GPP_F21, NONE),
	/* F22:	EMMC_RESETB			*/
	PAD_NC(GPP_F22, NONE),
	/* F23:	A4WP_PRESENT			*/
	PAD_CFG_NF(GPP_F23, DN_20K, DEEP, NF1),

	/* G0:	Not Connected			*/
	PAD_NC(GPP_G0, NONE),
	/* G1:	Not Connected			*/
	PAD_NC(GPP_G1, NONE),
	/* G2:	Not Connected			*/
	PAD_NC(GPP_G2, NONE),
	/* G3:	Not Connected			*/
	PAD_NC(GPP_G3, NONE),
	/* G4:	Not Connected			*/
	PAD_NC(GPP_G4, NONE),
	/* G5:	Not Connected			*/
	PAD_NC(GPP_G5, UP_20K),
	/* G6:	Not Connected			*/
	PAD_NC(GPP_G6, NONE),
	/* G7:	Not Connected			*/
	PAD_NC(GPP_G7, DN_20K),

	/* H0:	Not Connected			*/
	PAD_NC(GPP_H0, NONE),
	/* H1:	 GPPC_H1_SSP2_SFRM		*/
	PAD_CFG_NF(GPP_H1, UP_20K, DEEP, NF3),
	/* H2:	 GPPC_H2_SSP2_TXD		*/
	PAD_CFG_NF(GPP_H2, UP_20K, DEEP, NF3),
	/* H3:	Not Connected			*/
	PAD_NC(GPP_H3, UP_20K),
	/* H4:	 GSENSOR_I2C_SDA		*/
	PAD_NC(GPP_H4, NONE),
	/* H5:	 GSENSOR_I2C_SCL		*/
	PAD_NC(GPP_H5, NONE),
	/* H8:	Not Connected			*/
	PAD_NC(GPP_H8, NONE),
	/* H9:	Not Connected			*/
	PAD_NC(GPP_H9, NONE),
	/* H10:	 ISH_I2C2_SDA			*/
	PAD_CFG_GPO(GPP_H10, 1, PLTRST),
	/* H11:	 ISH_I2C2_SCL			*/
	PAD_CFG_GPO(GPP_H11, 1, PLTRST),
	/* H12:	Not Connected			*/
	PAD_NC(GPP_H12, NONE),
	/* H13:	Not Connected			*/
	PAD_NC(GPP_H13, NONE),
	/* H14:	Not Connected			*/
	PAD_NC(GPP_H14, NONE),
	/* H15:	Not Connected			*/
	PAD_NC(GPP_H15, NONE),
	/* H16:	Not Connected			*/
	PAD_NC(GPP_H16, NONE),
	/* H17:	 GPPC_H_17_WWAN_DISABLE_N	*/
	PAD_CFG_GPO(GPP_H17, 0, DEEP),
	/* H18:	 GPPC_H_18_CPU_C10		*/
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* H19:	Not Connected			*/
	PAD_NC(GPP_H19, NONE),
	/* H20:	Not Connected			*/
	PAD_NC(GPP_H20, NONE),
	/* H21:	 GPPC_H21			*/
	PAD_CFG_GPO(GPP_H21, 0, DEEP),
	/* H22:	Not Connected			*/
	PAD_NC(GPP_H22, NONE),
	/* H23:	 GPPC_H23			*/
	PAD_CFG_GPO(GPP_H23, 0, DEEP),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
