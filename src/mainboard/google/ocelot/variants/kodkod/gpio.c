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

	/* GPP_A08:     SOC_SSD_RST# */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_A09:     NC */
	PAD_NC(GPP_A09, NONE),
	/* GPP_A10:     NC */
	PAD_NC(GPP_A10, NONE),
	/* GPP_A11:     SOC_WLAN_RST# */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     NC*/
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     NC */
	PAD_NC(GPP_A13, NONE),
	/* GPP_A15:     NC */
	PAD_NC(GPP_A15, NONE),

	/* GPP_B */
	/* GPP_B00:     SOC_USBC_SMLCLK */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     SOC_USBC_SMLDATA */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     NC */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03:     NC */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04:     NC */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05:     NC */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B06:     NC */
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07:     NC */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08:     NC */
	PAD_NC(GPP_B08, NONE),
	/* GPP_B09:     NC */
	PAD_NC(GPP_B09, NONE),
	/* GPP_B10:     NC */
	PAD_NC(GPP_B10, NONE),
	/* GPP_B12:     PM_SLP_S0# */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13:     SOC_PLTRST# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14:     SOC_DP2_HPD# */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF2),
	/* GPP_B16:     NC */
	PAD_NC(GPP_B16, NONE),
	/* GPP_B17:     SOC_SPI_TPM_INT# */
	PAD_CFG_GPI_APIC_LOCK(GPP_B17, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_B18:     NC */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     NC */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B20:     NC */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B21:     NC */
	PAD_NC(GPP_B21, NONE),
	/* GPP_B22:     NC */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B23:     NC */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     NC */
	PAD_NC(GPP_B24, NONE),
	/* GPP_B25:     NC */
	PAD_NC(GPP_B25, NONE),

	/* GPP_C */
	/* GPP_C00:     SPD_SOC_SMBCLK */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* GPP_C01:     SPD_SOC_SMBDATA */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
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
	/* GPP_C08:     PM_SLP_S0#_GATE */
	PAD_CFG_GPO(GPP_C08, 1, PLTRST),
	/* GPP_C09:     CLKREQ_PCIE#0_WLAN */
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1),
	/* GPP_C10:     SOC_WIFI_KILL# */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_C11:     CLKREQ_PCIE#2_LAN */
	PAD_CFG_NF(GPP_C11, NONE, DEEP, NF1),
	/* GPP_C12:     CLKREQ_PCIE#3_SSD */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
	/* GPP_C13:     NC */
	PAD_NC(GPP_C13, NONE),
	/* GPP_C14:     NC */
	PAD_NC(GPP_C14, NONE),
	/* GPP_C15:     NC */
	PAD_NC(GPP_C15, NONE),
	/* GPP_C16:     NC */
	PAD_NC(GPP_C16, NONE),
	/* GPP_C17:     NC */
	PAD_NC(GPP_C17, NONE),
	/* GPP_C18:     NC */
	PAD_NC(GPP_C18, NONE),
	/* GPP_C19:     NC */
	PAD_NC(GPP_C19, NONE),
	/* GPP_C22:     SOC_DP2_CTRL_CLK */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     SOC_DP2_CTRL_DATA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),

	/* GPP_D */
	/* GPP_D01:     NC */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     SOC_SPI_WP_H1 */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_D03:     NC */
	PAD_NC(GPP_D03, NONE),
	/* GPP_D05:     NC */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06:     NC */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07:     NC */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08:     NC */
	PAD_NC(GPP_D08, NONE),
	/* GPP_D09:     NC */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10:     NC */
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11:     NC */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12:     NC */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13:     NC */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D16:     SOC_DMIC_CLK1 */
	PAD_CFG_NF(GPP_D16, NONE, DEEP, NF3),
	/* GPP_D17:     SOC_DMIC_DATA1 */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF3),
	/* GPP_D19:     NC */
	PAD_NC(GPP_D19, NONE),
	/* GPP_D21:     NC */
	PAD_NC(GPP_D21, NONE),

	/* GPP_E */
	/* GPP_E01:     NC */
	PAD_NC(GPP_E01, NONE),
	/* GPP_E02:     VR_ALERT#_R */
	PAD_CFG_NF(GPP_E02, NONE, DEEP, NF2),
	/* GPP_E03:     NC */
	PAD_NC(GPP_E03, NONE),
	/* GPP_E05:     NC */
	PAD_NC(GPP_E05, NONE),
	/* GPP_E06:     NC */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     NC */
	PAD_NC(GPP_E07, NONE),
	/* GPP_E08:     SOC_BT_KILL# */
	PAD_CFG_GPO(GPP_E08, 1, DEEP),
	/* GPP_E09:     SOC_USB_OC0# */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     NC */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     NC */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     SOC_THC_I2C_0_SCL */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13:     SOC_THC_I2C_0_SDA */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E14:     NC */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     NC */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     SOC_TCHSCR_RST# */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     NC */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     SOC_TCHSCR_INT# */
	PAD_NC(GPP_E18, NONE),
	/* GPP_E19:     SOC_WL_WAKE# */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20:     SOC_TP_INT# */
	PAD_CFG_GPI_APIC_DRIVER(GPP_E20, NONE, PLTRST, EDGE_SINGLE, INVERT),
	/* GPP_E21:     SOC_I2C_PD_INT# */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     NC */
	PAD_NC(GPP_E22, NONE),

	/* GPP_F */
	/* GPP_F00:     CNV_BRI_CTX_DRX */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     CNV_BRI_CRX_DTX */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     CNV_RGI_CTX_DRX */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     CNV_RGI_CRX_DTX */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CLKREQ_CNV# */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06:     NC */
	PAD_NC(GPP_F06, NONE),
	/* GPP_F07:     NC */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     SOC_TCH_PWR_EN */
	PAD_NC(GPP_F08, NONE),
	/* GPP_F09:     NC */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F10:     EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_F10, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F11:     NC */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12:     NC */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13:     NC */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F14:     NC */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     NC */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     NC */
	PAD_NC(GPP_F16, NONE),
	/* GPP_F17:     NC */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18:     NC */
	PAD_NC(GPP_F18, NONE),
	/* GPP_F19:     NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     CSE_EARLY_SW */
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
	/* GPP_H08:     UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_0_CTXD_DRXD */
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
	/* GPP_H17:     NC */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H18:     NC */
	PAD_NC(GPP_H18, NONE),
	/* GPP_H19:     SOC_I2C_0_SDA */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* GPP_H20:     SOC_I2C_0_SCL */
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),
	/* GPP_H21:     SOC_I2C_1_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_1_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* GPP_H23:     NC */
	PAD_NC(GPP_H23, NONE),
	/* GPP_H24:     NC */
	PAD_NC(GPP_H24, NONE),

	/* GPP_V */
	/* GPP_V00:     BATLOW# */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     BC_ACOK_MCP */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     WAKE# */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     EC_PWRBTN#_OUT_R */
	PAD_CFG_NF(GPP_V03, UP_20K, DEEP, NF1),
	/* GPP_V04:     PM_SLP_S3# */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     PM_SLP_S4# */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     PM_SLP_A# */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     WLAN_SUSCLK */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     NC */
	PAD_NC(GPP_V08, NONE),
	/* GPP_V09:     PM_SLP_S5# */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     LAN_WAKE# */
	PAD_CFG_GPI_IRQ_WAKE(GPP_V10, NONE, PWROK, LEVEL, INVERT),
	/* GPP_V11:     PM_SLP_LAN# */
	PAD_CFG_NF(GPP_V11, NONE, DEEP, NF1),
	/* GPP_V12:     NC */
	PAD_NC(GPP_V12, NONE),
	/* GPP_V13:     GPP_V13_CATERR_N */
	PAD_CFG_NF(GPP_V13, NONE, DEEP, NF1),
	/* GPP_V14:     GPP_V14_FORCEPR_N */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V15:     GPP_V15_THERMTRIP_N */
	PAD_CFG_NF(GPP_V15, NONE, DEEP, NF1),
	/* GPP_V16:     SOC_VCCST_EN */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),

	/* GPP_S00:     SNDW_3_SCL */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01:     SNDW_3_SDA0 */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02:     SNDW_3_SDA1 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S03:     SNDW_3_SDA2 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* GPP_S04:     NC */
	PAD_NC(GPP_S04, NONE),
	/* GPP_S05:     NC */
	PAD_NC(GPP_S05, NONE),
	/* GPP_S06:     NC */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07:     NC */
	PAD_NC(GPP_S07, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_B17:     SOC_SPI_TPM_INT# */
	PAD_CFG_GPI_APIC_LOCK(GPP_B17, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_H08:     UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_0_CTXD_DRXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H21:     SOC_I2C_1_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_1_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
};

/* Pad configuration in romstage */
static const struct pad_config romstage_gpio_table[] = {
	/* GPP_C00:     SPD_SOC_SMBCLK */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* GPP_C01:     SPD_SOC_SMBDATA */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
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
