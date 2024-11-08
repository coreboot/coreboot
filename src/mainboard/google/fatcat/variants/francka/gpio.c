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

	/* GPP_A08:     SOC_SD_PWR_EN */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_A09:     SOC_WWAN_OFF#_SW */
	PAD_CFG_GPO(GPP_A09, 1, PLTRST),
	/* GPP_A10:     SOC_WWAN_RA_DIS#_SW */
	PAD_CFG_GPO(GPP_A10, 1, PLTRST),
	/* GPP_A11:     Not used */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     Not used */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13:     Not used */
	PAD_NC(GPP_A13, NONE),
	/* GPP_A15:     Not used */
	PAD_NC(GPP_A15, NONE),
	/* GPP_A16:     SOC_BT_ON */
	PAD_CFG_GPO(GPP_A16, 1, DEEP),
	/* GPP_A17:     SOC_WL_OFF# */
	PAD_CFG_GPO(GPP_A17, 1, DEEP),

	/* GPP_B00:     SOC_USBC_SMLCLK */
	PAD_CFG_NF(GPP_B00, NONE, DEEP, NF1),
	/* GPP_B01:     SOC_USBC_SMLDATA */
	PAD_CFG_NF(GPP_B01, NONE, DEEP, NF1),
	/* GPP_B02:     ISH_I2C_0_SDA */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B02, NONE, DEEP, NF3),
	/* GPP_B03:     ISH_I2C_0_SCL */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B03, NONE, DEEP, NF3),
	/* GPP_B04:     Not used */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05:     SOC_EC_INT# */
	PAD_CFG_GPI_APIC(GPP_B05, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_B06:     Not used */
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07:     SOC_CODEC_RST# */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08:     SOC_TP_RST#1 */
	PAD_CFG_GPO(GPP_B08, 1, PLTRST),
	/* GPP_B09:     SOC_TCP0_DP_HPD# */
	PAD_CFG_NF(GPP_B09, NONE, DEEP, NF2),
	/* GPP_B10:     SOC_TCP1_DP_HPD# */
	PAD_CFG_NF(GPP_B10, NONE, DEEP, NF2),
	/* GPP_B11:     SOC_RTS_P0_P1_OC# */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* GPP_B12:     PM_SLP_S0# */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13:     SOC_PLTRST# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14:     Not used */
	PAD_NC(GPP_B14, NONE),
	/* GPP_B15:     Not used */
	PAD_NC(GPP_B15, NONE),
	/* GPP_B16:     SOC_SSD2_EN */
	PAD_CFG_GPO(GPP_B16, 1, PLTRST),
	/* GPP_B17:     SOC_ENVDD2 */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF2),
	/* GPP_B18:     TCHSCR_REPORT_DISABLE */
	PAD_CFG_GPO(GPP_B18, 0, PLTRST),
	/* GPP_B19:     SOC_CNVI_EN# */
	PAD_CFG_GPO(GPP_B19, 0, PLTRST),
	/* GPP_B20:     SOC_WWAN_RST# */
	PAD_CFG_GPO(GPP_B20, 1, PLTRST),
	/* GPP_B21:     SOC_RT_FORCE_PWR */
	PAD_CFG_GPO(GPP_B21, 0, DEEP),
	/* GPP_B22:     SEN_MODE_EC_PCH_INT_OD# */
	PAD_CFG_NF(GPP_B22, NONE, DEEP, NF4),
	/* GPP_B23:     Not used */
	PAD_NC(GPP_B23, NONE),
	/* GPP_B24:     Not used */
	PAD_NC(GPP_B24, NONE),
	/* GPP_B25:     SOC_SD_WAKE# */
	PAD_CFG_GPI_SCI_LOW(GPP_B25, NONE, DEEP, LEVEL),

	/* GPP_C00:     SOC_SMBCLK */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* GPP_C01:     SOC_SMBDATA */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
	/* GPP_C02:     Not used */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03:     SOC_SML0CLK */
	PAD_CFG_NF(GPP_C03, NONE, DEEP, NF1),
	/* GPP_C04:     SOC_SML0DATA */
	PAD_CFG_NF(GPP_C04, NONE, DEEP, NF1),
	/* GPP_C05:     Not used */
	PAD_NC(GPP_C05, NONE),
	/* GPP_C06:     Not used */
	PAD_NC(GPP_C06, NONE),
	/* GPP_C07:     Not used */
	PAD_NC(GPP_C07, NONE),
	/* GPP_C08:     Not used */
	PAD_NC(GPP_C08, NONE),
	/* GPP_C09:     Not used */
	PAD_NC(GPP_C09, NONE),
	/* GPP_C10:     CLKREQ_PCIE#1 */
	PAD_CFG_NF(GPP_C10, NONE, DEEP, NF1),
	/* GPP_C11:     CLKREQ_PCIE#2 */
	PAD_CFG_NF(GPP_C11, NONE, DEEP, NF1),
	/* GPP_C12:     Not used */
	PAD_NC(GPP_C12, NONE),
	/* GPP_C13:     CLKREQ_PCIE#4 */
	PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1),
	/* GPP_C14:     CLKREQ_PCIE#5 */
	PAD_CFG_NF(GPP_C14, NONE, DEEP, NF1),
	/* GPP_C15:     Not used */
	PAD_NC(GPP_C15, NONE),
	/* GPP_C16:     TBT_0_LSX_TX  */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17:     TBT_0_LSX_RX  */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18:     TBT_1_LSX_TX  */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* GPP_C19:     TBT_2_LSX_RX */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* GPP_C20:     TBT_2_LSX_TX */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* GPP_C21:     TBT_2_LSX_RX */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* GPP_C22:     TBT_3_LSX_TX */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),
	/* GPP_C23:     TBT_3_LSX_RX */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),

	/* GPP_D00:     SOC_SD_PRSNT# */
	PAD_CFG_GPO(GPP_D00, 1, PLTRST),
	/* GPP_D01:     Not used */
	PAD_NC(GPP_D01, NONE),
	/* GPP_D02:     Not used */
	PAD_NC(GPP_D02, NONE),
	/* GPP_D03:     SOC_WWAN_PCIE_RST# */
	PAD_CFG_GPO(GPP_D03, 1, PLTRST),
	/* GPP_D04:     IPCM_SLOW */
	PAD_CFG_GPO(GPP_D04, 1, PLTRST),
	/* GPP_D05:     Not used */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06:     Not used */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07:     DDR_STRAP_2 */
	PAD_CFG_GPI(GPP_D07, NONE, DEEP),
	/* GPP_D08:     DDR_STRAP_1 */
	PAD_CFG_GPI(GPP_D08, NONE, DEEP),
	/* GPP_D09:     PCH_DGPU_HOLD_RST#_R */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10:     HDA_BIT_CLK */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF1),
	/* GPP_D11:     HDA_SYNC */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF1),
	/* GPP_D12:     HDA_SDOUT */
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF1),
	/* GPP_D13:     HDA_SDIN0 */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* GPP_D14:     SOC_HDMI_STRAP */
	PAD_CFG_GPI(GPP_D14, NONE, DEEP),
	/* GPP_D15:     Not used */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D16:     HDA_RST# */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17:     FPMCU_INT# */
	PAD_CFG_GPI_INT(GPP_D17, NONE, PLTRST, LEVEL),
	/* GPP_D18:     CLKREQ_PCIE#6 */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	/* GPP_D19:     SOC_SD_RST# */
	PAD_CFG_GPO(GPP_D19, 1, PLTRST),
	/* GPP_D20:     CSE_EARLY_SW */
	PAD_CFG_GPI_SCI_HIGH(GPP_D20, NONE, DEEP, LEVEL),
	/* GPP_D21:     Not used */
	PAD_NC(GPP_D21, NONE),
	/* GPP_D22:     SOC_I3C_BPK_SDA */
	PAD_CFG_NF(GPP_D22, NONE, DEEP, NF2),
	/* GPP_D23:     SOC_I3C_BPK_SCL */
	PAD_CFG_NF(GPP_D23, NONE, DEEP, NF2),
	/* GPP_D24:     Not used */
	PAD_NC(GPP_D24, NONE),
	/* GPP_D25:     Not used */
	PAD_NC(GPP_D25, NONE),

	/* GPP_E01:     SOC_TP_INT#1 */
	PAD_CFG_GPI_APIC(GPP_E01, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_E02:     SOC_WWAN_WAKE2#_R  */
	PAD_CFG_GPI_SCI_LOW(GPP_E02, NONE, DEEP, LEVEL),
	/* GPP_E03:     SOC_SSD2_RST# */
	PAD_CFG_GPO(GPP_E03, 1, PLTRST),
	/* GPP_E05:     Not used */
	PAD_NC(GPP_E05, NONE),
	/* GPP_E06:     Not used */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07:     Not used */
	PAD_NC(GPP_E07, NONE),
	/* GPP_E08:     Not used*/
	PAD_NC(GPP_E08, NONE),
	/* GPP_E09:     USB_OC0# */
	PAD_CFG_NF(GPP_E09, NONE, DEEP, NF1),
	/* GPP_E10:     Not used */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11:     Not used */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     SOC_THC_I2C_0_SCL */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),
	/* GPP_E13:     SOC_THC_I2C_0_SDA */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),
	/* GPP_E14:     Not used */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     Not used */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16:     Not used */
	/* THC NOTE: use GPO instead of GPO for THC0 Rst */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     Not used */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     SOC_TP_INT# */
	PAD_CFG_GPI_APIC(GPP_E18, NONE, PLTRST, EDGE_SINGLE, INVERT),
	/* GPP_E19:     Not used */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20:     Not used# */
	PAD_NC(GPP_E20, NONE),
	/* GPP_E21:     SOC_I2C_PD_INT# */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),
	/* GPP_E22:     SOC_WP_OD */
	PAD_CFG_GPI(GPP_E22, NONE, DEEP),

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
	/* GPP_F06:     COEX3 */
	PAD_CFG_NF(GPP_F06, NONE, DEEP, NF1),
	/* GPP_F07:     Not used */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08:     Not used */
	PAD_NC(GPP_F08, NONE),
	/* GPP_F09:     SX_EXIT_HOLDOFF# */
	PAD_CFG_NF(GPP_F09, NONE, DEEP, NF2),
	/* GPP_F10:     Not used */
	PAD_NC(GPP_F10, NONE),
	/* GPP_F11:     SOC_THC_1_CLK */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF5),
	/* GPP_F12:     SOC_THC_1_D0 */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF5),
	/* GPP_F13:     SOC_THC_1_D1 */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF5),
	/* GPP_F14:     Not used */
	PAD_NC(GPP_F14, NONE),
	/* GPP_F15:     Not used */
	PAD_NC(GPP_F15, NONE),
	/* GPP_F16:     SOC_THC_1_RST# */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),
	/* GPP_F17:     SOC_THC_1_CS# */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF5),
	/* GPP_F18:     SOC_THC_1_INT# */
	PAD_CFG_GPI_APIC(GPP_F18, NONE, PWROK, LEVEL, INVERT),
	/* GPP_F19:     Not used */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20:     AP_FP_FW_UP_STRAP */
	PAD_CFG_GPO(GPP_F20, 1, DEEP),
	/* GPP_F22:     Not used */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23:     SLP_S0#_GATE */
	PAD_CFG_GPO(GPP_F23, 1, PLTRST),

	/* GPP_H00:     Not used */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H01:     Not used */
	PAD_NC(GPP_H01, NONE),
	/* GPP_H02:     Not used */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H03:     EN_PWR_FP */
	PAD_CFG_GPO(GPP_H03, 0, DEEP),
	/* GPP_H04:     COEX1 */
	PAD_CFG_NF(GPP_H04, NONE, DEEP, NF2),
	/* GPP_H05:     COEX2 */
	PAD_CFG_NF(GPP_H05, NONE, DEEP, NF2),
	/* GPP_H06:     Not used */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07:     Not used */
	PAD_NC(GPP_H07, NONE),
	/* GPP_H08:     UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_0_CTXD_DRXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10:     Not used */
	PAD_NC(GPP_H10, NONE),
	/* GPP_H11:     GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_H11, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_H13:     CPU_C10_GATE# */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* GPP_H14:     ISH_I2C_1_SDA */
	PAD_CFG_NF(GPP_H14, NONE, DEEP, NF2),
	/* GPP_H15:     ISH_I2C_1_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF2),
	/* GPP_H16:     SOC_AUDIO_STRAP */
	PAD_CFG_GPI(GPP_H16, NONE, DEEP),
	/* GPP_H17:     FP_RST_1V8_OD# */
	PAD_CFG_GPO(GPP_H17, 1, DEEP),
	/* GPP_H19:     Not used*/
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20:     Not used */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21:     SOC_I2C_1_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22:     SOC_I2C_1_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),

	/* GPP_S00:     SNDW_3_SCL */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01:     SNDW_3_SDA */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02:     SOC_DMIC_CLK0 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* GPP_S03:     SOC_DMIC_DATA0 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* GPP_S04:     SNDW2_CLK */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF2),
	/* GPP_S05:     SNDW2_DATA0 */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF2),
	/* GPP_S06:     SOC_DMIC_CLK1 */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF5),
	/* GPP_S07:     SOC_DMIC_DATA1 */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF5),

	/* GPP_V00:     PM_BATLOW# */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01:     AC_PRESENT */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02:     LAN_WAKE# */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03:     PBTN_OUT# */
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1),
	/* GPP_V04:     PM_SLP_S3# */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05:     PM_SLP_S4# */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06:     PM_SLP_A# */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V07:     SUSCLK */
	PAD_CFG_NF(GPP_V07, NONE, DEEP, NF1),
	/* GPP_V08:     SLP_WLAN# */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09:     PM_SLP_S5# */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10:     Not used  */
	PAD_NC(GPP_V10, NONE),
	/* GPP_V11:     PM_SLP_LAN# */
	PAD_NC(GPP_V11, NONE),
	/* GPP_V12:     WAKE# */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V13:     GPP_V13_CATERR_N */
	PAD_CFG_NF(GPP_V13, NONE, DEEP, NF1),
	/* GPP_V14:     GPP_V14_FORCEPR_N */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V15:     GPP_V15_THERMTRIP_N */
	PAD_CFG_NF(GPP_V15, NONE, DEEP, NF1),
	/* GPP_V16:     VCCST_EN */
	PAD_CFG_NF(GPP_V16, NONE, DEEP, NF1),
	/* GPP_V17:     Not used */
	PAD_NC(GPP_V17, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_H08:     UART_0_CRXD_DTXD */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09:     UART_0_CTXD_DRXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),

	/* GPP_H06: SOC_I2C_3_SDA */
	PAD_CFG_NF(GPP_H06, NONE, DEEP, NF1),
	/* GPP_H07: SOC_I2C_3_SCL */
	PAD_CFG_NF(GPP_H07, NONE, DEEP, NF1),
	/* GPP_D15: GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_H11, NONE, LEVEL, INVERT, LOCK_CONFIG),
};

/* Pad configuration in romstage */
static const struct pad_config romstage_gpio_table[] = {
	/* GPP_C00:     SOC_SMBCLK */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),
	/* GPP_C01:     SOC_SMBDATA */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),
	/* GPP_F16:     SOC_THC_1_RST# */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),
	/* GPP_H03:     EN_PWR_FP */
	PAD_CFG_GPO(GPP_H03, 0, DEEP),
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
};

DECLARE_CROS_GPIOS(cros_gpios);
