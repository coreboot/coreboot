/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <types.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* A0 thru A5, A9 and A10 come configured out of reset, do not touch */
	/* GPP_A0  : [] ==> ESPI_PCH_D0_EC */
	/* GPP_A1  : [] ==> ESPI_PCH_D1_EC */
	/* GPP_A2  : [] ==> ESPI_PCH_D2_EC */
	/* GPP_A3  : [] ==> ESPI_PCH_D3_EC */
	/* GPP_A4  : [] ==> ESPI_PCH_CS_EC_L */
	/* GPP_A5  : [] ==> TP101 */
	PAD_NC(GPP_A5, NONE),
	/* GPP_A6  : [] ==> TP102 */
	PAD_NC(GPP_A6, NONE),
	/* GPP_A7  : [] ==> PCIE_LAN_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW(GPP_A7, NONE, DEEP, EDGE_SINGLE),
	/* GPP_A8 : [] ==> USB_A1_RT_RST_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* GPP_A9  : [] ==> ESPI_PCH_CLK */
	/* GPP_A10 : [] ==> ESPI_PCH_RST_EC_L */
	/* GPP_A11 : [] ==> EN_SPKR_PA */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
	/* GPP_A12 : [] ==> EN_PP3300_LAN_X */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* GPP_A13 : [] ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_A13, 1, DEEP),
	/* GPP_A14 : [] ==> EC_USB_PCH_C0_OC_ODL */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* GPP_A15 : [] ==> EC_USB_PCH_C1_OC_ODL */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* GPP_A16 : [] ==> USB_A0_OC_ODL */
	PAD_CFG_NF_LOCK(GPP_A16, NONE, NF1, LOCK_CONFIG),
	/* GPP_A17 : [] ==> EN_GPU_PPVAR_GPU_NVVDD_X_PCH */
	PAD_CFG_GPI(GPP_A17, NONE, PLTRST),
	/* GPP_A18 : [] ==> HDMI_HPD */
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* GPP_A19 : [] ==> EN_PCH_PPVAR_GPU_FBVDDQ_X */
	PAD_CFG_GPO(GPP_A19, 0, PLTRST),
	/* GPP_A20 : [] ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_A20, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_A21 : [] ==> NC */
	PAD_NC(GPP_A21, NONE),
	/* GPP_A22 : [] ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* GPP_A23 : [] ==> HP_INT_L */
	PAD_CFG_GPI_INT(GPP_A23, NONE, PLTRST, EDGE_BOTH),

	/* GPP_B0  : [] ==> SOC_VID0 */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* GPP_B1  : [] ==> SOC_VID1 */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* GPP_B2  : [] ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* GPP_B3  : [] ==> GPU_PERST_L */
	PAD_CFG_GPO(GPP_B3, 0, PLTRST),
	/* GPP_B4  : [] ==> SSD_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_B4, 1, LOCK_CONFIG),
	/* GPP_B5  : [] ==> PCH_I2C_NVVDD_GPU_R_SDA */
	PAD_CFG_NF_LOCK(GPP_B5, NONE, NF2, LOCK_CONFIG),
	/* GPP_B6  : [] ==> PCH_I2C_NVVDD_GPU_R_SCL */
	PAD_CFG_NF_LOCK(GPP_B6, NONE, NF2, LOCK_CONFIG),
	/* GPP_B7  : [] ==> PCH_I2C_TPM_R_SDA */
	PAD_CFG_NF_LOCK(GPP_B7, NONE, NF2, LOCK_CONFIG),
	/* GPP_B8  : [] ==> PCH_I2C_TPM_R_SCL */
	PAD_CFG_NF_LOCK(GPP_B8, NONE, NF2, LOCK_CONFIG),
	/* GPP_B9  : [] ==> NC */
	PAD_NC(GPP_B9, NONE),
	/* GPP_B10 : [] ==> NC */
	PAD_NC(GPP_B10, NONE),
	/* GPP_B11 : [] ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_B11, 1, DEEP),
	/* GPP_B12 : [] ==> SLP_S0_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13 : [] ==> PLT_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14 : [] ==> GPP_B14_STRAP */
	PAD_NC_LOCK(GPP_B14, NONE, LOCK_CONFIG),
	/* GPP_B15  : [] ==> NC */
	PAD_NC(GPP_B15, NONE),
	/* GPP_B16 : [] ==> PCH_I2C_TCHPAD_SDA */
	PAD_CFG_NF_LOCK(GPP_B16, NONE, NF2, LOCK_CONFIG),
	/* GPP_B17 : [] ==> PCH_I2C_TCHPAD_SCL */
	PAD_CFG_NF_LOCK(GPP_B17, NONE, NF2, LOCK_CONFIG),
	/* GPP_B18 : [] ==> GPP_B18_STRAP */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19 : [] ==> NC */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B20 : [] ==> NC */
	PAD_NC(GPP_B20, NONE),
	/* GPP_B21 : [] ==> NC */
	PAD_NC(GPP_B21, NONE),
	/* GPP_B22 : [] ==> NC */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B23 : [] ==> PCHHOT_ODL_STRAP */
	PAD_NC(GPP_B23, NONE),

	/* GPP_C0  : [] ==> PCH_SMB_CLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),
	/* GPP_C1  : [] ==> PCH_SMB_DATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),
	/* GPP_C2  : [] ==> GPP_C2_STRAP */
	PAD_NC(GPP_C2, NONE),
	/* GPP_C3 : [] ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* GPP_C4 : [] ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* GPP_C5 : [] ==> GPP_C5_BOOT_STRAP0 */
	PAD_NC(GPP_C5, NONE),
	/* GPP_C6  : [] ==> NC */
	PAD_NC(GPP_C6, NONE),
	/* GPP_C7  : [] ==> NC */
	PAD_NC(GPP_C7, NONE),

	/* GPP_D0 : [] ==> EN_PP1200_GPU_X */
	PAD_CFG_GPO(GPP_D0, 0, PLTRST),
	/* GPP_D1 : [] ==> PG_PP1200_GPU_X_OD */
	PAD_CFG_GPI(GPP_D1, NONE, DEEP),
	/* GPP_D2  : [] ==> LAN_PE_ISOLATE_ODL */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* GPP_D3 : [] ==> PS_NVVDD_TALERT_ODL */
	PAD_CFG_GPI(GPP_D3, NONE, PLTRST),
	/* GPP_D4  : [] ==> NC */
	PAD_NC(GPP_D4, NONE),
	/* GPP_D5  : [] ==> GPU_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D5, NONE, PLTRST, NF1),
	/* GPP_D6  : [] ==> PCIE_SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* GPP_D7  : [] ==> PCIE_WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),
	/* GPP_D8  : [] ==> PCIE_SD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),
	/* GPP_D9  : [] ==> GPU_THERM_INT_ODL */
	PAD_CFG_GPI(GPP_D9, NONE, PLTRST),
	/* GPP_D10 : [] ==> GPP_D10_STRAP */
	PAD_NC_LOCK(GPP_D10, NONE, LOCK_CONFIG),
	/* GPP_D11 : [] ==> EN_PP3300_SSD */
	PAD_CFG_GPO_LOCK(GPP_D11, 1, LOCK_CONFIG),
	/* GPP_D12 : [] ==> GPP_D12_STRAP */
	PAD_NC_LOCK(GPP_D12, NONE, LOCK_CONFIG),
	/* GPP_D13 : [] ==> NC */
	PAD_NC_LOCK(GPP_D13, NONE, LOCK_CONFIG),
	/* GPP_D14 : [] ==> NC */
	PAD_CFG_GPI_LOCK(GPP_D14, NONE, LOCK_CONFIG),
	/* GPP_D15 : [] ==> NC */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* GPP_D16 : [] ==> NC */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* GPP_D17 : [] ==> SD_PE_PRSNT_L */
	PAD_CFG_GPI_LOCK(GPP_D17, NONE, LOCK_CONFIG),
	/* GPP_D18 : [] ==> SD_PE_RST_L */
	PAD_CFG_GPO_LOCK(GPP_D18, 1, LOCK_CONFIG),
	/* GPP_D19 : [] ==> I2S_MCLK_R */
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),

	/* GPP_E0 : [] ==> EN_PPVAR_GPU_NVVDD_X */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* GPP_E1  : [] ==> EN_PP3300_GPU_X */
	PAD_CFG_GPO(GPP_E1, 0, PLTRST),
	/* GPP_E2  : [] ==> PG_PP3300_GPU_X_OD */
	PAD_CFG_GPI_LOCK(GPP_E2, NONE, LOCK_CONFIG),
	/* GPP_E3  : [] ==> WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_E3, 1, DEEP),
	/* GPP_E4  : [] ==> PG_PPVAR_GPU_FBVDDQ_X_OD */
	PAD_CFG_GPI(GPP_E4, NONE, DEEP),
	/* GPP_E5  : [] ==> PG_GPU_ALLRAILS */
	PAD_CFG_GPO(GPP_E5, 0, PLTRST),
	/* GPP_E6  : [] ==> GPPE6_STRAP */
	PAD_NC_LOCK(GPP_E6, NONE, LOCK_CONFIG),
	/* GPP_E7  : [] ==> NC */
	PAD_NC(GPP_E7, NONE),
	/* GPP_E8  : [] ==> PG_PPVAR_GPU_NVVDD_X_OD */
	PAD_CFG_GPI(GPP_E8, NONE, DEEP),
	/* GPP_E9  : [] ==> USB_A1_OC_ODL */
	PAD_CFG_NF_LOCK(GPP_E9, NONE, NF1, LOCK_CONFIG),
	/* GPP_E10 : [] ==> EN_PPVAR_PEXVDD_GPU_X */
	PAD_CFG_GPO(GPP_E10, 0, PLTRST),
	/* GPP_E11 : [] ==> EN_PP1800_GPU_X */
	PAD_CFG_GPO(GPP_E11, 0, PLTRST),
	/* GPP_E12 : [] ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/* GPP_E13 : [] ==> NC */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E14 : [] ==> SOC_EDP_HPD */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* GPP_E15 : [] ==> NC */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16 : [] ==> NC */
	PAD_CFG_GPI(GPP_E16, NONE, DEEP),
	/* GPP_E17 : [] ==> PG_PP0950_GPU_X_OD */
	PAD_CFG_GPI(GPP_E17, NONE, DEEP),
	/* GPP_E18 : [] ==> EN_PP1800_GPU_X */
	PAD_CFG_GPO_LOCK(GPP_E18, 0, LOCK_CONFIG),
	/* GPP_E19 : [] ==> GPP_E19_STRAP */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20 : [] ==> PG_PP1800_GPU_X_OD */
	PAD_CFG_GPI(GPP_E20, NONE, DEEP),
	/* GPP_E21 : [] ==> GPP_E21_STRAP */
	PAD_NC(GPP_E21, NONE),
	/* GPP_E22 : [] ==> GPP_E22_DNX_FRCE_RLD_STRAP */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF2),
	/* GPP_E23 : [] ==> GPP_E23_TP */
	PAD_NC(GPP_E23, NONE),

	/* GPP_F0  : [] ==> CNV_BRI_DT_STRAP */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),
	/* GPP_F1  : [] ==> CNV_BRI_RSP */
	PAD_CFG_NF(GPP_F1, UP_20K, DEEP, NF1),
	/* GPP_F2  : [] ==> CNV_RGI_DT_STRAP */
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF1),
	/* GPP_F3  : [] ==> CNV_RGI_RSP */
	PAD_CFG_NF(GPP_F3, UP_20K, DEEP, NF1),
	/* GPP_F4  : [] ==> CNV_RF_RST_L */
	PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),
	/* GPP_F5  : [] ==> CNV_CLKREQ0 */
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF3),
	/* GPP_F6  : [] ==> NC */
	PAD_NC(GPP_F6, NONE),
	/* GPP_F7  : [] ==> GPPF7_STRAP */
	PAD_NC(GPP_F7, NONE),
	/* GPP_F8  : [] ==> NC */
	PAD_NC(GPP_F8, NONE),
	/* GPP_F9  : [] ==> SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_F9, 1, PLTRST),
	/* GPP_F10 : [] ==> GPPF10_STRAP */
	PAD_NC(GPP_F10, DN_20K),
	/* GPP_F11 : [] ==> NC */
	PAD_NC_LOCK(GPP_F11, NONE, LOCK_CONFIG),
	/* GPP_F12 : [] ==> NC */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13 : [] ==> NC */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* GPP_F14 : [] ==> TCHPAD_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F14, NONE, DEEP, LEVEL, INVERT),
	/* GPP_F15 : [] ==> NC */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
	/* GPP_F16 : [] ==> NC */
	PAD_NC_LOCK(GPP_F16, NONE, LOCK_CONFIG),
	/* GPP_F17 : [] ==> EC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC_LOCK(GPP_F17, NONE, LEVEL, INVERT, LOCK_CONFIG),
	/* GPP_F18 : [] ==> EC_IN_RW_OD */
	PAD_CFG_GPI_LOCK(GPP_F18, NONE, LOCK_CONFIG),
	/* GPP_F19 : [] ==> NC */
	PAD_NC(GPP_F19, NONE),
	/* GPP_F20 : [] ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* GPP_F21 : [] ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* GPP_F22 : [] ==> NC */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23 : [] ==> NC */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H0  : [] ==> GPPH0_BOOT_STRAP1 */
	PAD_NC(GPP_H0, NONE),
	/* GPP_H1  : [] ==> GPPH1_BOOT_STRAP2 */
	PAD_NC(GPP_H1, NONE),
	/* GPP_H2  : [] ==> GPPH2_BOOT_STRAP3 */
	PAD_NC(GPP_H2, NONE),
	/* GPP_H3  : [] ==> WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_LOCK(GPP_H3, NONE, LOCK_CONFIG),
	/* GPP_H4  : [] ==> PCH_I2C_AUD_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* GPP_H5  : [] ==> PCH_I2C_AUD_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* GPP_H6  : [] ==> PCH_I2C_GPU_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* GPP_H7  : [] ==> PCH_I2C_GPU_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* GPP_H8  : [] ==> NC */
	PAD_NC(GPP_H8, NONE),
	/* GPP_H9  : [] ==> NC */
	PAD_NC(GPP_H9, NONE),
	/* GPP_H10 : [] ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* GPP_H11 : [] ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* GPP_H12 : [] ==> SD_PE_WAKE_ODL */
	PAD_CFG_GPI_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* GPP_H13 : [] ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),
	/* GPP_H14 : [] ==> NC */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15 : [] ==> DDIB_HDMI_CTRLCLK */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* GPP_H16 : [] ==> NC */
	PAD_NC(GPP_H16, NONE),
	/* GPP_H17 : [] ==> DDIB_HDMI_CTRLDATA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* GPP_H18 : [] ==> CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* GPP_H19 : [] ==> LAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),
	/* GPP_H20 : [] ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),
	/* GPP_H21 : [] ==> NC */
	PAD_NC(GPP_H21, NONE),
	/* GPP_H22 : [] ==> NC */
	PAD_NC(GPP_H22, NONE),
	/* GPP_H23 : [] ==> WWAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_H23, NONE, DEEP, NF2),

	/* GPP_R0 : [] ==> I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),
	/* GPP_R1 : [] ==> I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),
	/* GPP_R2 : [] ==> I2S_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),
	/* GPP_R3 : [] ==> I2S_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),
	/* GPP_R4 : [] ==> DMIC_CLK0 */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* GPP_R5 : [] ==> DMIC_DATA0 */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* GPP_R6 : [] ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* GPP_R7 : [] ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* GPP_S0 : [] ==> I2S_SPKR_SCLK */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* GPP_S1 : [] ==> I2S_SPKR_SFRM */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* GPP_S2 : [] ==> I2S_PCH_SPKR_RX */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* GPP_S3 : [] ==> NC */
	PAD_NC(GPP_S3, NONE),
	/* GPP_S4 : [] ==> NC */
	PAD_NC(GPP_S4, NONE),
	/* GPP_S5 : [] ==> NC */
	PAD_NC(GPP_S5, NONE),
	/* GPP_S6 : [] ==> NC */
	PAD_NC(GPP_S6, NONE),
	/* GPP_S7 : [] ==> NC */
	PAD_NC(GPP_S7, NONE),

	/* GPD0: [] ==> BATLOW_L */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
	/* GPD1: [] ==> PCH_ACPRESENT */
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),
	/* GPD2 : [] ==> EC_PCH_WAKE_ODL */
	PAD_CFG_NF(GPD2, NONE, DEEP, NF1),
	/* GPD3: [] ==> EC_PCH_PWR_BTN_ODL */
	PAD_CFG_NF(GPD3, NONE, DEEP, NF1),
	/* GPD4: [] ==> SLP_S3_L */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
	/* GPD5: [] ==> SLP_S4_L */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
	/* GPD6: [] ==> SLP_A_L */
	PAD_CFG_NF(GPD6, NONE, DEEP, NF1),
	/* GPD7: [] ==> GPD7_STRAP */
	PAD_NC(GPD7, NONE),
	/* GPD8: [] ==> PCH_SUSCLK */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
	/* GPD9: [] ==> SLP_WLAN_L */
	PAD_CFG_NF(GPD9, NONE, DEEP, NF1),
	/* GPD10: [] ==> SLP_S5_L */
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1),
	/* GPD11: [] ==> WWAN_CONFIG1 */
	PAD_NC(GPD11, NONE),

	/* Virtual GPIO */
	/* Put unused Cnvi BT UART lines in NC mode since we use USB mode. */
	PAD_NC(GPP_VGPIO_6, NONE),
	PAD_NC(GPP_VGPIO_7, NONE),
	PAD_NC(GPP_VGPIO_8, NONE),
	PAD_NC(GPP_VGPIO_9, NONE),

	/* Put unused Cnvi UART0 lines in NC mode since we use USB mode. */
	PAD_NC(GPP_VGPIO_18, NONE),
	PAD_NC(GPP_VGPIO_19, NONE),
	PAD_NC(GPP_VGPIO_20, NONE),
	PAD_NC(GPP_VGPIO_21, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* GPP_A12 : [] ==> EN_PPVAR_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* GPP_A13 : [] ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A20, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_B4  : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* GPP_B7  : [] ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* GPP_B8  : [] ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/*
	 * GPP_D1  : [] ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* GPP_D2  : [] ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* GPP_D11 : [] ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, PLTRST),
	/* GPP_E13 : [] ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* GPP_E15 : [] ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* GPP_E16 : [] ==> WWAN_RST_L
	 * To meet timing constrains - drive reset low.
	 * Deasserted in ramstage.
	 */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* GPP_E15 : [] ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_E15, NONE, LOCK_CONFIG),
	/* GPP_E18 : [] ==> EN_PP1800_GPU_X */
	PAD_CFG_GPO(GPP_E18, 0, PLTRST),
	/* GPP_F18 : [] ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* GPP_H10 : [] ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* GPP_H11 : [] ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/*
	 * enable EN_PP3300_SSD in bootblock, then PERST# is asserted, and
	 * then deassert PERST# in romstage
	 */
	/* GPP_H13 : [] ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, PLTRST),
	/* GPP_B4  : [] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, PLTRST, NF1),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = 0;
	return NULL;
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
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};

DECLARE_WEAK_CROS_GPIOS(cros_gpios);
