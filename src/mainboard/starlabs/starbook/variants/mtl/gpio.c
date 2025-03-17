/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
const struct pad_config early_gpio_table[] = {
	/* H08:	UART0 RXD			*/
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* H09:	UART0 TXD			*/
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),

	/* C00:	SMB_CLK				*/
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* C01:	SMB_DATA			*/
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Pad configuration in ramstage. */
const struct pad_config gpio_table[] = {
	/*
	 * GPP_V
	 * Start:	GPP_V00
	 * End:		GPP_V22
	 */
	/* V00:	PM_BATLOW_N			*/
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* V01:	CHG_ACOK			*/
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* V02:	PCIE_WAKE_LAN			*/
	PAD_NC(GPP_V02, NONE),
	/* V03:	EC_PWRBTN_N			*/
	PAD_CFG_NF(GPP_V03, UP_20K, DEEP, NF1),
	/* V04:	PM_SLP_S3_N			*/
	PAD_NC(GPP_V04, NONE),
	/* V05:	SLP S4#				*/
	PAD_NC(GPP_V05, NONE),
	/* V06:	GPD_6_SLP_A_N			*/
	PAD_NC(GPP_V06, NONE),
	/* V07:					*/
	PAD_NC(GPP_V07, NONE),
	/* V08:	M.2_BTWIFI_SUS_CLK		*/
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* V09:	GPD_9_SLP_WLAN_N		*/
	PAD_NC(GPP_V09, NONE),
	/* V10:	PM_SLP_S5_N			*/
	PAD_NC(GPP_V10, NONE),
	/* V11:	LANPHY_ENABLE			*/
	PAD_NC(GPP_V11, NONE),
	/* V12:	SLP_LAN_N			*/
	PAD_NC(GPP_V12, NONE),
	/* V13					*/
	PAD_NC(GPP_V13, NONE),
	/* V14:	WAKE_N				*/
	PAD_NC(GPP_V14, NONE),
	/* V15:					*/
	PAD_NC(GPP_V15, NONE),
	/* V16:					*/
	PAD_NC(GPP_V16, NONE),
	/* V17:					*/
	PAD_NC(GPP_V17, NONE),
	/* V18:					*/
	PAD_NC(GPP_V18, NONE),
	/* V19:					*/
	PAD_NC(GPP_V19, NONE),
	/* V20:					*/
	PAD_NC(GPP_V20, NONE),
	/* V21:					*/
	PAD_NC(GPP_V21, NONE),
	/* V22:					*/
	PAD_NC(GPP_V22, NONE),
	/* V23:					*/
	PAD_NC(GPP_V23, NONE),

	/*
	 * GPP_C
	 * Start:	GPP_C00
	 * End:		GPP_C23
	 */
	/* C02:	TLS CONFIDENTIALITY
	 *		HIGH:	ENABLED
	 *		LOW:	DISABLED
	 *		WEAK INTERNAL PD 20K	*/
	PAD_CFG_GPO(GPP_C02, 1, DEEP),
	/* C03:	SML0_CLK			*/
	PAD_CFG_NF(GPP_C03, NONE, DEEP, NF1),
	/* C04:	SML0_DATA			*/
	PAD_CFG_NF(GPP_C04, NONE, DEEP, NF1),
	/* C05:	ESPI
	 *		HIGH:	ENABLED
	 *		LOW:	DISABLED
	 *		WEAK INTERNAL PD 20K	*/
	PAD_CFG_GPO(GPP_C05, 1, DEEP),
	/* C06:	SML1_CLK_USBC_PD_N		*/
	PAD_CFG_NF(GPP_C06, NONE, DEEP, NF1),
	/* C07:	SML1_DATA_USBC_PD_N		*/
	PAD_CFG_NF(GPP_C07, NONE, DEEP, NF1),
	/* C08:	SML1ALERT			*/
	PAD_NC(GPP_C08, NONE),
	/* C09:					*/
	PAD_NC(GPP_C09, NONE),
	/* C10:					*/
	PAD_NC(GPP_C10, NONE),
	/* C11:					*/
	PAD_NC(GPP_C11, NONE),
	/* C12:	CLKREQ3				*/
	PAD_NC(GPP_C12, NONE),
	/* C13:					*/
	PAD_NC(GPP_C13, NONE),
	/* C14:					*/
	PAD_NC(GPP_C14, NONE),
	/* C15:	RESERVED
	 *		WEAK INTERNAL PD 20K	*/
	PAD_NC(GPP_C15, NONE),
	/* C16:					*/
	PAD_NC(GPP_C16, NONE),
	/* C17:					*/
	PAD_NC(GPP_C17, NONE),
	/* C18:					*/
	PAD_NC(GPP_C18, NONE),
	/* C19:					*/
	PAD_NC(GPP_C19, NONE),
	/* C20:					*/
	PAD_NC(GPP_C20, NONE),
	/* C21:					*/
	PAD_NC(GPP_C21, NONE),
	/* C22:					*/
	PAD_NC(GPP_C22, NONE),
	/* C23:					*/
	PAD_NC(GPP_C23, NONE),

	/*
	 * GPP_A
	 * Start:	GPP_A00
	 * End:		GPP_A23
	 */
	/* A00:	ESPI_IO0_EC_R			*/
	PAD_CFG_NF(GPP_A00, UP_20K, DEEP, NF1),
	/* A01:	ESPI_IO1_EC_R			*/
	PAD_CFG_NF(GPP_A01, UP_20K, DEEP, NF1),
	/* A02:	ESPI_IO2_EC_R			*/
	PAD_CFG_NF(GPP_A02, UP_20K, DEEP, NF1),
	/* A03:	ESPI_IO3_EC_R			*/
	PAD_CFG_NF(GPP_A03, UP_20K, DEEP, NF1),
	/* A04:	ESPI_CS0_N			*/
	PAD_CFG_NF(GPP_A04, UP_20K, DEEP, NF1),
	/* A05:	ESPI_CLK_EC_R			*/
	PAD_CFG_NF(GPP_A05, UP_20K, DEEP, NF1),
	/* A06:	ESPI_RESET_N			*/
	PAD_CFG_NF(GPP_A06, NONE, DEEP, NF1),
	/* A07:					*/
	PAD_NC(GPP_A07, NONE),
	/* A08:					*/
	PAD_NC(GPP_A08, NONE),
	/* A09:					*/
	PAD_NC(GPP_A09, NONE),
	/* A10:					*/
	PAD_NC(GPP_A10, NONE),
	/* A11:					*/
	PAD_NC(GPP_A11, NONE),
	/* A12:	WLAN_PEWAKE			*/
	PAD_CFG_GPI(GPP_A12, NONE, DEEP),
	/* A13:					*/
	PAD_NC(GPP_A13, NONE),
	/* A14:					*/
	PAD_NC(GPP_A14, NONE),
	/* A15:					*/
	PAD_NC(GPP_A15, NONE),
	/* A16:					*/
	PAD_NC(GPP_A16, NONE),
	/* A17:	EC_SLP_S0_CS_N			*/
	PAD_NC(GPP_A17, NONE),
	/* A18:					*/
	PAD_NC(GPP_A18, NONE),
	/* A19:					*/
	PAD_NC(GPP_A19, NONE),
	/* A20:	M.2_CPU_SSD_RESET_N		*/
	PAD_CFG_GPO(GPP_A20, 1, PLTRST),
	/* A21:	I2C_PMC_PD_INT_N		*/
	PAD_CFG_NF(GPP_A21, NONE, DEEP, NF1),
	/* A22:					*/
	PAD_NC(GPP_A22, NONE),
	/* A23:					*/
	PAD_NC(GPP_A23, NONE),

	/*
	 * GPP_E
	 * Start:	GPP_E00
	 * End:		GPP_E23
	 */
	/* E00:					*/
	PAD_NC(GPP_E00, NONE),
	/* E01:					*/
	PAD_NC(GPP_E01, NONE),
	/* E02:					*/
	PAD_NC(GPP_E02, NONE),
	/* E03:					*/
	PAD_NC(GPP_E03, NONE),
	/* E04:	M.2_SSD_DEVSLP0			*/
	PAD_NC(GPP_E04, NONE),
	/* E05:					*/
	PAD_NC(GPP_E05, NONE),
	/* E06:	JTAG ODT
	 *		HIGH:	ENABLED
	 *		LOW:	DISABLED
	 *		20K INTERNAL PU		*/
	/* E07:					*/
	PAD_NC(GPP_E07, NONE),
	/* E08:					*/
	PAD_NC(GPP_E08, NONE),
	/* E09:	USB2_OC0			*/
	/* E10:					*/
	PAD_NC(GPP_E10, NONE),
	/* E11:					*/
	PAD_NC(GPP_E11, NONE),
	/* E12:					*/
	PAD_NC(GPP_E12, NONE),
	/* E13:					*/
	PAD_NC(GPP_E13, NONE),
	/* E14:	EDP_HPD_N			*/
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15:	BOOTHALT_N			*/
	PAD_NC(GPP_E15, NONE),
	/* E16:	BC_PROCHOT_N			*/
	PAD_CFG_GPI_SCI(GPP_E16, NONE, PLTRST, EDGE_SINGLE, INVERT),
	/* E17:					*/
	PAD_NC(GPP_E17, NONE),
	/* E18:					*/
	PAD_NC(GPP_E18, NONE),
	/* E19:					*/
	PAD_NC(GPP_E19, NONE),
	/* E20:					*/
	PAD_NC(GPP_E20, NONE),
	/* E21:					*/
	PAD_NC(GPP_E21, NONE),
	/* E22:					*/
	PAD_NC(GPP_E22, NONE),
	/* E23:					*/
	PAD_NC(GPP_E23, NONE),

	/*
	 * GPP_H
	 * Start:	GPP_H00
	 * End:		GPP_H23
	 */
	/* H00:	M.2_PCH_SSD_RESET_N		*/
	PAD_CFG_GPO(GPP_H00, 1, PLTRST),
	/* H01:	FLASH RECOVERY
	 *		HIGH:	ENABLED
	 *		LOW:	DISABLED
	 *		WEAK INTERNAL PD 20K	*/
	PAD_NC(GPP_H01, NONE),
	/* H02:	WLAN_RST_N			*/
	PAD_CFG_GPO(GPP_H02, 1, PLTRST),
	/* H03:					*/
	PAD_NC(GPP_H03, NONE),
	/* H04:					*/
	PAD_NC(GPP_H04, NONE),
	/* H05:					*/
	PAD_NC(GPP_H05, NONE),
	/* H06:					*/
	PAD_NC(GPP_H06, NONE),
	/* H07:	M.2_CPU_SSD_PWREN		*/
	PAD_CFG_GPO(GPP_H07, 1, DEEP),
	/* H10:					*/
	PAD_NC(GPP_H10, NONE),
	/* H11:					*/
	PAD_NC(GPP_H11, NONE),
	/* H12:					*/
	PAD_NC(GPP_H12, NONE),
	/* H13:	CPU_C10_GATE_N_R		*/
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* H14:					*/
	PAD_NC(GPP_H14, NONE),
	/* H15:					*/
	PAD_NC(GPP_H15, NONE),
	/* H16:	DDPB_CTRLCLK			*/
	PAD_CFG_NF(GPP_H16, NONE, DEEP, NF1),
	/* H17:	DDPB_CTRLDATA			*/
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H18:					*/
	PAD_NC(GPP_H18, NONE),
	/* H19:	TCHPAD_I2C0_SDA_N		*/
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* H20:	TCHPAD_I2C0_SCL_N		*/
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),
	/* H21:					*/
	PAD_NC(GPP_H21, NONE),
	/* H22:					*/
	PAD_NC(GPP_H22, NONE),
	/* H23:					*/
	PAD_NC(GPP_H23, NONE),

	/*
	 * GPP_F
	 * Start:	GPP_F00
	 * End:		GPP_F23
	 */
	/* F00:	CNV_BRI_DT			*/
	PAD_NC(GPP_F00, NONE),
	/* F01:	CNV_BRI_RSP			*/
	PAD_NC(GPP_F01, NONE),
	/* F02:	CNV_RGI_DT			*/
	PAD_NC(GPP_F02, NONE),
	/* F03:	CNV_RGI_RSP			*/
	PAD_NC(GPP_F03, NONE),
	/* F04:	CNVI_RF_RESET_N			*/
	PAD_NC(GPP_F04, NONE),
	/* F05:	MODEM_CLKREQ			*/
	PAD_NC(GPP_F05, NONE),
	/* F06:					*/
	PAD_NC(GPP_F06, NONE),
	/* F07:					*/
	PAD_NC(GPP_F07, NONE),
	/* F08:					*/
	PAD_NC(GPP_F08, NONE),
	/* F09:					*/
	PAD_NC(GPP_F09, NONE),
	/* F10:	SATAXPCIE_1_SATAGP_1		*/
	PAD_NC(GPP_F10, NONE),
	/* F11:					*/
	PAD_NC(GPP_F11, NONE),
	/* F12:					*/
	PAD_NC(GPP_F12, NONE),
	/* F13:	TPM_IRQ				*/
	PAD_CFG_GPI_APIC_LOW(GPP_F13, NONE, PLTRST),
	/* F14:					*/
	PAD_NC(GPP_F14, NONE),
	/* F15:					*/
	PAD_NC(GPP_F15, NONE),
	/* F16:					*/
	PAD_NC(GPP_F16, NONE),
	/* F17:					*/
	PAD_NC(GPP_F17, NONE),
	/* F18:					*/
	PAD_NC(GPP_F18, NONE),
	/* F19:	ESPI FLASH SHARING
	 *		HIGH:	MASTER
	 *		LOW:	SLAVE
	 *		WEAK INTERNAL PD 20K	*/
	PAD_NC(GPP_F19, NONE),
	/* F20:	SVID
	 *		HIGH:	PRESENT
	 *		LOW:	NOT PRESENT
	 *		WEAK INTERNAL PD 20K	*/
	PAD_NC(GPP_F20, NONE),
	/* F21:	CCD
	 *		HIGH:	BALTIC PEAK
	 *		LOW:	BSSB-LS
	 *		WEAK INTERNAL PU 20K	*/
	/* F21:					*/
	PAD_NC(GPP_F21, NONE),
	/* F22:					*/
	PAD_NC(GPP_F22, NONE),
	/* F23:					*/
	PAD_NC(GPP_F23, NONE),

	/*
	 * GPP_S
	 * Start:	GPP_S00
	 * End:		GPP_S07
	 */
	/* S00:					*/
	PAD_NC(GPP_S00, NONE),
	/* S01:					*/
	PAD_NC(GPP_S01, NONE),
	/* S02:					*/
	PAD_NC(GPP_S02, NONE),
	/* S03:					*/
	PAD_NC(GPP_S03, NONE),
	/* S04:					*/
	PAD_NC(GPP_S04, NONE),
	/* S05:					*/
	PAD_NC(GPP_S05, NONE),
	/* S06:					*/
	PAD_NC(GPP_S06, NONE),
	/* S07:					*/
	PAD_NC(GPP_S07, NONE),

	/*
	 * GPP_B
	 * Start:	GPP_B00
	 * End:		GPP_B23
	 */
	/* B00:	TCHPAD_INT_N			*/
	PAD_CFG_GPI_APIC(GPP_B00, NONE, DEEP, LEVEL, INVERT),
	/* B01:					*/
	PAD_NC(GPP_B01, NONE),
	/* B02:					*/
	PAD_NC(GPP_B02, NONE),
	/* B03:					*/
	PAD_NC(GPP_B03, NONE),
	/* B04:	REBOOT CONTROL
	 *		HIGH:	NO REBOOT
	 *		LOW:	REBOOT ENABLED
	 *		WEAK INTERNAL PD 20K
	 */
	PAD_CFG_GPO(GPP_B04, 0, DEEP),
	/* B05:					*/
	PAD_NC(GPP_B05, NONE),
	/* B06:					*/
	PAD_NC(GPP_B06, NONE),
	/* B07:					*/
	PAD_NC(GPP_B07, NONE),
	/* B08:					*/
	PAD_NC(GPP_B08, NONE),
	/* B09:					*/
	PAD_NC(GPP_B09, NONE),
	/* B10:					*/
	PAD_NC(GPP_B10, NONE),
	/* B11:					*/
	PAD_NC(GPP_B11, NONE),
	/* B12:					*/
	PAD_NC(GPP_B12, NONE),
	/* B13:	PLT_RST_N			*/
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* B14:					*/
	PAD_NC(GPP_B14, NONE),
	/* B15:					*/
	PAD_NC(GPP_B15, NONE),
	/* B16:	DDIB_DP_HPD			*/
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF2),
	/* B17:					*/
	PAD_NC(GPP_B17, NONE),
	/* B18:	BT_RF_KILL_N			*/
	PAD_CFG_GPO(GPP_B18, 1, DEEP),
	/* B19:	WIFI_RF_KILL_N			*/
	PAD_CFG_GPO(GPP_B19, 1, DEEP),
	/* B20:					*/
	PAD_NC(GPP_B20, NONE),
	/* B21:					*/
	PAD_NC(GPP_B21, NONE),
	/* B22:					*/
	PAD_NC(GPP_B22, NONE),
	/* B23:					*/
	PAD_NC(GPP_B23, NONE),

	/*
	 * GPP_D
	 * Start:	GPP_D00
	 * End:		GPP_D23
	 */
	/* D00:					*/
	PAD_NC(GPP_D00, NONE),
	/* D01:					*/
	PAD_NC(GPP_D01, NONE),
	/* D02:					*/
	PAD_NC(GPP_D02, NONE),
	/* D03:					*/
	PAD_NC(GPP_D03, NONE),
	/* D04:					*/
	PAD_NC(GPP_D04, NONE),
	/* D05:					*/
	PAD_NC(GPP_D05, NONE),
	/* D06:	SML0BALERT#			*/
	PAD_NC(GPP_D06, NONE),
	/* D07:					*/
	PAD_NC(GPP_D07, NONE),
	/* D08:					*/
	PAD_NC(GPP_D08, NONE),
	/* D09:					*/
	PAD_NC(GPP_D09, NONE),
	/* D10:	HDA_CODEC_BCLK			*/
	PAD_CFG_NF(GPP_D10, NATIVE, DEEP, NF1),
	/* D11:	HDA_CODEC_SYNC			*/
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),
	/* D12:	HDA_CODEC_SDO			*/
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),
	/* D13:	HDA_CODEC_SDI			*/
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),
	/* D14:					*/
	PAD_NC(GPP_D14, NONE),
	/* D15:					*/
	PAD_NC(GPP_D15, NONE),
	/* D16:					*/
	PAD_NC(GPP_D16, NONE),
	/* D17:	HDA_CODEC_RESET0_N		*/
	PAD_CFG_NF(GPP_D17, NATIVE, DEEP, NF1),
	/* D18:					*/
	PAD_NC(GPP_D18, NONE),
	/* D19:					*/
	PAD_NC(GPP_D19, NONE),
	/* D20: CLKREQ8
	 *			SSD		*/
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	/* D21:	CLKREQ5
	 *			WLAN		*/
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF2),
	/* D22:					*/
	PAD_NC(GPP_D22, NONE),
	/* D23:					*/
	PAD_NC(GPP_D23, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
