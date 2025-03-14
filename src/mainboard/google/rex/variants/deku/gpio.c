/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* GPP_A00 : [] ==> ESPI_SOC_D0_R */
	PAD_CFG_NF_IOSSTATE(GPP_A00, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A01 : [] ==> ESPI_SOC_D1_R */
	PAD_CFG_NF_IOSSTATE(GPP_A01, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A02 : [] ==> ESPI_SOC_D2_R */
	PAD_CFG_NF_IOSSTATE(GPP_A02, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A03 : [] ==> ESPI_SOC_D3_R */
	PAD_CFG_NF_IOSSTATE(GPP_A03, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A04 : [] ==> ESPI_SOC_CS_L */
	PAD_CFG_NF_IOSSTATE(GPP_A04, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A05 : [] ==> ESPI_SOC_CLK_R */
	PAD_CFG_NF_IOSSTATE(GPP_A05, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A06 : [] ==> ESPI_SOC_RST_L */
	PAD_CFG_NF_IOSSTATE(GPP_A06, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A11 : [] ==> LAN0_ISOLATE_R_ODL */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
	/* GPP_A12 : [] ==> LAN1_ISOLATE_R_ODL */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* GPP_A13 : net NC is not present in the given design */
	PAD_NC(GPP_A13, NONE),
	/* GPP_A14 : net NC is not present in the given design */
	PAD_NC(GPP_A14, NONE),
	/* GPP_A15 : net NC is not present in the given design */
	PAD_NC(GPP_A15, NONE),
	/* GPP_A16 : [] ==> ESPI_SOC_ALERT_L */
	 PAD_CFG_NF_IOSSTATE(GPP_A16, UP_20K, DEEP, NF1, IGNORE),
	/* GPP_A17 : [] ==> EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_A17, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_A18 : net NC is not present in the given design */
	PAD_NC(GPP_A18, NONE),
	/* GPP_A21 : [] ==> SOC_GPP_A21 */
	PAD_NC(GPP_A21, NONE),

	/* GPP_B00 : net NC is not present in the given design */
	PAD_NC(GPP_B00, NONE),
	/* GPP_B01 : [] ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_B01, 1, DEEP),
	/* GPP_B02 : net NC is not present in the given design */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03 : net NC is not present in the given design */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04 : [GPP_B04_STRAP] ==> Component NC */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05 : net NC is not present in the given design */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B06 : net NC is not present in the given design */
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07 : net NC is not present in the given design */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08 : [] ==> PWM_BUZZER */
	PAD_CFG_GPO(GPP_B08, 0, DEEP),
	/* GPP_B09 : net NC is not present in the given design */
	PAD_NC(GPP_B09, NONE),
	/* GPP_B10 : [] ==> WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
	/* GPP_B11 : net NC is not present in the given design */
	PAD_NC(GPP_B11, NONE),
	/* GPP_B12 : [] ==> SLP_S0_R_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13 : [] ==> PLT_PCIE_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14 : [GPP_B14_STRAP] ==> Component NC */
	PAD_NC(GPP_B14, NONE),
	/* GPP_B15 : [] ==> USB_A_OC_ODL */
	PAD_CFG_NF_LOCK(GPP_B15, NONE, NF1, LOCK_CONFIG),
	/* GPP_B16 : net NC is not present in the given design */
	PAD_NC(GPP_B16, NONE),
	/* GPP_B17 : net NC is not present in the given design */
	PAD_NC(GPP_B17, NONE),
	/* GPP_B18 : [] ==> SOC_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_B18, NONE, NF2, LOCK_CONFIG),
	/* GPP_B19 : [] ==> SOC_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_B19, NONE, NF2, LOCK_CONFIG),
	/* GPP_B20 : net NC is not present in the given design */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B21 : net NC is not present in the given design */
	PAD_NC(GPP_B21, NONE),
	/* GPP_B22 : [] ==> USB_C_FORCE_PWR */
	/* TODO: Set back to 0 when the Hayden Bridge Re-timer issue is fixed (b/386019934) */
	PAD_CFG_GPO(GPP_B22, 1, DEEP),
	/* GPP_B23 : net NC is not present in the given design */
	PAD_NC(GPP_B23, NONE),

	/* GPP_C00 : net NC is not present in the given design */
	PAD_NC(GPP_C00, NONE),
	/* GPP_C01 : net NC is not present in the given design */
	PAD_NC(GPP_C01, NONE),
	/* GPP_C02 : [GPP_C02_STRAP] ==> Component NC */
	PAD_NC(GPP_C02, NONE),
	/* GPP_C03 : net NC is not present in the given design */
	PAD_NC(GPP_C03, NONE),
	/* GPP_C04 : net NC is not present in the given design */
	PAD_NC(GPP_C04, NONE),
	/* GPP_C05 : [GPP_C05_STRAP] ==> Component NC */
	PAD_NC(GPP_C05, NONE),
	/* GPP_C06 : net NC is not present in the given design */
	PAD_NC(GPP_C06, NONE),
	/* GPP_C07 : net NC is not present in the given design */
	PAD_NC(GPP_C07, NONE),
	/* GPP_C08 : [] ==> SOCHOT_ODL */
	PAD_CFG_NF(GPP_C08, NONE, DEEP, NF2),
	/* GPP_C09 : [] ==> MISC_SYNC_OD */
	PAD_CFG_GPI(GPP_C09, NONE, DEEP),
	/* GPP_C10 : net NC is not present in the given design*/
	PAD_NC(GPP_C10, NONE),
	/* GPP_C11 : [] ==> LAN1_PCIE_CLKREQ_ODL */
	PAD_CFG_NF(GPP_C11, NONE, DEEP, NF1),
	/* GPP_C12 : net NC is not present in the given design */
	PAD_NC(GPP_C12, NONE),
	/* GPP_C13 : [] ==> LAN0_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_C13, 1, LOCK_CONFIG),
	/* GPP_C15 : [GPP_C15_STRAP] ==> Component NC */
	PAD_NC(GPP_C15, NONE),
	/* GPP_C16 : [] ==> USB_C0_LSX_TX */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* GPP_C17 : [] ==> USB_C0_LSX_RX */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* GPP_C18 : [] ==> USB_C2_LSX_TX */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* GPP_C19 : [] ==> USB_C2_LSX_RX */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* GPP_C20 : [] ==> USB_C1_LSX_TX */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* GPP_C21 : [] ==> USB_C1_LSX_RX */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* GPP_C22 : [] ==> USB_C3_LSX_TX */
	PAD_CFG_NF(GPP_C22, NONE, DEEP, NF1),
	/* GPP_C23 : [] ==> USB_C3_LSX_RX */
	PAD_CFG_NF(GPP_C23, NONE, DEEP, NF1),

	/* GPP_D00 : net NC is not present in the given design */
	PAD_NC(GPP_D00, NONE),
	/* GPP_D01 : [] ==> LAN1_PCIE_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_D01, NONE, DEEP, EDGE_SINGLE),
	/* GPP_D02 : [] ==> LAN1_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_D02, 1, LOCK_CONFIG),
	/* GPP_D03 : net NC is not present in the given design */
	PAD_NC(GPP_D03, NONE),
	/* GPP_D04 : net NC is not present in the given design */
	PAD_NC(GPP_D04, NONE),
	/* GPP_D05 : [] ==> UART_DBG_TX_ISH_RX */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06 : [] ==> UART_ISH_TX_DBG_RX */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07 : [] ==> SOC_GPP_D07 */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08 : net NC is not present in the given design */
	PAD_NC(GPP_D08, NONE),
	/* GPP_D09 : net NC is not present in the given design */
	PAD_NC(GPP_D09, NONE),
	/* GPP_D10 : net NC is not present in the given design */
	PAD_NC(GPP_D10, NONE),
	/* GPP_D11 : net NC is not present in the given design */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12 : [GPP_D12_STRAP] ==> Component NC */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13 : net NC is not present in the given design */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D14 : net NC is not present in the given design */
	PAD_NC(GPP_D14, NONE),
	/* GPP_D15 : net NC is not present in the given design */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D16 : net NC is not present in the given design */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17 : net NC is not present in the given design */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D18 : net NC is not present in the given design */
	PAD_NC(GPP_D18, NONE),
	/* GPP_D19 : [] ==> SSD_PCIE_CLKREQ_ODL */
	 PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
	/* GPP_D20 : [] ==> LAN0_PCIE_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
	/* GPP_D21 : [] ==> WLAN_PCIE_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF2),
	/* GPP_D22 : [] ==> SOC_DBG_BPKI3C_SDA */
	PAD_NC(GPP_D22, NONE),
	/* GPP_D23 : [] ==> SOC_DBG_BPKI3C_SCL */
	PAD_NC(GPP_D23, NONE),

	/* GPP_E00 : net NC is not present in the given design */
	PAD_NC(GPP_E00, NONE),
	/* GPP_E01 : [] ==> MEM_STRAP_2 */
	PAD_CFG_GPI_LOCK(GPP_E01, NONE, LOCK_CONFIG),
	/* GPP_E02 : [] ==> MEM_STRAP_1 */
	PAD_CFG_GPI_LOCK(GPP_E02, NONE, LOCK_CONFIG),
	/* GPP_E03 : [] ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_E03, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_E04 : [] ==> LAN0_PCIE_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_E04, NONE, DEEP, EDGE_SINGLE),
	/* GPP_E05 : [] ==> WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_E05, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_E06 : GPP_E06_STRAP ==> Component NC */
	PAD_NC(GPP_E06, NONE),
	/* GPP_E07 : net NC is not present in the given design */
	PAD_NC(GPP_E07, NONE),
	/* GPP_E08 : [] ==> USB_C0_AUX_DC_N */
	PAD_CFG_NF(GPP_E08, NONE, DEEP, NF6),
	/* GPP_E09 : [] ==> USB_C_OC_ODL */
	PAD_CFG_NF_LOCK(GPP_E09, NONE, NF1, LOCK_CONFIG),
	/* GPP_E10 : net NC is not present in the given design */
	PAD_NC(GPP_E10, NONE),
	/* GPP_E11 : [] ==> MEM_STRAP_0 */
	PAD_CFG_GPI_LOCK(GPP_E11, NONE, LOCK_CONFIG),
	/* GPP_E12 : [] ==> MEM_STRAP_3 */
	PAD_CFG_GPI_LOCK(GPP_E12, NONE, LOCK_CONFIG),
	/* GPP_E13 : [] ==> MEM_CH_SEL */
	PAD_CFG_GPI_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* GPP_E14 : net NC is not present in the given design */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15 : [] ==> SOC_GPP_E15 */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16 : [] ==> GPP_E16_ISH_GP10 */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17 : net NC is not present in the given design */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E22 : [] ==> USB_C0_AUX_DC_P */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF6),

	/* GPP_F00 : [] ==> CNV_BRI_DT_R */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01 : [] ==> WLAN_CNVI_BRI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, UP_20K, DEEP, NF1),
	/* GPP_F02 : [] ==> CNV_RGI_DT_R */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03 : [] ==> WLAN_CNVI_RGI_RSP */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, UP_20K, DEEP, NF1),
	/* GPP_F04 : [] ==> WLAN_CNVI_RF_RST_L */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05 : [] ==> WLAN_CNVI_CLKREQ0 */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_F06 : [] ==> WLAN_COEX3 */
	PAD_CFG_NF(GPP_F06, NONE, DEEP, NF1),
	/* GPP_F07 : net NC is not present in the given design */
	PAD_NC(GPP_F07, NONE),
	/* GPP_F08 : [] ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_F08, 1, DEEP),
	/* GPP_F09 : net NC is not present in the given design */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F10 : net NC is not present in the given design */
	PAD_NC(GPP_F10, NONE),
	/* GPP_F11 : [] ==> AV_GPIO_P0 */
	PAD_CFG_GPO(GPP_F11, 0, DEEP),
	/* GPP_F12 : [] ==> AV_GPIO_P1 */
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	/* GPP_F13 : [] ==> AV_GPIO_P2 */
	PAD_CFG_GPO(GPP_F13, 0, DEEP),
	/* GPP_F14 : [] ==> AV_GPIO_P3 */
	PAD_CFG_GPO(GPP_F14, 0, DEEP),
	/* GPP_F15 : [] ==> AV_GPIO_P4 */
	PAD_CFG_GPO(GPP_F15, 0, DEEP),
	/* GPP_F16 : [] ==> AV_GPIO_P5 */
	PAD_CFG_GPO(GPP_F16, 0, DEEP),
	/* GPP_F17 : [] ==> AV_GPIO_P6 */
	PAD_CFG_GPO(GPP_F17, 0, DEEP),
	/* GPP_F18 : [] ==> AV_GPIO_P7 */
	PAD_CFG_GPO(GPP_F18, 0, DEEP),
	/* GPP_F19 : [GPP_F19_STRAP] ==> Component NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20 : [GPP_F20_STRAP] ==> Component NC */
	PAD_NC(GPP_F20, NONE),
	/* GPP_F21 : [GPP_F21_STRAP] ==> Component NC */
	PAD_NC(GPP_F21, NONE),
	/* GPP_F22 : [] ==> GPP_F22_ISH_GP8A */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23 : [] ==> GPP_F23_ISH_GP9A */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H00 : GPP_H00_STRAP ==> Component NC */
	PAD_NC(GPP_H00, NONE),
	/* GPP_H01 : GPP_H01_STRAP ==> Component NC */
	PAD_NC(GPP_H01, NONE),
	/* GPP_H02 : GPP_H02_STRAP ==> Component NC */
	PAD_NC(GPP_H02, NONE),
	/* GPP_H04 : [] ==> WLAN_COEX1 */
	PAD_CFG_NF(GPP_H04, NONE, DEEP, NF2),
	/* GPP_H05 : [] ==> WLAN_COEX2 */
	PAD_CFG_NF(GPP_H05, NONE, DEEP, NF2),
	/* GPP_H06 : net NC is not present in the given design */
	PAD_NC(GPP_H06, NONE),
	/* GPP_H07 : net NC is not present in the given design */
	PAD_NC(GPP_H07, NONE),
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
	/* GPP_H16 : net NC is not present in the given design */
	PAD_NC(GPP_H16, NONE),
	/* GPP_H17 : net NC is not present in the given design */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H19 : net NC is not present in the given design */
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20 : net NC is not present in the given design */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21 : net NC is not present in the given design */
	PAD_NC(GPP_H21, NONE),
	/* GPP_H22 : net NC is not present in the given design */
	PAD_NC(GPP_H22, NONE),

	/* GPP_S00 : net NC is not present in the given design */
	PAD_NC(GPP_S00, NONE),
	/* GPP_S01 : net NC is not present in the given design */
	PAD_NC(GPP_S01, NONE),
	/* GPP_S02 : net NC is not present in the given design */
	PAD_NC(GPP_S02, NONE),
	/* GPP_S03 : net NC is not present in the given design */
	PAD_NC(GPP_S03, NONE),
	/* GPP_S04 : net NC is not present in the given design */
	PAD_NC(GPP_S04, NONE),
	/* GPP_S05 : net NC is not present in the given design */
	PAD_NC(GPP_S05, NONE),
	/* GPP_S06 : net NC is not present in the given design */
	PAD_NC(GPP_S06, NONE),
	/* GPP_S07 : net NC is not present in the given design */
	PAD_NC(GPP_S07, NONE),

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
	/* GPP_V06 : [] ==> SLP_A_L */
	PAD_CFG_NF(GPP_V06, NONE, DEEP, NF1),
	/* GPP_V08 : [] ==> SOC_SUSCLK */
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1),
	/* GPP_V09 : [] ==> SLP_WLAN_L */
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1),
	/* GPP_V10 : [] ==> SLP_S5_L */
	PAD_CFG_NF(GPP_V10, NONE, DEEP, NF1),
	/* GPP_V11 : [] ==> EC_SOC_REC_SWITCH_ODL */
	PAD_CFG_GPI_LOCK(GPP_V11, NONE, LOCK_CONFIG),
	/* GPP_V12 : [] ==> SLP_LAN_L */
	PAD_CFG_NF(GPP_V12, NONE, DEEP, NF1),
	/* GPP_V14 : [] ==> SOC_WAKE_L */
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1),
	/* GPP_V22 : net NC is not present in the given design */
	PAD_NC(GPP_V22, NONE),
	/* GPP_V23 : net NC is not present in the given design */
	PAD_NC(GPP_V23, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_A20 : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_A20, 0, DEEP),

	/* GPP_B18 : [] ==> SOC_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF2),
	/* GPP_B19 : [] ==> SOC_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B19, NONE, DEEP, NF2),

	/* GPP_C13 : [] ==> LAN0_PERST_L */
	PAD_CFG_GPO(GPP_C13, 0, DEEP),

	/* GPP_D02 : [] ==> LAN1_PERST_L */
	PAD_CFG_GPO(GPP_D02, 0, DEEP),

	/* GPP_E03 : [] ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E03, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_E13 :  [] ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),

	/* GPP_H08 : [] ==> UART_DBG_TX_SOC_RX_R */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),
	/* GPP_H09 : [] ==> UART_SOC_TX_DBG_RX_R */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),
	/* GPP_H10 : [] ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_H10, NONE, LOCK_CONFIG),

	/* GPP_A19 : [] ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_A19, 1, DEEP),
};

static const struct pad_config romstage_gpio_table[] = {
	/* GPP_C13 : [] ==> LAN0_PERST_L */
	PAD_CFG_GPO(GPP_C13, 0, DEEP),

	/* GPP_D02 : [] ==> LAN1_PERST_L */
	PAD_CFG_GPO(GPP_D02, 0, DEEP),

	/* GPP_A20 : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_A20, 1, DEEP),
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
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_CROS_GPIOS(cros_gpios);
