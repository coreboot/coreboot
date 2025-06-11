/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage for Pujjolo */
static const struct pad_config override_gpio_table[] = {
	/* A0 thru A4, A9 and A10 come configured out of reset, do not touch */
	/* A0  : ESPI_IO0 ==> ESPI_SOC_D0_EC */
	/* A1  : ESPI_IO1 ==> ESPI_SOC_D1_EC */
	/* A2  : ESPI_IO2 ==> ESPI_SOC_D2_EC */
	/* A3  : ESPI_IO3 ==> ESPI_SOC_D3_EC */
	/* A4  : ESPI_CS0# ==> ESPI_SOC_CS_EC_L */
	/* A5  : ESPI_ALERT0# ==> NC */
	PAD_NC(GPP_A5, NONE),
	/* A6  : ESPI_ALERT1# ==> NC */
	PAD_NC(GPP_A6, NONE),
	/* A7  : NC ==> SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_A7, 1, DEEP),
	/* A8  : GPP_A8 ==> WWAN_RF_DISABLE_ODL */
	PAD_CFG_GPO(GPP_A8, 1, DEEP),
	/* A9  : ESPI_CLK ==> ESPI_SOC_CLK */
	/* A10 : ESPI_RESET# ==> ESPI_SOC_RST_EC_L */
	/* A11 : EN_SPK_PA ==> NC */
	PAD_NC(GPP_A11, NONE),
	/* A12 : SOC_PEN_DETECT_ODL */
	PAD_CFG_GPI_SCI_HIGH(GPP_A12, NONE, PLTRST, EDGE_BOTH),
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* A14 : USB_OC1# */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* A15 : USB_OC2# */
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1),
	/* A16 : USB_OC3# ==> NC */
	PAD_NC_LOCK(GPP_A16, NONE, LOCK_CONFIG),
	/* A17 : NC */
	PAD_NC(GPP_A17, NONE),
	/* A18 : NC ==> HDMI_HPD_SRC*/
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1),
	/* A19 : NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A21 : GPP_A21 ==> USB_C1_AUX_DC_P */
	PAD_CFG_NF(GPP_A21, NONE, DEEP, NF6),
	/* A22 : GPP_A22 ==> USB_C1_AUX_DC_N */
	PAD_CFG_NF(GPP_A22, NONE, DEEP, NF6),
	/* A23 : GPP_A23 ==> NC */
	PAD_NC(GPP_A23, NONE),
	/* B0  : CORE_VID0 ==> VCCIN_AUX_VID0 */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* B1  : CORE_VID1 ==> VCCIN_AUX_VID1 */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* B2  : NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : CPU_GP2 ==> GYRO_SENSOR_INT */
	PAD_CFG_GPI_APIC(GPP_B3, NONE, PWROK, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> ACC_SENSOR_INT */
	PAD_CFG_GPI_APIC(GPP_B4, NONE, PWROK, LEVEL, INVERT),
	/* B5  : GPP_B5 ==> ISH_I2C0_SENSOR_SCL */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF1),
	/* B6  : GPP_B6 ==> ISH_I2C0_SENSOR_SDA */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),
	/* B7  : GPP_B7 ==> NC */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : WWAN_SAR_DETECT_2_ODL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/* B9  : Not available */
	PAD_NC(GPP_B9, NONE),
	/* B10 : Not available */
	PAD_NC(GPP_B10, NONE),
	/* B11 : SOC_PD0_INT# */
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF1),
	/* B12 : SLP_S0# ==> PM_SLP_S0# */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* B13 : PLTRST# ==> PLT_RST_L */
	PAD_CFG_NF_LOCK(GPP_B13, NONE, NF1, LOCK_CONFIG),
	/* B14 : ACZ_SPKR */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),
	/* B15 : NC */
	PAD_NC_LOCK(GPP_B15, NONE, LOCK_CONFIG),
	/* B16  : GPP_B16 ==> I2C_TOUCHPAD_SDA */
	PAD_CFG_NF_LOCK(GPP_B16, NONE, NF2, LOCK_CONFIG),
	/* B17  : GPP_B17 ==> I2C_TOUCHPAD_SCL */
	PAD_CFG_NF_LOCK(GPP_B17, NONE, NF2, LOCK_CONFIG),
	/* B18 : GPP_B18 ==> GPP_B18_STRAP */
	PAD_NC(GPP_B18, NONE),
	/* B19 : Not available */
	PAD_NC(GPP_B19, NONE),
	/* B20 : Not available */
	PAD_NC(GPP_B20, NONE),
	/* B21 : Not available */
	PAD_NC(GPP_B21, NONE),
	/* B22 : Not available */
	PAD_NC(GPP_B22, NONE),
	/* B23 : SML1ALERT# ==> PCHHOT_ODL_STRAP */
	PAD_NC(GPP_B23, NONE),
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR_X */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> TCHSCR_RST_L */
	PAD_CFG_GPO(GPP_C1, 1, DEEP),
	/* C2  : SMBALERT# ==> GPP_C2_STRAP */
	PAD_NC(GPP_C2, NONE),
	/* C3  : EN_PP3300_UCAM_X */
	PAD_CFG_GPO_LOCK(GPP_C3, 1, LOCK_CONFIG),
	/* C4  : TCHSCR_REPORT_EN */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* C5  : SML0ALERT# ==> GPP_C5_STRAP */
	PAD_NC(GPP_C5, NONE),
	/* C6  : SML1_SMBCLK */
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1),
	/* C7  : SML1_SMBDATA */
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1),
	/* D0  : ISH_GP0 ==> PCH_FP_BOOT0 */
	PAD_CFG_GPO_LOCK(GPP_D0, 0, LOCK_CONFIG),
	/* D1  : ISH_GP1 ==> SOC_GSEN2_INT# */
	PAD_CFG_NF(GPP_D1, NONE, DEEP, NF1),
	/* D2  : NC */
	PAD_NC_LOCK(GPP_D2, NONE, LOCK_CONFIG),
	/* D3  : ISH_GP3 ==> WCAM_RST_L */
	PAD_CFG_GPO_LOCK(GPP_D3, 0, LOCK_CONFIG),
	/* D4  : IMGCLKOUT0 ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_D4, 1, DEEP),
	/* D5  : NC */
	PAD_NC(GPP_D5, NONE),
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 1, DEEP),
	/* D7  : SRCCLKREQ2# ==> WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),
	/* D8  : SRCCLKREQ3# ==> SD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D8, NONE, DEEP, NF1),
	/* D9  : NC */
	PAD_NC_LOCK(GPP_D9, NONE, LOCK_CONFIG),
	/* D10 : ISH_SPI_CLK ==> GPP_D10_STRAP */
	PAD_NC_LOCK(GPP_D10, NONE, LOCK_CONFIG),
	/* D11 : NC */
	PAD_NC_LOCK(GPP_D11, NONE, LOCK_CONFIG),
	/* D12 : ISH_SPI_MOSI ==> GPP_D12_STRAP */
	PAD_NC_LOCK(GPP_D12, NONE, LOCK_CONFIG),
	/* D13 : UART0_ISH_RX_DBG_TX */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF1),
	/* D14 : UART0_ISH_TX_DBG_RX */
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF1),
	/* D15 : ISH_UART0_RTS# ==> EN_PP2800_WCAM_X */
	PAD_CFG_GPO_LOCK(GPP_D15, 1, LOCK_CONFIG),
	/* D16 : ISH_UART0_CTS# ==> EN_PP1800_PP1200_WCAM_X */
	PAD_CFG_GPO_LOCK(GPP_D16, 1, LOCK_CONFIG),
	/* D17  : NC ==> UART_AP_RX_FP_TX */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
	/* D18  : NC ==> UART_AP_TX_FP_RX */
	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
	/* D19 : WWAN_SAR_DETECT_2_ODL ==> NC */
	PAD_NC(GPP_D19, NONE),
	/* E0  : SOC_PEN_DETECT_R_ODL */
	PAD_CFG_GPI_INT(GPP_E0, NONE, PLTRST, EDGE_BOTH),
	/* E1  : THC0_SPI1_IO2 ==> MEM_STRAP_0 */
	PAD_CFG_GPI_LOCK(GPP_E1, NONE, LOCK_CONFIG),
	/* E2  : THC0_SPI1_IO3 ==> MEM_STRAP_1 */
	PAD_CFG_GPI_LOCK(GPP_E2, NONE, LOCK_CONFIG),
	/* E3  : PROC_GP0 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),
	/* E4  : WLAN_WWAN_COEX_1 temp out high*/
	PAD_CFG_GPO(GPP_E4, 0, DEEP),
	/* E5  : WLAN_WWAN_COEX_2 temp out high */
	PAD_CFG_GPO(GPP_E5, 0, DEEP),
	/* E6  : THC0_SPI1_RST# ==> GPP_E6_STRAP */
	PAD_NC_LOCK(GPP_E6, NONE, LOCK_CONFIG),
	/* E7  : NC ==> FP_RST_ODL */
	PAD_CFG_GPO_LOCK(GPP_E7, 1, LOCK_CONFIG),
	/* E8  : GPP_E8 ==> WLAN_DISABLE_L */
	PAD_CFG_GPO(GPP_E8, 1, DEEP),
	/* E9  : NC */
	PAD_NC_LOCK(GPP_E9, NONE, LOCK_CONFIG),
	/* E10 : EN_PP3300_WLAN_X */
	PAD_CFG_GPO(GPP_E10, 0, DEEP),
	/* E11 : TCHSCR_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_E11, NONE, PLTRST, LEVEL, INVERT),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_E12, NONE, LOCK_CONFIG),
	/* E13 : SD_WAKE_N*/
	PAD_CFG_GPI_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* E14 : DDSP_HPDA ==> EDP_HPD */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* E15 : NC */
	PAD_NC(GPP_E15, NONE),
	/* E16 : NC */
	PAD_NC(GPP_E16, NONE),
	/* E17  : WWAN_RST_L */
	PAD_CFG_GPO_LOCK(GPP_E17, 1, LOCK_CONFIG),
	/* E18 : NC */
	PAD_NC(GPP_E18, NONE),
	/* E19 : DDP1_CTRLDATA ==> GPP_E19_STRAP */
	PAD_NC(GPP_E19, NONE),
	/* E20 : DDP2_CTRLCLK ==> HDMI_DDC_SCL */
	PAD_NC(GPP_E20, NONE),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),
	/* E22 : DDPA_CTRLCLK ==> USB_C0_AUX_DC_P */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF6),
	/* E23 : DDPA_CTRLDATA ==> USB_C0_AUX_DC_N */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF6),
	/* F0  : CNV_BRI_DT ==> CNV_BRI_DT_STRAP */
	PAD_CFG_NF(GPP_F0, NONE, DEEP, NF1),
	/* F1  : CNV_BRI_RSP ==> CNV_BRI_RSP */
	PAD_CFG_NF(GPP_F1, UP_20K, DEEP, NF1),
	/* F2  : CNV_RGI_DT ==> CNV_RGI_DT_STRAP */
	PAD_CFG_NF(GPP_F2, NONE, DEEP, NF1),
	/* F3  : CNV_RGI_RSP ==> CNV_RGI_RSP */
	PAD_CFG_NF(GPP_F3, UP_20K, DEEP, NF1),
	/* F4  : CNV_RF_RESET# ==> CNV_RF_RST_L */
	PAD_CFG_NF(GPP_F4, NONE, DEEP, NF1),
	/* F5  : CRF_XTAL_CLKREQ ==> CNV_CLKREQ0 */
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF3),
	/* F6  : CNV_PA_BLANKING ==> WLAN_WWAN_COEX_3 */
	PAD_CFG_NF(GPP_F6, NONE, DEEP, NF1),
	/* F7  : GPP_F7 ==> GPP_F7_STRAP */
	PAD_NC(GPP_F7, NONE),
	/* F8  : Not available */
	PAD_NC(GPP_F8, NONE),
	/* F9  : Not available */
	PAD_NC(GPP_F9, NONE),
	/* F10 : GPP_F10 ==> GPP_F10_STRAP */
	PAD_NC(GPP_F10, NONE),
	/* F11 : NC ==> GSPI_PCH_CLK_FPMCU */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF4),
	/* F12 : WWAN_RST_L ==> GSPI_PCH_DO_FPMCU_DI_R */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF4),
	/* F13 : SOC_PEN_DETECT_R_ODL ==> GSPI_PCH_DI_FPMCU_DO */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF4),
	/* F14 : GSXDIN ==> TCHPAD_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F14, NONE, PWROK, LEVEL, INVERT),
	/* F15 : SOC_PEN_DETECT_ODL ==> FPMCU_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F15, NONE, PWROK, LEVEL, INVERT),
	/* F16  : NC ==> GSPI_PCH_CS_FPMCU_R_L */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF4),
	/* F17 : THC1_SPI2_RST# ==> EC_SOC_WAKE_ODL */
	PAD_CFG_GPI_IRQ_WAKE_LOCK(GPP_F17, NONE, EDGE_SINGLE, INVERT, LOCK_CONFIG),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI_LOCK(GPP_F18, NONE, LOCK_CONFIG),
	/* F19 : Not available */
	PAD_NC(GPP_F19, NONE),
	/* F20 : Not available */
	PAD_NC(GPP_F20, NONE),
	/* F21 : Not available */
	PAD_NC(GPP_F21, NONE),
	/* F22 : NC */
	PAD_NC(GPP_F22, NONE),
	/* F23 : V1P05_CTRL ==> V1P05EXT_CTRL */
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF1),
	/* H0  : GPP_H0_STRAP */
	PAD_NC(GPP_H0, NONE),
	/* H1  : GPP_H1_STRAP */
	PAD_NC(GPP_H1, NONE),
	/* H2  : GPP_H2_STRAP */
	PAD_NC(GPP_H2, NONE),
	/* H3  : SX_EXIT_HOLDOFF# ==> WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_SCI_LOW_LOCK(GPP_H3, NONE, EDGE_SINGLE, LOCK_CONFIG),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF_LOCK(GPP_H4, NONE, NF1, LOCK_CONFIG),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF_LOCK(GPP_H5, NONE, NF1, LOCK_CONFIG),
	/* H6  : I2C1_SDA ==> SOC_I2C_TCHSCR_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> SOC_I2C_TCHSCR_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H8  : MIPI_WCAM_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9  : MIPI_WCAM_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H12 : GPP_H12 ==> NC */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : GPP_H13 ==> NC */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
	/* H14 : Not available */
	PAD_NC(GPP_H14, NONE),
	/* H15 : DDPB_CTRLCLK ==> HDMI_DDC_SCL */
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),
	/* H16 : Not available */
	PAD_NC(GPP_H16, NONE),
	/* H17 : DDPB_CTRLDATA ==> HDMI_DDC_SDA */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),
	/* H18 : PROC_C10_GATE# ==> CPU_C10_GATE# */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* H19 : SAR_INT_L */
	PAD_CFG_GPI_APIC(GPP_H19, NONE, PLTRST, LEVEL, NONE),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO(GPP_H20, 1, DEEP),
	/* H21  : WWAN_PERST_L */
	PAD_NC_LOCK(GPP_H21, NONE, LOCK_CONFIG),
	/* H22 : IMGCLKOUT3 ==> WCAM_MCLK_R */
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1),
	/* H23 : GPP_H23 ==> WWAN_SAR_DETECT_ODL */
	PAD_CFG_GPO(GPP_H23, 1, DEEP),
	/* R0  : HDA_HP_BCLK_R */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1),
	/* R1  : HDA_HP_SYNC_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF1),
	/* R2 : HDA_HP_SDO_R */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF1),
	/* R3 : HDA_HP_SDIN0_R */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF1),
	/* R4 : DMIC_CLK_A_0A ==> DMIC_UCAM_CLK_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* R5 : DMIC_DATA_0A ==> DMIC_UCAM_DATA */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* R6 : DMIC_CLK_A_1A ==> DMIC_WCAM_CLK_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* R7 : DMIC_DATA_1A ==> DMIC_WCAM_DATA */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),
	/* S0  : I2S_SPK_BCLK_R ==> NC */
	PAD_NC(GPP_S0, NONE),
	/* S1  : I2S_SPK_LRCK_R ==> NC */
	PAD_NC(GPP_S1, NONE),
	/* S2 : DMIC_CKL_A0 ==> NC */
	PAD_NC(GPP_S2, NONE),
	/* S3 : DMIC_DATA0 ==> NC */
	PAD_NC(GPP_S3, NONE),
	/* S4  : NC */
	PAD_NC(GPP_S4, NONE),
	/* S5  : NC */
	PAD_NC(GPP_S5, NONE),
	/* S6  : NC */
	PAD_NC(GPP_S6, NONE),
	/* S7  : NC */
	PAD_NC(GPP_S7, NONE),
	/* I5  : NC */
	PAD_NC(GPP_I5, NONE),
	/* I7  : EMMC_CMD ==> NC */
	PAD_NC(GPP_I7, NONE),
	/* I8  : EMMC_DATA0 ==> NC */
	PAD_NC(GPP_I8, NONE),
	/* I9  : EMMC_DATA1 ==> NC */
	PAD_NC(GPP_I9, NONE),
	/* I10 : EMMC_DATA2 ==> NC */
	PAD_NC(GPP_I10, NONE),
	/* I11 : EMMC_DATA3 ==> NC */
	PAD_NC(GPP_I11, NONE),
	/* I12 : EMMC_DATA4 ==> NC */
	PAD_NC(GPP_I12, NONE),
	/* I13 : EMMC_DATA5 ==> NC */
	PAD_NC(GPP_I13, NONE),
	/* I14 : EMMC_DATA6 ==> NC */
	PAD_NC(GPP_I14, NONE),
	/* I15 : EMMC_DATA7 ==> NC */
	PAD_NC(GPP_I15, NONE),
	/* I16 : EMMC_RCLK ==> NC */
	PAD_NC(GPP_I16, NONE),
	/* I17 : EMMC_CLK ==> NC */
	PAD_NC(GPP_I17, NONE),
	/* I18 : EMMC_RESET# ==> NC */
	PAD_NC(GPP_I18, NONE),
	/* GPD0  : BATLOW# ==> SOC_BATLOW_L */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
	/* GPD1  : ACPRESENT ==> SOC_ACPRESENT */
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),
	/* GPD2  : EC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPD2, NONE, PLTRST, LEVEL, INVERT),
	/* GPD3  : PWRBTN# ==> EC_SOC_PWR_BTN_ODL */
	PAD_CFG_NF(GPD3, NONE, DEEP, NF1),
	/* GPD4  : SLP_S3# ==> SLP_S3_L */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
	/* GPD5  : SLP_S4# ==> SLP_S4_L */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
	/* GPD6  : SLP_A# ==> NC */
	PAD_NC(GPD6, NONE),
	/* GPD7  : GPD7_STRAP */
	PAD_NC(GPD7, NONE),
	/* GPD8  : SUSCLK ==> PCH_SUSCLK */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
	/* GPD9  : NC */
	PAD_NC(GPD9, NONE),
	/* GPD10 : SLP_S5# ==> NC */
	PAD_NC(GPD10, NONE),
	/* GPD11 : NC */
	PAD_NC(GPD11, NONE),
	/* Configure the unused virtual CNVi Bluetooth UART pads to NC mode. */
	/* vCNV_BT_UART_TXD */
	PAD_NC(GPP_VGPIO_6, NONE),
	/* vCNV_BT_UART_RXD */
	PAD_NC(GPP_VGPIO_7, NONE),
	/* vCNV_BT_UART_CTS_B */
	PAD_NC(GPP_VGPIO_8, NONE),
	/* vCNV_BT_UART_RTS_B */
	PAD_NC(GPP_VGPIO_9, NONE),
	/* Configure the unused vUART for Bluetooth pads to NC mode. */
	/* vUART0_TXD */
	PAD_NC(GPP_VGPIO_18, NONE),
	/* vUART0_RXD */
	PAD_NC(GPP_VGPIO_19, NONE),
	/* vUART0_CTS_B */
	PAD_NC(GPP_VGPIO_20, NONE),
	/* vUART0_RTS_B */
	PAD_NC(GPP_VGPIO_21, NONE),
	/* Configure the virtual CNVi Bluetooth I2S GPIO Pads.*/
	/* BT_I2S_BCLK */
	PAD_CFG_NF(GPP_VGPIO_30, NONE, DEEP, NF3),
	/* BT_I2S_SYNC */
	PAD_CFG_NF(GPP_VGPIO_31, NONE, DEEP, NF3),
	/* BT_I2S_SDO */
	PAD_CFG_NF(GPP_VGPIO_32, NONE, DEEP, NF3),
	/* BT_I2S_SDI */
	PAD_CFG_NF(GPP_VGPIO_33, NONE, DEEP, NF3),
	/* SSP2_SCLK */
	PAD_CFG_NF(GPP_VGPIO_34, NONE, DEEP, NF1),
	/* SSP2_SFRM */
	PAD_CFG_NF(GPP_VGPIO_35, NONE, DEEP, NF1),
	/* SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO_36, NONE, DEEP, NF1),
	/* SSP_RXD */
	PAD_CFG_NF(GPP_VGPIO_37, NONE, DEEP, NF1),

};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : GPP_A13 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/*
	* WWAN_EN is asserted in ramstage to meet the 500 ms warm reset toff
	* requirement. WWAN_EN must be asserted before WWAN_RST_L is released
	* (with min delay 0 ms), so this works as long as the pin used for
	* WWAN_EN comes before the pin used for WWAN_RST_L.
	*/
	/* D6  : SRCCLKREQ1# ==> WWAN_EN */
	PAD_CFG_GPO(GPP_D6, 0, DEEP),
	/* E12 : THC0_SPI1_IO1 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E12, NONE, DEEP),
	/*
	 * E7 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	 PAD_CFG_GPO(GPP_E7, 0, DEEP),
	 /* D2  : ISH_GP2 ==> EN_FP_PWR */
	 PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* E17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_E17, 0, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
};

/* Pad configuration in romstage for Sundance */
static const struct pad_config romstage_gpio_table[] = {
	/* E17 : WWAN_RST_L */
	PAD_CFG_GPO(GPP_E17, 1, DEEP),
	/* Enable touchscreen, hold in reset */
	/* C0  : SMBCLK ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C1  : SMBDATA ==> USI_RST_L */
	PAD_CFG_TERM_GPO(GPP_C1, 0, UP_20K, DEEP),
	/*
	 * E7 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	 PAD_CFG_GPO(GPP_E7, 0, DEEP),
	 /* D2  : ISH_GP2 ==> EN_FP_PWR */
	 PAD_CFG_GPO(GPP_D2, 0, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
}

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(override_gpio_table);
	return override_gpio_table;
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
