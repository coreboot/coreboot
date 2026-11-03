/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
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

	/* GPP_A08:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_A11:     NC */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     NC */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     PCIE_LNK_DOWN */
	PAD_CFG_NF_OWNERSHIP(GPP_A13, NONE, DEEP, NF2, ACPI),
	/* GPP_A15:     NC */
	PAD_NC(GPP_A15, NONE),

	/* GPP_B */
	/* GPP_B00:     USBC_SML_CLK_PD */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     USBC_SML_DATA_PD */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     ISH_I2C0_SDA_SNSR_HDR */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03:     ISH_I2C0_SCL_SNSR_HDR */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04:     ISH_GP_0_SNSR_HDR */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B06:     MEM_ID2 */
	PAD_CFG_GPI(GPP_B06, NONE, DEEP),
	/* GPP_B07:     BEEP_IN (reserved) */
	PAD_CFG_GPI(GPP_B07, NONE, DEEP),
	/* GPP_B08:     MEM_ID3 */
	PAD_CFG_GPI(GPP_B08, NONE, DEEP),
	/* GPP_B09:     BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_B09, 1, DEEP),
	/* GPP_B10:     NC */
	PAD_NC(GPP_B10, NONE),
	/* GPP_B12:     PM_SLP_S0_N */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13:     PLT_RST_N */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B16:     COINLESS_MODE_SELECT */
	PAD_CFG_GPI(GPP_B16, NONE, DEEP),
	/* GPP_B17:     Test point; no GSC interrupt */
	PAD_NC(GPP_B17, NONE),
	/* GPP_B21:     NC (no external USB4 retimer) */
	PAD_NC(GPP_B21, NONE),
	/* GPP_B22:     EC_NMI#_R (reserved) */
	PAD_CFG_GPI(GPP_B22, NONE, DEEP),
	/* GPP_B23:     Strap */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     ESPI_ALERT0_EC_R_N */
	PAD_NC(GPP_B24, NONE),
	/* GPP_B25:     None */
	PAD_NC(GPP_B25, NONE),

	/* GPP_C */
	/* GPP_C00:     SPD_SMB_CLK */
	PAD_NC(GPP_C00, NONE),
	/* GPP_C01:     SPD_SMB_DATA */
	PAD_NC(GPP_C01, NONE),
	/* GPP_C02:     NC */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03:     TCP_LAN_SML0_SCL_R */
	PAD_CFG_NF(GPP_C03, NONE, DEEP, NF1),
	/* GPP_C04:     TCP_LAN_SML0_SDA_R */
	PAD_CFG_NF(GPP_C04, NONE, DEEP, NF1),
	/* GPP_C05:     NC */
	PAD_NC(GPP_C05, NONE),
	/* GPP_C06:     NC */
	PAD_NC(GPP_C06, NONE),
	/* GPP_C07:     NC */
	PAD_NC(GPP_C07, NONE),
	/* GPP_C08:     PM_SLP_S0_N_GPP_CNTRL */
	PAD_CFG_GPO(GPP_C08, 1, PLTRST),
	/* GPP_C09:     NC */
	PAD_NC(GPP_C09, NONE),
	/* GPP_C10:     WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_C10, 1, DEEP),
	/* GPP_C12:     CLKREQ3_X4_GEN4_M2_SSD_N */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
	/* GPP_C13:     NC */
	PAD_NC(GPP_C13, NONE),
	/* GPP_C14:     NC */
	PAD_NC(GPP_C14, NONE),
	/* GPP_C16:     MOD_TCSS1_LS_TX_DDC_SCL */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17:     MOD_TCSS1_LS_RX_DDC_SDA */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18:     MOD_TCSS2_LS_TX_DDC_SCL */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* GPP_C19:     MOD_TCSS2_LS_RX_DDC_SDA */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),

	/* GPP_D */
	/* GPP_D01:     NC (no SD) */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     SOC_WP_OD */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_D05:     disable ISH_UART0_RXD */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D07:     Board_ID0 */
	PAD_CFG_GPI(GPP_D07, NONE, DEEP),
	/* GPP_D08:     Board_ID1 */
	PAD_CFG_GPI(GPP_D08, NONE, DEEP),
	/* GPP_D09:     NC */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10:     HDA_BCLK (HDR) */
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11:     HDA_SYNC (HDR) */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12:     STRAP_HDA_SDO */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13:     HDA_SDI0 (HDR) */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D16:     HDA_RST_N (HDR) */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     HDA_SDI1 (HDR) */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D19:     SOC_TCHSCR_REPORT_EN (disabled) */
	PAD_NC(GPP_D19, NONE),
	/* GPP_D21:     NC (no UFS) */
	PAD_NC(GPP_D21, NONE),

	/* GPP_E */
	/* GPP_E06:     Strap */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     NC (no PCIe WLAN) */
	PAD_NC(GPP_E07, NONE),
	/* GPP_E08:     EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E08, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E09:     USB_FP_CONN_1_CONN_2_OC0_N */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     NC (no SD) */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     NC (no SPI fingerprint) */
	PAD_NC(GPP_E11, NONE),
	/* Touchpad interface disabled until its controller configuration is defined. */
	/* GPP_E12:     TPAD_I2C1_CLK */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13:     TPAD_I2C1_SDA */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E14:     GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E14, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E15:     Test point */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     TPAD_RST_N (not used) */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     NC (no SPI fingerprint) */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     TCHPAD_INT_N */
	PAD_NC(GPP_E18, NONE),
	/* GPP_E21:     I2C_PMC_PD_INT_N */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     NC */
	PAD_NC(GPP_E22, NONE),

	/* GPP_F */
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
	/* GPP_F06:     NC */
	PAD_NC(GPP_F06, NONE),
	/* GPP_F07:     NC */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     EN_TCHSCR_PWR */
	PAD_CFG_GPO(GPP_F08, 0, PLTRST),
	/* GPP_F09:     NC (no UFS) */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F11:     NC (no SPI touch) */
	PAD_NC(GPP_F11, NONE),
	/* Touchscreen interface disabled until its controller configuration is defined. */
	/* GPP_F12:     TP_I2C0_SCL */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13:     TP_I2C0_SDA */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F14:     NC (no SPI fingerprint) */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     NC (no SPI fingerprint) */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     TP_I2C0_RST */
	PAD_NC(GPP_F16, NONE),
	/* GPP_F17:     NC */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18:     TP_I2C0_INT# */
	PAD_NC(GPP_F18, NONE),
	/* GPP_F19:     NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     CSE_EARLY_SW */
	PAD_CFG_GPI(GPP_F20, NONE, DEEP),
	/* GPP_F22:     TP_ISH_GP8 */
	PAD_NC(GPP_F22, NONE),

	/* GPP_H */
	/* GPP_H00:     NC */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H02:     Strap */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H03:     NC */
	PAD_NC(GPP_H03, NONE),
	/* GPP_H04:     CNV_MFUART2_RXD */
	PAD_CFG_NF(GPP_H04, NONE, DEEP, NF1),
	/* GPP_H05:     CNV_MFUART2_TXD */
	PAD_CFG_NF(GPP_H05, NONE, DEEP, NF1),
	/* GPP_H06:     NC */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07:     NC */
	PAD_NC(GPP_H07, NONE),
	/* GPP_H08:     SOC_BIOS_LOG_TTK_UART_RX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     SOC_BIOS_LOG_TTK_UART_TX */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10:     UART0_BUF_RTS */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF1),
	/* GPP_H11:     UART0_BUF_CTS */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF1),
	/* GPP_H13:     CPU_C10_GATE_N_R */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* GPP_H14:     Board_ID2 */
	PAD_CFG_GPI(GPP_H14, NONE, DEEP),
	/* GPP_H15:     ISH_I3C1_SCL_SNSR_HDR */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H17:     NC */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H19:     I3C0_SDA_HDR */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF2),
	/* GPP_H20:     I3C0_SCL_HDR */
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF2),
	/* GPP_H21:     I2C1_SDA_TTK_CHROME */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     I2C1_SCL_TTK_CHROME */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* GPP_H23:     MEM_ID0 */
	PAD_CFG_GPI(GPP_H23, NONE, DEEP),
	/* GPP_H24:     MEM_ID1 */
	PAD_CFG_GPI(GPP_H24, NONE, DEEP),

	/* GPP_S */
	/* GPP_S06:     NC */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07:     NC */
	PAD_NC(GPP_S07, NONE),

	/* GPP_V */
	/* GPP_V00:     PM_BATLOW_N */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     BC_ACOK_MCP */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     LANWAKE_N_R */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     PWRBTN_MCP_N */
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1),
	/* GPP_V04:     PM_SLP_S3_N */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     PM_SLP_S4_N */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     PM_SLP_A_N */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     M.2_BTWIFI_SUS_CLK_LS */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     SLP_WLAN_N */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09:     PM_SLP_S5_N */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     LANPHYPC_R_N */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),
	/* GPP_V11:     PM_SLP_LAN_N */
	PAD_CFG_NF(GPP_V11, NONE, DEEP, NF1),
	/* GPP_V12:     WAKE_N */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V13:     GPP_V13_CATERR_N */
	PAD_CFG_NF(GPP_V13, NONE, DEEP, NF1),
	/* GPP_V14:     GPP_V14_FORCEPR_N */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V15:     GPP_V15_THERMTRIP_N */
	PAD_CFG_NF(GPP_V15, NONE, DEEP, NF1),
	/* GPP_V16:     GPP_V16_VCCST_EN */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),

	PAD_NC(GPP_VGPIO3_THC0, NONE),
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* Memory and board straps: leave levels to the external straps. */
	PAD_CFG_GPI(GPP_H23, NONE, DEEP), /* MEM_ID0 */
	PAD_CFG_GPI(GPP_H24, NONE, DEEP), /* MEM_ID1 */
	PAD_CFG_GPI(GPP_B06, NONE, DEEP), /* MEM_ID2 */
	PAD_CFG_GPI(GPP_B08, NONE, DEEP), /* MEM_ID3 */
	PAD_CFG_GPI(GPP_D07, NONE, DEEP), /* Board_ID0 */
	PAD_CFG_GPI(GPP_D08, NONE, DEEP), /* Board_ID1 */
	PAD_CFG_GPI(GPP_H14, NONE, DEEP), /* Board_ID2 */

	/* GPP_D02:     SOC_WP_OD */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_B14:     GPP_B14_DDSP_HPDB */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF2),
	/* GPP_E14:     GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E14, NONE, DEEP, LEVEL, INVERT),
	/* GPP_C22:     DDPB_HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     DDPB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),
	/* GPP_H06:     NC */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07:     NC */
	PAD_NC(GPP_H07, NONE),
	/* GPP_H08:     SOC_BIOS_LOG_TTK_UART_RX */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     SOC_BIOS_LOG_TTK_UART_TX */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H21:     I2C1_SDA_TTK_CHROME */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     I2C1_SCL_TTK_CHROME */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 0, PLTRST),
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

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE0_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE1_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE2_NAME),
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE3_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE4_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
