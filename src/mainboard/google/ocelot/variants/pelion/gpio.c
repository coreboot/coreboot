/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* GPP_A */
	/* GPP_A00:     ESPI_IO0_AIC */
	/*  GPP_A00 : GPP_A00 ==> ESPI_IO0_EC_R configured on reset, do not touch */

	/* GPP_A01:     ESPI_IO1_AIC */
	/*  GPP_A01 : GPP_A01 ==> ESPI_IO1_EC_R configured on reset, do not touch */

	/* GPP_A02:     ESPI_IO2_AIC */
	/*  GPP_A02 : GPP_A02 ==> ESPI_IO2_EC_R configured on reset, do not touch */

	/* GPP_A03:     ESPI_IO3_AIC */
	/*  GPP_A03 : GPP_A03 ==> ESPI_IO3_EC_R configured on reset, do not touch */

	/* GPP_A04:     ESPI_CS0_AIC_N */
	/*  GPP_A04 : GPP_A04 ==> ESPI_CS0_HDR_L configured on reset, do not touch */

	/* GPP_A05:     ESPI_CLK_AIC */
	/*  GPP_A05 : GPP_A05 ==> ESPI_CLK_HDR configured on reset, do not touch */

	/* GPP_A06:     ESPI_RST_AIC_N */
	/*  GPP_A06 : GPP_A06 ==> ESPI_RST_HDR configured on reset, do not touch */

	/* GPP_A09:     NC */
	PAD_NC(GPP_A09, NONE),
	/* GPP_A10:     NC */
	PAD_NC(GPP_A10, NONE),
	/* GPP_A11:     WLAN_PERST_L (PCIe WLAN not stuffed, CNVi only) */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     NC */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     NC */
	PAD_NC(GPP_A13, NONE),
	/* GPP_A15:     EN_SPK_PA */
	PAD_CFG_GPO(GPP_A15, 1, DEEP),

	/* GPP_B */
	/* GPP_B00:     PMC_I2C_PD_SCL */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     PMC_I2C_PD_SDA */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     SOC_I2C_AUDIO_SAR_SDA */
	PAD_CFG_NF(GPP_B02, NONE, DEEP, NF8),
	/* GPP_B03:     SOC_I2C_AUDIO_SAR_SCL */
	PAD_CFG_NF(GPP_B03, NONE, DEEP, NF8),
	/* GPP_B04:     NC */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05:     EC_ISH_INT_ODL */
	PAD_CFG_NF(GPP_B05, NONE, DEEP, NF4),
	/* GPP_B06:     MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_B06, NONE, DEEP),
	/* GPP_B07:     WWAN_SAR_DETECT_ODL (WWAN option, not used) */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08:     MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_B08, NONE, DEEP),
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
	/* GPP_B16:     NC */
	PAD_NC(GPP_B16, NONE),
	/* GPP_B17:     NC */
	PAD_NC(GPP_B17, NONE),
	/* GPP_B18:     ISH_I2C_SENSOR_SDA */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B18, NONE, DEEP, NF1),
	/* GPP_B19:     ISH_I2C_SENSOR_SCL */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B19, NONE, DEEP, NF1),
	/* GPP_B20:     WWAN_RST_ODL (WWAN option, not used) */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B21:     NC */
	PAD_NC(GPP_B21, NONE),
	/* GPP_B22:     ISH_ACCEL_DB_INT_L */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF4),
	/* GPP_B23:     NC */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     NC */
	PAD_NC(GPP_B24, NONE),
	/* GPP_B25:     NC */
	PAD_NC(GPP_B25, NONE),

	/* GPP_C */
	/* GPP_C00:     EN_UCAM_PWR */
	PAD_CFG_GPO(GPP_C00, 1, DEEP),
	/* GPP_C01:     EN_FCAM_PWR */
	PAD_CFG_GPO(GPP_C01, 1, DEEP),
	/* GPP_C02:     NC */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03:     NC */
	PAD_NC(GPP_C03, NONE),
	/* GPP_C04:     NC */
	PAD_NC(GPP_C04, NONE),
	/* GPP_C05:     NC */
	PAD_NC(GPP_C05, NONE),
	/* GPP_C06:     NC */
	PAD_NC(GPP_C06, NONE),
	/* GPP_C07:     NC */
	PAD_NC(GPP_C07, NONE),
	/* GPP_C08:     SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_C08, 1, PLTRST),
	/* GPP_C09:     WLAN_PCIE_CLKREQ_ODL (PCIe WLAN not stuffed, CNVi only) */
	PAD_NC(GPP_C09, NONE),
	/* GPP_C10:     WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_C11:     NC */
	PAD_NC(GPP_C11, NONE),
	/* GPP_C13:     NC */
	PAD_NC(GPP_C13, NONE),
	/* GPP_C14:     NC */
	PAD_NC(GPP_C14, NONE),
	/* GPP_C15:     SOC_FP_RST_STRAP_L */
	PAD_NC(GPP_C15, NONE),
	/* GPP_C16:     USB_C0_LSX_TX */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17:     USB_C0_LSX_RX */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18:     USB_C1_LSX_TX */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* GPP_C19:     USB_C1_LSX_RX */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* GPP_C22:     DDIB_HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     DDIB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),

	/* GPP_D */
	/* GPP_D01:     NC */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     SOC_WP_OD */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_D03:     NC */
	PAD_NC(GPP_D03, NONE),
	/* GPP_D05:     UART_ISH_RX_DBG_TX */
	PAD_CFG_NF(GPP_D05, NONE, DEEP, NF2),
	/* GPP_D06:     UART_ISH_TX_DBG_RX */
	PAD_CFG_NF(GPP_D06, NONE, DEEP, NF2),
	/* GPP_D07:     NC */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08:     NC */
	PAD_NC(GPP_D08, NONE),
	/* GPP_D09:     I2S0_MCLK_R */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2),
	/* GPP_D10:     HDA_BCLK_I2S0_SCLK */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2),
	/* GPP_D11:     HDA_SYNC_I2S0_SFRM */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2),
	/* GPP_D12:     HDA_SDO_I2S0_TXD_STRAP */
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF2),
	/* GPP_D13:     HDA_SDI0_I2S0_RDX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2),
	/* GPP_D16:     NC */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     NC */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D19:     SOC_TCHSCR_REPORT_EN */
	PAD_CFG_GPO(GPP_D19, 1, PLTRST),

	/* GPP_E */
	/* GPP_E01:     EN_WWAN_PWR (WWAN option, not used) */
	PAD_NC(GPP_E01, NONE),
	/* GPP_E02:     SOC_VR_ALERT_ODL */
	PAD_CFG_NF(GPP_E02, NONE, DEEP, NF2),
	/* GPP_E03:     NC */
	PAD_NC(GPP_E03, NONE),
	/* GPP_E05:     EN_PWR_FP */
	PAD_CFG_GPO(GPP_E05, 1, DEEP),
	/* GPP_E06:     NC */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     WLAN_PCIE_WAKE_ODL (PCIe WLAN not stuffed, CNVi only) */
	PAD_NC(GPP_E07, NONE),
	/* GPP_E08:     EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E08, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E09:     USB_A_OC_ODL */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     SD_PE_PRSNT_L (SD reader option, not used) */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     NC */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     I2C_TCHPAD_SCL */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),
	/* GPP_E13:     I2C_TCHPAD_SDA */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),
	/* GPP_E14: GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E14, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E15:     NC */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     NC */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     NC */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     TCHPAD_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E18, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_E19:     NC */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20:     NC */
	PAD_NC(GPP_E20, NONE),
	/* GPP_E21:     PMC_I2C_PD_INT_ODL */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     NC */
	PAD_NC(GPP_E22, NONE),

	/* GPP_F */
	/* GPP_F00:     CNV_BRI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     CNV_BRI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     CNV_RGI_DT */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     CNV_RGI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RST_L */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CNV_CLKREQ0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06:     COEX3_WWAN_WLAN_CNVI (WWAN option, not used) */
	PAD_NC(GPP_F06, NONE),
	/* GPP_F07:     NC */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     EN_TCHSCR_PWR */
	PAD_CFG_GPO(GPP_F08, 1, PLTRST),
	/* GPP_F10:     ISH_ACCEL_MB_INT_L */
	PAD_CFG_NF(GPP_F10, NONE, DEEP, NF8),
	/* GPP_F11:     SPI_TCHSCR_CLK (SPI touchscreen option, not used) */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12:     SPI_I2C_TCHSCR_MOSI_SCL */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* GPP_F13:     SPI_I2C_TCHSCR_MISO_SDA */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* GPP_F14:     NC */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     NC */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     TCHSCR_RST_ODL */
	PAD_CFG_GPO(GPP_F16, 1, DEEP),
	/* GPP_F17:     SPI_TCHSCR_CS_L (SPI touchscreen option, not used) */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18:     TCHSCR_INT_ODL */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF3),
	/* GPP_F19:     NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     EC_SOC_REC_SWITCH_ODL */
	PAD_CFG_GPI(GPP_F20, NONE, DEEP),
	/* GPP_F22:     NC */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23:     NC */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H */
	/* GPP_H00:     NC */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H01:     NC */
	PAD_NC(GPP_H01, NONE),
	/* GPP_H02:     NC */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H03:     NC */
	PAD_NC(GPP_H03, NONE),
	/* GPP_H04:     NC */
	PAD_NC(GPP_H04, NONE),
	/* GPP_H05:     NC */
	PAD_NC(GPP_H05, NONE),
	/* GPP_H06:     NC */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07:     NC */
	PAD_NC(GPP_H07, NONE),
	/* GPP_H08:     UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10:     NC */
	PAD_NC(GPP_H10, NONE),
	/* GPP_H11:     NC */
	PAD_NC(GPP_H11, NONE),
	/* GPP_H13:     NC */
	PAD_NC(GPP_H13, NONE),
	/* GPP_H14:     NC */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15:     NC */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H17:     HP_INT_ODL */
	PAD_CFG_GPI_INT(GPP_H17, NONE, PLTRST, EDGE_BOTH),
	/* GPP_H19:     NC */
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20:     NC */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21:     SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* GPP_H23:     MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H23, NONE, DEEP),
	/* GPP_H24:     MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H24, NONE, DEEP),

	/* GPP_S00:     SDW3_CLK_I2S1_TXD (I2S_SPK_AUDIO) */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF6),
	/* GPP_S01:     SDW_HP_DAT0 (not connected) */
	PAD_NC(GPP_S01, NONE),
	/* GPP_S02:     SDW3_DAT1_I2S1_SCLK (I2S_SPK_BCLK) */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF6),
	/* GPP_S03:     SDW3_DAT2_I2S1_SFRM (I2S_SPK_LRCK) */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF6),
	/* GPP_S04:	DMIC_CLK0_EDP */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* GPP_S05:	DMIC_DATA0_EDP */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
	/* GPP_S06:     NC */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07:     NC */
	PAD_NC(GPP_S07, NONE),

	/* GPP_V */
	/* GPP_V00:     BATLOW_L */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     ACPRESENT */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     EC_SOC_WAKE_ODL */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     EC_SOC_PWR_BTN_ODL */
	PAD_CFG_NF(GPP_V03, UP_20K, DEEP, NF1),
	/* GPP_V04:     SLP_S3_L */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     SLP_S4_L */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     TP_SLP_A_L */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     SOC_WLAN_SUSCLK */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     RSVD */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09:     SLP_S5_L */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     NC */
	PAD_NC(GPP_V10, NONE),
	/* GPP_V11:     NC */
	PAD_NC(GPP_V11, NONE),
	/* GPP_V12:     BOOTHALT_L */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V13:     GPP_V13_CATERR_N */
	PAD_CFG_NF(GPP_V13, NONE, DEEP, NF1),
	/* GPP_V14:     GPP_V14_FORCEPR_N */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V15:     GPP_V15_THERMTRIP_N */
	PAD_CFG_NF(GPP_V15, NONE, DEEP, NF1),
	/* GPP_V16:     GPP_V16_VCCST_EN */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),

	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC1, NONE, PLTRST, LEVEL, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_E14:     GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E14, NONE, PLTRST, LEVEL, INVERT),

	/* GPP_F08:     EN_TCHSCR_PWR */
	PAD_CFG_GPO(GPP_F08, 1, DEEP),
	/* GPP_F16:     TCHSCR_RST_ODL */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),

	/* GPP_H08:     UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_SOC_TX_DBG_RX */
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
