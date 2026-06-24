/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* GPP_A00:     ESPI_IO0_EC_R */
	/*  GPP_A00 : GPP_A00 ==> ESPI_IO0_EC_R configured on reset, do not touch */

	/* GPP_A01:     ESPI_IO1_EC_R */
	/*  GPP_A01 : GPP_A01 ==> ESPI_IO1_EC_R configured on reset, do not touch */

	/* GPP_A02:     ESPI_IO2_EC_R */
	/*  GPP_A02 : GPP_A02 ==> ESPI_IO2_EC_R configured on reset, do not touch */

	/* GPP_A03:     ESPI_IO3_EC_R */
	/*  GPP_A03 : GPP_A03 ==> ESPI_IO3_EC_R configured on reset, do not touch */

	/* GPP_A04:     ESPI_CS0_EC_R_N */
	/*  GPP_A04 : GPP_A04 ==> ESPI_CS0_HDR_L configured on reset, do not touch */

	/* GPP_A05:     ESPI_CLK_EC_R */
	/*  GPP_A05 : GPP_A05 ==> ESPI_CLK_HDR configured on reset, do not touch */

	/* GPP_A06:     ESPI_RST_EC_R_N */
	/*  GPP_A06 : GPP_A06 ==> ESPI_RST_HDR configured on reset, do not touch */

	/* GPP_A08:     SSD_PERST_L */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_A09:     Not used */
	PAD_NC(GPP_A09, NONE),
	/* GPP_A10:     Not used */
	PAD_NC(GPP_A10, NONE),
	/* GPP_A11:     WLAN_PERST_L */
	PAD_CFG_GPO(GPP_A11, 1, PLTRST),
	/* GPP_A12:     Not used */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     Not used */
	PAD_NC(GPP_A13, NONE),
	/* GPP_A15:     Not used */
	PAD_NC(GPP_A15, NONE),

	/* GPP_B00:     USBC_SML_CLK_PD ==> I2C_SOC_PD_SCL_R */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     USBC_SML_DATA_PD ==> I2C_SOC_PD_SDA_R */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     Not used */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03:     Not used */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04:     ISH_GP_0_SNSR_HDR ==> NC*/
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05:     EC_ISH_INT_ODL */
	PAD_CFG_NF(GPP_B05, NONE, DEEP, NF4),
	/* GPP_B06:     SOC_PDB_CTRL ==> NC*/
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07:     ISH_GP_3_SNSR_HDR ==> NC */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08:     ISH_GP_4_SNSR_HDR ==> NC */
	PAD_NC(GPP_B08, NONE),
	/* GPP_B09:     BT_DISABLE_L */
	PAD_CFG_GPO(GPP_B09, 1, DEEP),
	/* GPP_B10:     NC */
	PAD_NC(GPP_B10, NONE),
	/* GPP_B12:     SLP_S0_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13:     PLT_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14:     HDMI_HPD_L_STRAP */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF2),
	/* GPP_B16:     COINLESS_MODE_SELECT ==> NC*/
	PAD_NC(GPP_B16, NONE),
	/* GPP_B18:     Not used */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     Not used */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B20:     Not used */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B23:     Not used */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     ESPI_ALERT0_EC_R_N ==> NC*/
	PAD_NC(GPP_B24, NONE),
	/* GPP_B25:     Not used */
	PAD_NC(GPP_B25, NONE),

	/* GPP_C00:     GPP_C0_SMBCLK ==> SMB_CLK */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* GPP_C01:     GPP_C1_SMBDATA ==> SMB_DAT */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
	/* GPP_C02:     Not used */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03:     TBT_SMB_CLK */
	PAD_CFG_NF(GPP_C03, NONE, DEEP, NF1),
	/* GPP_C04:     TBT_SMB_DATA */
	PAD_CFG_NF(GPP_C04, NONE, DEEP, NF1),
	/* GPP_C05:     NC */
	PAD_NC(GPP_C05, NONE),
	/* GPP_C06:     NC */
	PAD_NC(GPP_C06, NONE),
	/* GPP_C07:     NC */
	PAD_NC(GPP_C07, NONE),
	/* GPP_C08:     SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_C08, 1, DEEP),
	/* GPP_C09:     WLAN_PCIE_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1),
	/* GPP_C10:     WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_C12:     SSD_PCIE_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
	/* GPP_C13:     Not used */
	PAD_NC(GPP_C13, NONE),
	/* GPP_C14:     Not used */
	PAD_NC(GPP_C14, NONE),
	/* GPP_C15:     SOC_FP_RST_STRAP_L */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* GPP_C16:     UNot used */
	PAD_NC(GPP_C16, NONE),
	/* GPP_C17:     Not used */
	PAD_NC(GPP_C17, NONE),
	/* GPP_C18:     Not used */
	PAD_NC(GPP_C18, NONE),
	/* GPP_C19:     Not used */
	PAD_NC(GPP_C19, NONE),
	/* GPP_C22:     SOC_HDMI_DDC_SCL*/
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     SOC_HDMI_DDC_SDA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),

	/* GPP_D01:     Not used */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     SOC_WP_OD */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_D03:     Not used */
	PAD_NC(GPP_D03, NONE),
	/* GPP_D05:     HW_ID3  */
	PAD_CFG_GPI(GPP_D05, NONE, DEEP),
	/* GPP_D06:     HW_ID2  */
	PAD_CFG_GPI(GPP_D06, NONE, DEEP),
	/* GPP_D07:     Not used */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08:     HW_ID4 */
	PAD_CFG_GPI(GPP_D08, NONE, DEEP),
	/* GPP_D09:     Not used */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10:     Not used */
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11:     Not used */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12:     Not used ==> HDA_SDO_I2S0_TXD_STRAP */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13:     Not used */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D16:     Not used  */ /* DMIC connect to Audio Codec */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     Not used  */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D19:     Not used */
	PAD_NC(GPP_D19, NONE),
	/* GPP_D21:     Not used */
	PAD_NC(GPP_D21, NONE),

	/* GPP_E01:     Not used */
	PAD_NC(GPP_E01, NONE),
	/* GPP_E02:     SOC_VR_ALERT_ODL */
	PAD_CFG_NF(GPP_E02, NONE, DEEP, NF2),
	/* GPP_E03:     Not used */
	PAD_NC(GPP_E03, NONE),
	/* GPP_E05:     EN_PWR_FP */
	PAD_CFG_GPO(GPP_E05, 1, DEEP),
	/* GPP_E06:     Not used */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW_LOCK(GPP_E07, NONE, EDGE_SINGLE, LOCK_CONFIG),
	/* GPP_E08:     EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E08, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E09:     USB_A_OC_ODL */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     Not used */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     Not used */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     I2C_TCHPAD_SCL_R */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* GPP_E13:     I2C_TCHPAD_SDA_R */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* GPP_E14:     GSC_SOC_INT_ODL  */
	PAD_CFG_GPI_APIC_LOCK(GPP_E14, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E15:     Not used */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     Not used */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     Not used  */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     TCHPAD_INT_ODL */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF3),
	/* GPP_E19:     HW_ID1 */
	PAD_CFG_GPI(GPP_E19, NONE, DEEP),
	/* GPP_E20:     HW_ID0 */
	PAD_CFG_GPI(GPP_E20, NONE, DEEP),
	/* GPP_E21:     PMC_I2C_PD_INT_ODL */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     Not used */
	PAD_NC(GPP_E22, NONE),

	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CRF_CLKREQ_R */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06:     Not used */
	PAD_NC(GPP_F06, NONE),
	/* GPP_F07:     Not used */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     EN_TCHSCR_PWR */
	PAD_CFG_GPO(GPP_F08, 1, PLTRST),
	/* GPP_F09:     NC */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F10:     NC */
	PAD_NC(GPP_F10, NONE),
	/* GPP_F11:     NC */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12:     I2C_TCHSCR_SCL */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* GPP_F13:     I2C_TCHSCR_SDA */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* GPP_F14:     NC */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     NC */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     TCHSCR_RST_ODL */
	PAD_CFG_GPO(GPP_F16, 1, DEEP),
	/* GPP_F17:     NC */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18:     TCHSCR_INT_ODL */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF3),
	/* GPP_F19:     NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     EC_SOC_REC_SWITCH_ODL */
	PAD_CFG_GPI(GPP_F20, NONE, DEEP),
	/* GPP_F22:     NC */
	PAD_NC(GPP_F22, NONE),

	/* GPP_H00:     Not used */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H01:     Not used */
	PAD_NC(GPP_H01, NONE),
	/* GPP_H02:     Not used */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H03:     Not used */
	PAD_NC(GPP_H03, NONE),
	/* GPP_H04:     SOC_CAM_PWR_EN */
	PAD_CFG_GPO(GPP_H04, 1, DEEP),
	/* GPP_H05:     Not used */
	PAD_NC(GPP_H05, NONE),
	/* GPP_H06:     Not used */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07:     Not used */
	PAD_NC(GPP_H07, NONE),
	/* GPP_H08:     SOC_DEBUG_UART0_RXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     SOC_DEBUG_UART0_TXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10:     Not used */
	PAD_NC(GPP_H10, NONE),
	/* GPP_H11:     Not used */
	PAD_NC(GPP_H11, NONE),
	/* GPP_H13:     SOC_EC_C10_GATE_R_N */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* GPP_H14:     NC */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15:     NC */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H17:     NC */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H19:     NC */
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20:     NC */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21:     SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* GPP_H23:     NC */
	PAD_NC(GPP_H23, NONE),
	/* GPP_H24:     NC */
	PAD_NC(GPP_H24, NONE),

	/* GPP_S00:     SDW_HP_CLK */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01:     SDW_HP_DAT0 */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02:     SDW_HP_DAT1 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S03:     SDW_HP_DAT2 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* GPP_S04:     DMIC_CLK0_EDP */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* GPP_S05:     DMIC_DATA0_EDP */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),

	/* GPP_V00:     BATLOW_L */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     ACPRESENT */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     EC_SOC_WAKE_ODL */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     EC_SOC_PWR_BTN_ODL */
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1),
	/* GPP_V04:     SLP_S3_L */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     SLP_S4_L */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     TP_SLP_A_L */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     SUS_CLK */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     SLP_WLAN_N */ /* NC */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09:     SLP_S5_L */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     LANPHYPC_R_N */ /* NC */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),
	/* GPP_V11:     TP_SLP_LAN_L */
	PAD_CFG_NF(GPP_V11, NONE, DEEP, NF1),
	/* GPP_V12:     WAKE_N */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V16:     EN_VCCST */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),

	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC0, NONE, PLTRST, LEVEL, NONE),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC1, NONE, PLTRST, LEVEL, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_A11:     WLAN_RST_N */
	PAD_CFG_GPO(GPP_A11, 0, PLTRST),

	/* GPP_C15:     SOC_FP_RST_STRAP_L */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),

	/* GPP_E05:     FP_PWR_EN */
	PAD_CFG_GPO(GPP_E05, 1, PLTRST),
	/* GPP_E14:     GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E14, NONE, PLTRST, LEVEL, INVERT),

	/* GPP_F08:     EN_TCHSCR_PWR */
	PAD_CFG_GPO(GPP_F08, 1, DEEP),
	/* GPP_F16:     TCHSCR_RST_ODL */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),

	/* GPP_H08:     SOC_DEBUG_UART0_RXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     SOC_DEBUG_UART0_TXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H21:     SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
};

/* Pad configuration in romstage */
static const struct pad_config romstage_gpio_table[] = {
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* Create the stub for romstage gpio, typically use for power sequence */
const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE0_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE1_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE2_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE3_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE4_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
