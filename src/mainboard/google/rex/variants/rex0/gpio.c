/* SPDX-License-Identifier: GPL-2.0-or-later */

/* This header block is used to supply information to arbitrage, a
 * google-internal tool. Updating it incorrectly will lead to issues,
 * so please don't update it unless a change is specifically required.
 * BaseID: 3EC4CE58201758F4
 * Overrides: c826ba419f06f9df9cded8e60633253ddc7b60ff
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <console/console.h>
#include <boardid.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* GPP_A00 : GPP_A00 ==> ESPI_SOC_IO0_R configured on reset, do not touch */
	/* GPP_A01 : GPP_A01 ==> ESPI_SOC_IO1_R configured on reset, do not touch */
	/* GPP_A02 : GPP_A02 ==> ESPI_SOC_IO2_R configured on reset, do not touch */
	/* GPP_A03 : GPP_A03 ==> ESPI_SOC_IO3_R configured on reset, do not touch */
	/* GPP_A04 : GPP_A04 ==> ESPI_SOC_CS0_L configured on reset, do not touch */
	/* GPP_A05 : GPP_A05 ==> ESPI_SOC_CLK_R configured on reset, do not touch */
	/* GPP_A06 : GPP_A06 ==> ESPI_SOC_RESET_L configured on reset, do not touch */
	/* GPP_A11 : [] ==> EN_UCAM_SENR_PWR */
	PAD_CFG_GPO(GPP_A11, 0, DEEP),
	/* GPP_A12 : [] ==> EN_UCAM_PWR */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* GPP_A13 : [] ==> SD_PE_LS_PRSNT_L */
	PAD_CFG_GPI_LOCK(GPP_A13, NONE, LOCK_CONFIG),
	/* GPP_A14 : [] ==> WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A14, 1, DEEP),
	/* GPP_A15 : [] ==> WWAN_RST_L */
	PAD_CFG_GPO(GPP_A15, 1, DEEP),
	/* GPP_A16 : GPP_A16 ==> ESPI_SOC_ALERT_L configured on reset, do not touch */
	/* GPP_A17 : [] ==> EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_A17, NONE, LEVEL, INVERT, LOCK_CONFIG),

	/* GPP_A18 : [] ==> CAM_PSW_L */
	PAD_CFG_GPI_INT_LOCK(GPP_A18, NONE, EDGE_BOTH, LOCK_CONFIG),
	/* GPP_A19 : [] ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_A19, 1, DEEP),
	/* GPP_A20 : [] ==> SSD_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_A20, 1, LOCK_CONFIG),
	/* GPP_A21 : [] ==> WWAN_CONFIG2 */
	PAD_CFG_GPI(GPP_A21, NONE, DEEP),

	/* GPP_B00 : [] ==> TCHPAD_INT_ODL_LS */
	PAD_CFG_GPI_IRQ_WAKE_LOCK(GPP_B00, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_B01 : [] ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_B01, 1, DEEP),
	/* GPP_B02 : net NC is not present in the given design */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03 : net NC is not present in the given design */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04 : GPP_B04_STRAP ==> Component NC */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05 : [] ==> SPKR_INT_L_R */
	PAD_CFG_GPI(GPP_B05, NONE, DEEP),
	/* GPP_B06 : [] ==> HP_INT_L_R */
	PAD_CFG_GPI_INT(GPP_B06, NONE, PLTRST, EDGE_BOTH),
	/* GPP_B07 : [] ==> RST_HP_L */
	PAD_CFG_GPO(GPP_B07, 1, DEEP),
	/* GPP_B08 : net NC is not present in the given design */
	PAD_NC(GPP_B08, NONE),
	/* GPP_B09 : [] ==> EN_FCAM_PWR */
	PAD_CFG_GPO(GPP_B09, 0, DEEP),
	/* GPP_B10 : [] ==> WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
	/* GPP_B11 : [] ==> EN_FP_PWR */
	PAD_CFG_GPO_LOCK(GPP_B11, 1, LOCK_CONFIG),
	/* GPP_B12 : [] ==> SLP_SO_R_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13 : [] ==> PLT_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14 : GPP_B14_STRAP ==> Component NC */
	PAD_NC(GPP_B14, NONE),
	/* GPP_B15 : [] ==> USB_OC3# */
	PAD_CFG_NF_LOCK(GPP_B15, NONE, NF1, LOCK_CONFIG),
	/* GPP_B16 : [] ==> SOC_HDMI_HPD_L */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF2),
	/* GPP_B17 : [] ==> EN_WWAN_PWR */
	PAD_CFG_GPO(GPP_B17, 1, DEEP),
	/* GPP_B18 : [] ==> SOC_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_B18, NONE, NF2, LOCK_CONFIG),
	/* GPP_B19 : [] ==> SOC_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_B19, NONE, NF2, LOCK_CONFIG),
	/* GPP_B20 : [] ==> SOC_I2C_MISC_SDA */
	PAD_CFG_NF_LOCK(GPP_B20, NONE, NF2, LOCK_CONFIG),
	/* GPP_B21 : [] ==> SOC_I2C_MISC_SCL */
	PAD_CFG_NF_LOCK(GPP_B21, NONE, NF2, LOCK_CONFIG),
	/* GPP_B22 : [] ==> USB4_RT_FORCE_PWR */
	PAD_CFG_GPO(GPP_B22, 0, DEEP),
	/* GPP_B23 : [] ==> WWAN_CONFIG0 */
	PAD_CFG_GPI_LOCK(GPP_B23, NONE, LOCK_CONFIG),

	/* GPP_C00 : [] ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C00, 0, DEEP),
	/* GPP_C01 : [] ==> USI_RST_L */
	PAD_CFG_GPO(GPP_C01, 0, DEEP),
	/* GPP_C02 : SOC_TCHSCR_SPI_INT_STRAP ==> Component NC */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03 : [] ==> EN_WCAM_SENR_PWR */
	PAD_CFG_GPO_LOCK(GPP_C03, 0, LOCK_CONFIG),
	/* GPP_C04 : [] ==> EN_WCAM_PWR */
	PAD_CFG_GPO_LOCK(GPP_C04, 0, LOCK_CONFIG),
	/* GPP_C05 : [] ==> WWAN_PERST_L_STRAP */
	PAD_CFG_GPO(GPP_C05, 1, PLTRST),
	/* GPP_C06 : [] ==> SOC_TCHSCR_RPT_EN */
	/*
	 * FIXME: Remove this code after resolving b/247029304.
	 *
	 * ELAN6918 Power Sequencing seems not perfectly matching
	 * with the previous platforms and setting GPP_C06 to high prior
	 * to the power sequencing is actually makes it work.
	 *
	 * Ideally Power Sequencing should be as below for ELAN6918 (in ACPI)
	 * `POWER enabled -> RESET deasserted -> Report EN enabled`
	 *
	 * But below sequence is only working currently:
	 * `Report EN enabled (ramstage) -> POWER enabled (ACPI) -> RESET deasserted (ACPI)`
	 */
	PAD_CFG_GPO(GPP_C06, 1, DEEP),
	/* GPP_C07 : [] ==> SOC_TCHSCR_INT */
	PAD_CFG_GPI_APIC(GPP_C07, NONE, PLTRST, LEVEL, NONE),
	/* GPP_C08 : [] ==> SOCHOT_ODL */
	PAD_CFG_NF(GPP_C08, NONE, DEEP, NF2),
	/* GPP_C09 : net NC is not present in the given design */
	PAD_NC(GPP_C09, NONE),
	/* GPP_C10 : net NC is not present in the given design */
	PAD_NC(GPP_C10, NONE),
	/* GPP_C11 : [] ==> SD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C11, NONE, DEEP, NF1),
	/* GPP_C12 : [] ==> WWAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
	/* GPP_C13 : [] ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C13, NONE, DEEP, NF1),
	/* GPP_C15 : [] ==> GPP_C15_STRAP */
	PAD_NC(GPP_C15, NONE),
	/* GPP_C16 : [] ==> USB_C0_LSX_TX */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17 : [] ==> USB_C0_LSX_RX */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18 : [] ==> USB_C0_AUX_DC_P */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF6),
	/* GPP_C19 : [] ==> USB_C0_AUX_DC_N */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF6),
	/* GPP_C20 : [] ==> USB_C1_LSX_TX */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* GPP_C21 : [] ==> USB_C1_LSX_RX */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* GPP_C22 : [] ==> SOC_FP_BOOT0 */
	PAD_CFG_GPO_LOCK(GPP_C22, 0, LOCK_CONFIG),
	/* GPP_C23 : [] ==> FP_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_C23, 1, LOCK_CONFIG),

	/* GPP_D00 : WCAM_MCLK_R */
	PAD_CFG_NF(GPP_D00, NONE, DEEP, NF1),
	/* GPP_D01 : [] ==> SD_PE_WAKE_ODL */
	PAD_CFG_GPI_LOCK(GPP_D01, NONE, LOCK_CONFIG),
	/* GPP_D02 : [] ==> SD_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_D02, 1, LOCK_CONFIG),
	/* GPP_D03 : [] ==> EN_PP3300_SD */
	PAD_CFG_GPO_LOCK(GPP_D03, 1, LOCK_CONFIG),
	/* GPP_D04 : [] ==> EN_SPKR */
	PAD_CFG_GPO(GPP_D04, 1, DEEP),
	/* GPP_D05 : net NC.  Test pad. */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06 : net NC.  Test pad.*/
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07 : [] ==> FPMCU_UWB_MUX_SEL */
	PAD_CFG_GPO_LOCK(GPP_D07, 1, LOCK_CONFIG),
	/* GPP_D08 : net NC.  Test pad. */
	PAD_NC(GPP_D08, NONE),
	/* GPP_D09 : [] ==> I2S_MCLK_R */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2),
	/* GPP_D10 : [] ==> I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2),
	/* GPP_D11 : [] ==> I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2),
	/* GPP_D12 : [] ==> I2S_SOC_TX_SPKR_RX_R_STRAP */
	PAD_CFG_NF(GPP_D12, DN_20K, DEEP, NF2),
	/* GPP_D13 : [] ==> I2S_SOC_RX_SPKR_TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2),
	/* GPP_D14 : [] ==> I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF2),
	/* GPP_D15 : [] ==> I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_D15, NONE, DEEP, NF2),
	/* GPP_D16 : [] ==> I2S_SOC_TX_HP_RX_R */
	PAD_CFG_NF(GPP_D16, NONE, DEEP, NF2),
	/* GPP_D17 : [] ==> I2S_SOC_RX_HP_TX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF2),
	/* GPP_D18 : net NC is not present in the given design */
	PAD_NC(GPP_D18, NONE),
	/* GPP_D19 : net NC is not present in the given design */
	PAD_NC(GPP_D19, NONE),
	/* GPP_D20 : net NC is not present in the given design */
	PAD_NC(GPP_D20, NONE),
	/* GPP_D21 : [] ==> WLAN_CLKREQ_ODLl */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF2),
	/* GPP_D22 : net NC is not present in the given design */
	PAD_NC(GPP_D22, NONE),
	/* GPP_D23 : net NC is not present in the given design */
	PAD_NC(GPP_D23, NONE),

	/* GPP_E00 : [] ==> SAR1_INT_L */
	PAD_CFG_GPI_APIC(GPP_E00, NONE, PLTRST, LEVEL, NONE),
	/* GPP_E01 : MEM_STRAP_2 ==> Component NC */
	PAD_CFG_GPI_LOCK(GPP_E01, NONE, LOCK_CONFIG),
	/* GPP_E02 : MEM_STRAP_1 ==> Component NC */
	PAD_CFG_GPI_LOCK(GPP_E02, NONE, LOCK_CONFIG),
	/* GPP_E03 : [] ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E03, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E04 : [] ==> HPS_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E04, NONE, PLTRST, LEVEL, NONE),
	/* GPP_E05 : [] ==> USB_A0_RT_RST_ODL */
	PAD_CFG_GPO(GPP_E05, 1, DEEP),
	/* GPP_E06 : GPP_E06_STRAP ==> Component NC */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07 : [] ==> WWAN_FCPO_L */
	PAD_CFG_GPO(GPP_E07, 1, DEEP),
	/* GPP_E08 : [] ==> SAR2_INT_L */
	PAD_CFG_GPI_APIC_LOCK(GPP_E08, NONE, LEVEL, NONE, LOCK_CONFIG),
	/* GPP_E09 : No heuristic was found useful */
	PAD_CFG_NF_LOCK(GPP_E09, NONE, NF1, LOCK_CONFIG),
	/* GPP_E10 : [] ==> SOC_FPMCU_INT_L */
	PAD_CFG_GPI_IRQ_WAKE_LOCK(GPP_E10, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E11 : [] ==> MEM_STRAP_0 */
	PAD_CFG_GPI_LOCK(GPP_E11, NONE, LOCK_CONFIG),
	/* GPP_E12 : [] ==> MEM_STRAP_3 */
	PAD_CFG_GPI_LOCK(GPP_E12, NONE, LOCK_CONFIG),
	/* GPP_E13 :  [] ==> MEM_CH_SEL */
	PAD_CFG_GPI_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* GPP_E14 : [] ==> SOC_EDP_HPD_L */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* GPP_E15 : net NC is not present in the given design */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16 : net NC.  Test pad. */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17 : [] ==> EN_HPS_PWR */
	PAD_CFG_GPO(GPP_E17, 1, DEEP),
	/* GPP_E22 : [] ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_E22, 1, DEEP),

	/* GPP_F00 :  [] ==> CNV_BRI_DT_R */
	PAD_CFG_NF(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01 :  [] ==> CNV_BRI_RSP */
	PAD_CFG_NF(GPP_F01, UP_20K, DEEP, NF1),
	/* GPP_F02 :  [] ==> CNV_RGI_DT_Rl */
	PAD_CFG_NF(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03 :  [] ==> CNV_RGI_RSP */
	PAD_CFG_NF(GPP_F03, UP_20K, DEEP, NF1),
	/* GPP_F04 :  [] ==> CNV_RF_RST_L */
	PAD_CFG_NF(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05 :  [] ==> CNV_CLKREQ */
	PAD_CFG_NF(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06 :  [] ==> WWAN_WLAN_COEX3 */
	PAD_CFG_NF(GPP_F06, NONE, DEEP, NF1),
	/* GPP_F07 :  [] ==> UCAM_MCLK_R */
	PAD_CFG_GPO(GPP_F07, 0, DEEP),
	/* GPP_F08 : [] ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_F08, 1, DEEP),
	/* GPP_F09 : [] ==> WLAN_PE_WAKE_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F09, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F10 : [] ==> WWAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F10, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F11 : GSP1_SOC_CLK_R */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF5),
	/* GPP_F12 : GSPI1_SOC_DO_FPMCU_DI_R */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF5),
	/* GPP_F13 : GSPI1_SOC_DI_FPMCU_DO_LS */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF5),
	/* GPP_F14 : GSPI_SOC_DO_TCHSCR_DI */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF8),
	/* GPP_F15 : [] ==> GSPI_SOC_DI_TCHSCR_DO */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF8),
	/* GPP_F16 : [] ==> GSPI_SOC_TCHSCR_CLK */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF8),
	/* GPP_F17 : [] ==> GSPI1_SOC_CS_L */
	PAD_CFG_NF(GPP_F17, NONE, DEEP, NF5),
	/* GPP_F18 : [] ==> GSPI_SOC_TCHSCR_CS_L */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF8),
	/* GPP_F19 : [] ==> GPP_F19_STRAP */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20 : [] ==> GPP_F20_STRAP */
	PAD_NC(GPP_F20, NONE),
	/* GPP_F21 : [] ==> GPP_F21_STRAP */
	PAD_NC(GPP_F21, NONE),
	/* GPP_F22 : net NC is not present in the given design */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23 : net NC is not present in the given design */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H00 : GPP_H00_STRAP ==> Component NC */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H01 : GPP_H01_STRAP ==> Component NC */
	PAD_NC(GPP_H01, NONE),
	/* GPP_H02 : GPP_H02_STRAP ==> Component NC */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H04 : [] ==> WWAN_WLAN_COEX1 */
	PAD_CFG_NF(GPP_H04, NONE, DEEP, NF2),
	/* GPP_H05 : [] ==> WWAN_WLAN_COEX2 */
	PAD_CFG_NF(GPP_H05, NONE, DEEP, NF2),
	/* GPP_H06 : [] ==> SOC_I2C_TCHPAD_SDA */
	PAD_CFG_NF_LOCK(GPP_H06, NONE, NF1, LOCK_CONFIG),
	/* GPP_H07 : [] ==> SOC_I2C_TCHPAD_SCL */
	PAD_CFG_NF_LOCK(GPP_H07, NONE, NF1, LOCK_CONFIG),
	/* GPP_H08 : [] ==> UART_DBG_TX_SOC_RX_R */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09 : [] ==> UART_SOC_TX_DBG_RX_R */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10 : [] ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_H10, NONE, LOCK_CONFIG),
	/* GPP_H11 : net NC is not present in the given design */
	PAD_NC(GPP_H11, NONE),
	/* GPP_H13 : [] ==> CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1),
	/* GPP_H14 : [] ==> SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_H14, 1, PLTRST),
	/* GPP_H15 : net NC is not present in the given design */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H16 : [] ==> DDIB_HDMI_CTRLCLK*/
	PAD_CFG_NF(GPP_H16, NONE, DEEP, NF1),
	/* GPP_H17 : [] ==> DDIB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* GPP_H19 : [] ==> SOC_I2C_AUD_WFC_SDA */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* GPP_H20 : [] ==> SOC_I2C_AUD_WFC_SCL */
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1),
	/* GPP_H21 : [] ==>  SOC_I2C_TCHSCR_SDA */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22 : [] ==>  SOC_I2C_TCHSCR_SCL */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),

	/* GPP_S00 :  [] ==> SDW_HP_CLK  */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* GPP_S01 :  [] ==> SDW_HP_DATA */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* GPP_S02 : [] ==> DMIC_SOC_CLK0_DB_RC */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF3),
	/* GPP_S03 : [] ==> DMIC_SOC_DATA0_DB_R */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF3),
	/* GPP_S04 : [] ==> SDW_SPKR_CLK */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF1),
	/* GPP_S05 : [] ==> SDW_SPKR_DATA */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF1),
	/* GPP_S06 : [] ==> DMIC_SOC_CLK1_DB_RC */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF3),
	/* GPP_S07 : [] ==> DMIC_SOC_DATA1_DB */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF3),

	/* GPP_V00 : [] ==> BATLOW_L */
	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1),
	/* GPP_V01 : [] ==> ACPRESENT */
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1),
	/* GPP_V02 : [] ==> EC_SOC_WAKE_ODL */
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1),
	/* GPP_V03 : [] ==> EC_SOC_PWR_BTN_ODL */
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1),
	/* GPP_V04 : [] ==> SLP_S3_L */
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1),
	/* GPP_V05 : [] ==> SLP_S4_L */
	PAD_CFG_NF(GPP_V05, NONE, DEEP, NF1),
	/* GPP_V06 : [] ==> SOC_SLP_A_L */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V08 : [] ==> SOC_SUSCLK */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09 : [] ==> SOC_SLP_WLAN_L */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10 : [] ==> SLP_S5_L */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),
	/* GPP_V11 : [] ==> SOC_GPP_V11 testpoint*/
	PAD_NC(GPP_V11, NONE),
	/* GPP_V12 : [] ==> SOC_SLP_LAN_L */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V14 : [] ==> SOC_WAKE_L */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V22 : [] ==> WCAM_RST_L */
	PAD_CFG_GPO(GPP_V22, 0, DEEP),
	/* GPP_V23 : [] ==> UCAM_RST_L */
	PAD_CFG_GPO(GPP_V23, 0, DEEP),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_B17 : [] ==> EN_WWAN_PWR */
	PAD_CFG_GPO(GPP_B17, 1, DEEP),
	/* GPP_B18 : [] ==> SOC_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF2),
	/* GPP_B19 : [] ==> SOC_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B19, NONE, DEEP, NF2),
	/* GPP_C05 : [] ==> WWAN_PERST_L_STRAP (updated in ramstage) */
	PAD_CFG_GPO(GPP_C05, 0, DEEP),
	/* GPP_A15 : [] ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_A15, 0, DEEP),
	/* GPP_E03 : [] ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E03, NONE, PLTRST, LEVEL, INVERT),

	/* GPP_E07 : [] ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_E07, 0, DEEP),
	/* GPP_H08 : [] ==> UART_DBG_TX_SOC_RX_R */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09 : [] ==> UART_SOC_TX_DBG_RX_R */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),

	/* GPP_D03 : [] ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_D03, 1, DEEP),

	/* GPP_E13 :  [] ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),

	/* GPP_A20 : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_A20, 0, DEEP),

	/* GPP_H10 : [] ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_H10, NONE, LOCK_CONFIG),
};

static const struct pad_config romstage_gpio_table[] = {
	/* GPP_B11 : [] ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_B11, 0, DEEP),
	/* A20 : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_A20, 0, DEEP),
	/* GPP_C23 : [] ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_C23, 0, DEEP),
	/* GPP_E07 : [] ==> WWAN_FCPO_L */
	PAD_CFG_GPO(GPP_E07, 1, DEEP),
	/* GPP_D02 : [] ==> SD_PERST_L */
	PAD_CFG_GPO(GPP_D02, 1, DEEP),
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
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);
