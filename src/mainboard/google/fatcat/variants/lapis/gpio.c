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

	/* GPP_A08:     Not used */
	PAD_NC(GPP_A08, NONE),
	/* GPP_A09:     Not used */
	PAD_NC(GPP_A09, NONE),
	/* GPP_A10:     Not used */
	PAD_NC(GPP_A10, NONE),
	/* GPP_A11:     Not used */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     WIFI_WAKE_N ==> TOUCHPANEL_SYNC */ /* NC */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_A13, NONE, DEEP),
	/* GPP_A15:     Not used */
	PAD_NC(GPP_A15, NONE),
	/* GPP_A16:     BT_DISABLE_L ==> BT_ON_WLAN */
	PAD_CFG_GPO(GPP_A16, 1, DEEP),
	/* GPP_A17:     WIFI_DISABLE_L ==> WLAN_ON_PCH */
	PAD_CFG_GPO(GPP_A17, 1, DEEP),

	/* GPP_B00:     PMC_I2C_PD_SCL ==> PD_SMB1_CLK */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     PMC_I2C_PD_SDA ==> PD_SMB1_DATA */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     ISH_I2C0_SDA_SNSR_HDR ==> NC  */ /* may be connect to EC */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03:     ISH_I2C0_SCL_SNSR_HDR ==> NC  */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04:     ISH_GP_0_SNSR_HDR ==> NC*/ /* maybe disable ISH */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05:     ISH_GP_1_SNSR_HDR ==>  NC */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B06:     ISH_GP_2_SNSR_HDR ==> NC*/
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07:     ISH_GP_3_SNSR_HDR ==> NC */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08:     ISH_GP_4_SNSR_HDR ==> GPP_B08_TBT_LOW_POWER_MODE */
	PAD_CFG_NF(GPP_B08, NONE, DEEP, NF4),
	/* GPP_B09:     M2_GEN5_SSD_RESET_N ==> SSD_GEN5_PERST_L */
	PAD_CFG_GPO(GPP_B09, 1, PLTRST),
	/* GPP_B10:     GEN4_SSD_PWREN */ /* may be NC */
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF2),
	/* GPP_B11:     MOD_TCSS1_DISP_HPD3 ==> USB_OC1 */ /* may be change to USB_OC1 */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF2),
	/* GPP_B12:     PM_SLP_S0_N ==> PCH_SLP_S0# */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13:     PLT_RST_N */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14:     MOD_TCSS2_DISP_HPD4 ==> HDMI_HPD_CPU# */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF2),
	/* GPP_B15:     Not used */
	PAD_NC(GPP_B15, NONE),
	/* GPP_B16:     EN_PP3300_SSD ==> EN_PP3300_SSD_GEN5 */
	PAD_CFG_GPO(GPP_B16, 1, PLTRST),
	/* GPP_B17:     Not used ==> GPP_B17_TBT_LOW_POWER_MODE */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF4),
	/* GPP_B18:     Not used */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19:     Not used */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B20:     Not used */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B21:     USB4_RT_FORCE_PWR ==> TBT_FORCE_PWR */
	PAD_CFG_GPO(GPP_B21, 0, DEEP),
	/* GPP_B22:     Not used */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B17:     Not used */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_B24, NONE, DEEP),
	/* GPP_B25:     DIMM_SEL2 */
	PAD_CFG_GPI(GPP_B25, NONE, DEEP),

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
	/* GPP_C06:     CAM_PWR_EN */ /* TBD */
	PAD_CFG_GPO(GPP_C06, 1, DEEP),
	/* GPP_C07:     MEM_CH_SEL */
	PAD_CFG_GPI(GPP_C07, NONE, DEEP),
	/* GPP_C08:     Not used */
	PAD_NC(GPP_C08, NONE),
	/* GPP_C09:     SSD_GEN5_CLKREQ_ODL ==> CLKREQ0_SSD1# */
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1),
	/* GPP_C10:     Not used */
	PAD_NC(GPP_C10, NONE),
	/* GPP_C11:     Not used */
	PAD_NC(GPP_C11, NONE),
	/* GPP_C12:     Not used */
	PAD_NC(GPP_C12, NONE),
	/* GPP_C13:     Not used */
	PAD_NC(GPP_C13, NONE),
	/* GPP_C14:     Not used */
	PAD_NC(GPP_C14, NONE),
	/* GPP_C15:     FP_RST_N */
	PAD_CFG_GPO(GPP_C15, 1, DEEP),
	/* GPP_C16:     USB_C0_LSX_TX  ==> TBT_LSX0_TXD */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17:     USB_C0_LSX_RX  ==> TBT_LSX0_RXD */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18:     Not used */
	PAD_NC(GPP_C18, NONE),
	/* GPP_C19:     Not used */
	PAD_NC(GPP_C19, NONE),
	/* GPP_C20:     USB_C1_LSX_TX ==> TBT_LSX2_TXD */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* GPP_C21:     USB_C1_LSX_RX ==> TBT_LSX2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* GPP_C22:     DDP3_HDMI_CTRLCLK ==> HDMI_X1_CLK*/
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF2),
	/* GPP_C23:     DDP3_HDMI_CTRLDATA ==> HDMI_X1_DATA */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF2),

	/* GPP_D00:     Not used */
	PAD_NC(GPP_D00, NONE),
	/* GPP_D01:     Not used */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     SOC_WP_OD ==> FLASH_WP_CPU */
	PAD_CFG_GPI(GPP_D02, NONE, DEEP),
	/* GPP_D03:     Not used */
	PAD_NC(GPP_D03, NONE),
	/* GPP_D04:     Not used */
	PAD_NC(GPP_D04, NONE),
	/* GPP_D05:     Not used  */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06:     Not used  */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07:     Not used */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08:     Not used */
	PAD_NC(GPP_D08, NONE),
	/* GPP_D09:     Not used ==> CPU_ID0 */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10:     Not used ==> CPU_ID1*/
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11:     Not used ==> CPU_ID2*/
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12:     Not used ==> HDA_SDO_PCH */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13:     Not used ==> HDA_SDI_PCH */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D14:     Not used */
	PAD_NC(GPP_D14, NONE),
	/* GPP_D15:     Not used */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D16:     Not used  */ /* DMIC connect to Audio Codec */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     Not used  */ /* DMIC connect to Audio Codec */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D18:     Not used */
	PAD_NC(GPP_D18, NONE),
	/* GPP_D19:     Not used */
	PAD_NC(GPP_D19, NONE),
	/* GPP_D20:     EC_SOC_REC_SWITCH_ODL */
	PAD_CFG_GPI(GPP_D20, NONE, DEEP),
	/* GPP_D21:     Not used */
	PAD_NC(GPP_D21, NONE),
	/* GPP_D22:     Not used ==> VPRO_ID0 */
	PAD_NC(GPP_D22, NONE),
	/* GPP_D23:     Not used ==> VPRO_ID1 */
	PAD_NC(GPP_D23, NONE),
	/* GPP_D24:     DIMM_SEL1 */
	PAD_CFG_GPI(GPP_D24, NONE, DEEP),
	/* GPP_D25:     DIMM_SEL0 */
	PAD_CFG_GPI(GPP_D25, NONE, DEEP),

	/* GPP_E01:     Not used */
	PAD_NC(GPP_E01, NONE),
	/* GPP_E02:     GSC_SOC_INT_ODL  */ /* locked */
	/* PAD_CFG_GPI_APIC_LOCK(GPP_E02, NONE, LEVEL, INVERT, LOCK_CONFIG), */
	/* GPP_E03:     Not used */
	PAD_NC(GPP_E03, NONE),
	/* GPP_E06:     Not used */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E07, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E08:     Not used */
	PAD_NC(GPP_E08, NONE),
	/* GPP_E09:     USBA0_OC_ODL ==> USB_OC0# */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     Not used */ /* TPM */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     EN_TCHSCR_PWR ==> TOUCH_ENABLE */
	PAD_CFG_GPO(GPP_E11, 1, PLTRST),
	/* GPP_E12:     SOC_I2C_TCHPAD_SCL ==> THC_I2C0_SCL_TOUCH_EDP */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* GPP_E13:     SOC_I2C_TCHPAD_SDA ==> THC_I2C0_SDA_TOUCH_EDP*/
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* GPP_E14:     Not used */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     Not used */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     Not used */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     Not used ==> TOUCHPAD_INTR */
	PAD_CFG_GPI_APIC(GPP_E17, NONE, PLTRST, LEVEL, NONE),
	/* GPP_E18:     TCHPAD_INT_ODL_LS ==> Not used */
	PAD_NC(GPP_E18, NONE),
	/* GPP_E20:     Not used ==> PANEL_ID1 */
	PAD_NC(GPP_E20, NONE),
	/* GPP_E21:     PMC_I2C_PD_INT_ODL ==> PD_SMB1_INT# */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     Not used */
	PAD_NC(GPP_E22, NONE),

	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CRF_CLKREQ_R */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06:     Not used */
	PAD_NC(GPP_F06, NONE),
	/* GPP_F07:     Not used */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     Not used */
	PAD_NC(GPP_F08, NONE),
	/* GPP_F09:     NC */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F10:     NC */
	PAD_NC(GPP_F10, NONE),
	/* GPP_F11:     NC */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12:     THC_I2C1_SCL_TCH_PAD */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF8),
	/* GPP_F13:     THC_I2C1_SDA_TCH_PAD */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF8),
	/* GPP_F14:     NC */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     NC */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     THC0_SPI1_RST_N_TCH_PNL1 */
	/* THC NOTE: use GPO instead of GPO for THC0 Rst */
	PAD_CFG_GPO(GPP_F16, 1, DEEP),
	/* GPP_F17:     CODEC_INT_N ==> NC */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18:     SOC_TCHSCR_INT */
	PAD_CFG_GPI_APIC(GPP_F18, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F19:     NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     NC */
	PAD_NC(GPP_F20, NONE),
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
	/* GPP_H04:     Not used */
	PAD_NC(GPP_H04, NONE),
	/* GPP_H05:     Not used */
	PAD_NC(GPP_H05, NONE),
	/* GPP_H06:     SOC_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H06, NONE, DEEP, NF1),
	/* GPP_H07:     SOC_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H07, NONE, DEEP, NF1),
	/* GPP_H08:     UART_DBG_TX_SOC_RX_U */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_SOC_TX_DBG_RX_U */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10:     Not used */
	PAD_NC(GPP_H10, NONE),
	/* GPP_H11:     Not used */
	PAD_NC(GPP_H11, NONE),
	/* GPP_H13:     CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* GPP_H14:     NC */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15:     NC */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H16:     NC */
	PAD_NC(GPP_H16, NONE),
	/* GPP_H17:     NC */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H19:     NC */
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20:     NC */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21:     NC */ /* Audio Codec */
	PAD_NC(GPP_H21, NONE),
	/* GPP_H22:     NC */ /* Audio Codec */
	PAD_NC(GPP_H22, NONE),

	/* GPP_S00:     SNDW3_CLK_CODEC */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01:     SNDW3_DATA0_CODEC */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02:     SNDW3_DATA1_CODEC */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S03:     NC */
	PAD_NC(GPP_S03, NONE),
	/* GPP_S04:     SNDW2_CLK */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF2),
	/* GPP_S05:     SNDW2_DATA0 */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF2),
	/* GPP_S06:     SNDW1_CLK */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07:     SNDW1_DATA0 */
	PAD_NC(GPP_S07, NONE),

	/* GPP_V00:     BATLOW_L */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     ACPRESENT */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     LANWAKE_N_R */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     EC_SOC_PWR_BTN_ODL */
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1),
	/* GPP_V04:     PM_SLP_S3_N */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     PM_SLP_S4_N */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     PM_SLP_A_N */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     SUS_CLK */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     SLP_WLAN_N */ /* NC */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09:     PM_SLP_S5_N */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     LANPHYPC_R_N */ /* NC */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),
	/* GPP_V11:     PM_SLP_LAN_N */ /* NC */
	PAD_CFG_NF(GPP_V11, NONE, DEEP, NF1),
	/* GPP_V12:     SOC_WAKE_L */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V16:     EN_VCCST */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),
	/* GPP_V17:     SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_V17, 1, DEEP),

	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC0, NONE, PLTRST, LEVEL, NONE),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC1, NONE, PLTRST, LEVEL, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_C15:     FP_RST_N */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),

	/* GPP_E19:     FP_PWR_EN */
	PAD_CFG_GPO(GPP_E19, 1, PLTRST),

	/* GPP_H08:     UART0_BUF_RXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART0_BUF_TXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),

	/* GPP_H06: SOC_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H06, NONE, DEEP, NF1),
	/* GPP_H07: SOC_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H07, NONE, DEEP, NF1),
	/* GPP_E02:     GSC_SOC_INT_ODL  */
	PAD_CFG_GPI_APIC_LOCK(GPP_E02, NONE, LEVEL, INVERT, LOCK_CONFIG),
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
