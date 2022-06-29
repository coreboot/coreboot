/* SPDX-License-Identifier: GPL-2.0-or-later */

/* This file is safe to edit, but please do not change this comment block. */
/* BaseID: None */
/* Overrides: None */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* GPD0 : [NF1: BATLOW#] ==> BATLOW_L */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),
	/* GPD1 : [NF1: ACPRESENT] ==> ACPRESENT */
	PAD_CFG_NF(GPD1, NONE, DEEP, NF1),
	/* GPD2 : [NF1: LAN_WAKE#] ==> EC_PCH_WAKE_ODL */
	PAD_CFG_NF(GPD2, NONE, DEEP, NF1),
	/* GPD3 : [NF1: PWRBTN#] ==> EC_PCH_PWR_BTN_ODL */
	PAD_CFG_NF(GPD3, NONE, DEEP, NF1),
	/* GPD4 : [NF1: SLP_S3#] ==> SLP_S3_R_L */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),
	/* GPD5 : [NF1: SLP_S4#] ==> SLP_S4_L */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),
	/* GPD6 : [NF1: SLP_A#] ==> SLP_A_L_CAP_SITE */
	PAD_CFG_NF(GPD6, NONE, DEEP, NF1),
	/* GPD7 : GPD7_STRAP ==> Component NC */
	PAD_NC(GPD7, NONE),
	/* GPD8 : [NF1: SUSCLK] ==> PCH_SUSCLK */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),
	/* GPD9 : [NF1: SLP_WLAN#] ==> SLP_WLAN_L_CAP_SITE */
	PAD_CFG_NF(GPD9, NONE, DEEP, NF1),
	/* GPD10 : [NF1: SLP_S5#] ==> SLP_S5_L */
	PAD_CFG_NF(GPD10, NONE, DEEP, NF1),
	/* GPD11 : No heuristic was found useful */
	PAD_NC(GPD11, NONE),
	/* GPP_A0 : GPP_A0 ==> ESPI_PCH_D0_EC_R configured on reset, do not touch */
	/* GPP_A1 : GPP_A1 ==> ESPI_PCH_D1_EC_R configured on reset, do not touch */
	/* GPP_A2 : GPP_A2 ==> ESPI_PCH_D2_EC_R configured on reset, do not touch */
	/* GPP_A3 : GPP_A3 ==> ESPI_PCH_D3_EC_R configured on reset, do not touch */
	/* GPP_A4 : GPP_A4 ==> ESPI_PCH_CS_EC_R_L configured on reset, do not touch */
	/* GPP_A5 : GPP_A5 ==> ESPI_ALERT0 configured on reset, do not touch */
	/* GPP_A6 : GPP_A6 ==> ESPI_ALERT1 configured on reset, do not touch */
	/* GPP_A7 : No heuristic was found useful */
	PAD_NC(GPP_A7, NONE),
	/* GPP_A8 : No heuristic was found useful */
	PAD_NC(GPP_A8, NONE),
	/* GPP_A9 : GPP_A9 ==> ESPI_PCH_CLK_R configured on reset, do not touch */
	/* GPP_A10 : GPP_A10 ==> ESPI_PCH_RST_EC_L configured on reset, do not touch */
	/* GPP_A11 : [NF6: USB_C_GPP_A11] ==> EN_SPKR_PA */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
	/* GPP_A12 : No heuristic was found useful */
	PAD_NC(GPP_A12, NONE),
	/* GPP_A13 : [NF6: USB_C_GPP_A13] ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_A14 : [NF1: USB_OC1# NF2: DDSP_HPD3 NF4: DISP_MISC3 NF6: USB_C_GPP_A14] ==> USB_C1_OC_ODL */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),
	/* GPP_A15 : net NC is not present in the given design */
	PAD_NC(GPP_A15, NONE),
	/* GPP_A16 : net NC is not present in the given design */
	PAD_NC(GPP_A16, NONE),
	/* GPP_A17 : No heuristic was found useful */
	PAD_NC(GPP_A17, NONE),
	/* GPP_A18 : net NC is not present in the given design */
	PAD_NC(GPP_A18, NONE),
	/* GPP_A19 : No heuristic was found useful */
	PAD_NC(GPP_A19, NONE),
	/* GPP_A20 : net NC is not present in the given design */
	PAD_NC(GPP_A20, NONE),
	/* GPP_A21 : net NC is not present in the given design */
	PAD_NC(GPP_A21, NONE),
	/* GPP_A22 : net NC is not present in the given design */
	PAD_NC(GPP_A22, NONE),
	/* GPP_A23 : [NF1: ESPI_CS1# NF6: USB_C_GPP_A23] ==> AUD_HP_INT_L */
	PAD_CFG_GPI_INT(GPP_A23, NONE, PLTRST, EDGE_BOTH),
	/* GPP_B0 : [NF1: CORE_VID0 NF6: USB_C_GPP_B0] ==> SOC_VID0 */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),
	/* GPP_B1 : [NF1: CORE_VID1 NF6: USB_C_GPP_B1] ==> SOC_VID1 */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),
	/* GPP_B2 : [NF1: VRALERT# NF6: USB_C_GPP_B2] ==> GPP_B2 */
	PAD_CFG_NF(GPP_B2, NONE, DEEP, NF6),
	/* GPP_B3 : No heuristic was found useful */
	PAD_NC(GPP_B3, NONE),
	/* GPP_B4 : [NF1: PROC_GP3 NF4: ISH_GP5B NF6: USB_C_GPP_B4] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, PLTRST),
	/* GPP_B5 : [NF1: ISH_I2C0_SDA NF2: I2C2_SDA NF6: USB_C_GPP_B5] ==> PCH_I2C_MISC_R_SDA */
	PAD_CFG_NF(GPP_B5, NONE, DEEP, NF2),
	/* GPP_B6 : [NF1: ISH_I2C0_SCL NF2: I2C2_SCL NF6: USB_C_GPP_B6] ==> PCH_I2C_MISC_R_SCL */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF2),
	/* GPP_B7 : [NF1: ISH_I2C1_SDA NF2: I2C3_SDA NF6: USB_C_GPP_B7] ==> PCH_I2C_TCHSCR_R_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* GPP_B8 : [NF1: ISH_I2C1_SCL NF2: I2C3_SCL NF6: USB_C_GPP_B8] ==> PCH_I2C_TCHSCR_R_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/* GPP_B11 : [NF1: PMCALERT# NF6: USB_C_GPP_B11] ==> EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_B11, 1, DEEP),
	/* GPP_B12 : [NF1: SLP_S0# NF6: USB_C_GPP_B12] ==> SLP_S0_R_L */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),
	/* GPP_B13 : [NF1: PLTRST# NF6: USB_C_GPP_B13] ==> PLT_RST_L */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),
	/* GPP_B14 : GPP_B14_STRAP ==> Component NC */
	PAD_NC(GPP_B14, NONE),
	/* GPP_B15 : net NC is not present in the given design */
	PAD_NC(GPP_B15, NONE),
	/* GPP_B16 : [NF2: I2C5_SDA NF4: ISH_I2C2_SDA NF6: USB_C_GPP_B16] ==> PCH_I2C_TCHPAD_R_SDA */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF2),
	/* GPP_B17 : [NF2: I2C5_SCL NF4: ISH_I2C2_SCL NF6: USB_C_GPP_B17] ==> PCH_I2C_TCHPAD_R_SCL */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF2),
	/* GPP_B18 : GPP_B18_STRAP ==> Component NC */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B23 : PCHHOT_ODL_STRAP ==> Component NC */
	PAD_NC(GPP_B23, NONE),
	/* GPP_C0 : [NF1: SMBCLK NF6: USB_C_GPP_C0] ==> EN_PP3300_TCHSCR */
	PAD_CFG_GPO(GPP_C0, 0, DEEP),
	/* GPP_C1 : [NF1: SMBDATA NF6: USB_C_GPP_C1] ==> EN_TCHSCR */
	PAD_CFG_GPO(GPP_C1, 0, DEEP),
	/* GPP_C2 : GPP_C2_STRAP ==> Component NC */
	PAD_NC(GPP_C2, NONE),
	/* GPP_C3 : net NC is not present in the given design */
	PAD_NC(GPP_C3, NONE),
	/* GPP_C4 : [NF1: SML0DATA NF6: USB_C_GPP_C4] ==> EN_UCAM_PWR */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* GPP_C5 : [NF1: SML0ALERT# NF6: USB_C_GPP_C5] ==> GPP_C5_BOOT_STRAP0 */
	PAD_CFG_NF(GPP_C5, NONE, DEEP, NF6),
	/* GPP_C6 : No heuristic was found useful */
	PAD_NC(GPP_C6, NONE),
	/* GPP_C7 : [NF1: SML1DATA NF6: USB_C_GPP_C7] ==> TCHSCR_INT */
	PAD_CFG_GPI_APIC(GPP_C7, NONE, PLTRST, LEVEL, NONE),
	/* GPP_D0 : PCH_FP_BOOT0 ==> Component NC */
	PAD_NC(GPP_D0, NONE),
	/* GPP_D1 : [NF1: ISH_GP1 NF2: BK1 NF5: SBK1 NF6: USB_C_GPP_D1] ==> FP_RST_ODL */
	PAD_CFG_GPO(GPP_D1, 1, DEEP),
	/* GPP_D2 : [NF1: ISH_GP2 NF2: BK2 NF5: SBK2 NF6: USB_C_GPP_D2] ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* GPP_D3 : [NF1: ISH_GP3 NF2: BK3 NF5: SBK3 NF6: USB_C_GPP_D3] ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D3, 1, DEEP),
	/* GPP_D4 : [NF1: IMGCLKOUT0 NF2: BK4 NF5: SBK4 NF6: USB_C_GPP_D4] ==> BT_DISABLE_L */
	PAD_CFG_GPO(GPP_D4, 1, DEEP),
	/* GPP_D5 : net NC is not present in the given design */
	PAD_NC(GPP_D5, NONE),
	/* GPP_D6 : [NF1: SRCCLKREQ1# NF6: USB_C_GPP_D6] ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* GPP_D7 : net NC is not present in the given design */
	PAD_NC(GPP_D7, NONE),
	/* GPP_D8 : net NC is not present in the given design */
	PAD_NC(GPP_D8, NONE),
	/* GPP_D9 : [NF1: ISH_SPI_CS# NF2: DDP3_CTRLCLK NF4: TBT_LSX2_TXD NF5: BSSB_LS2_RX NF6: USB_C_GPP_D9 NF7: GSPI2_CS0#] ==> USB_C0_LSX_TX */
	PAD_CFG_NF(GPP_D9, NONE, DEEP, NF4),
	/* GPP_D10 : [NF1: ISH_SPI_CLK NF2: DDP3_CTRLDATA NF4: TBT_LSX2_RXD NF5: BSSB_LS2_TX NF6: USB_C_GPP_D10 NF7: GSPI2_CLK] ==> USB_C0_LSX_RX_STRAP */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF4),
	/* GPP_D11 : net NC is not present in the given design */
	PAD_NC(GPP_D11, NONE),
	/* GPP_D12 : GPP_D12_STRAP ==> Component NC */
	PAD_NC(GPP_D12, NONE),
	/* GPP_D13 : No heuristic was found useful */
	PAD_NC(GPP_D13, NONE),
	/* GPP_D14 : No heuristic was found useful */
	PAD_NC(GPP_D14, NONE),
	/* GPP_D15 : net NC is not present in the given design */
	PAD_NC(GPP_D15, NONE),
	/* GPP_D16 : net NC is not present in the given design */
	PAD_NC(GPP_D16, NONE),
	/* GPP_D17 : net NC is not present in the given design */
	PAD_NC(GPP_D17, NONE),
	/* GPP_D18 : net NC is not present in the given design */
	PAD_NC(GPP_D18, NONE),
	/* GPP_D19 : No heuristic was found useful */
	PAD_NC(GPP_D19, NONE),
	/* GPP_E0 : net NC is not present in the given design */
	PAD_NC(GPP_E0, NONE),
	/* GPP_E1 : MEM_STRAP_2 ==> Component NC */
	PAD_NC(GPP_E1, NONE),
	/* GPP_E2 : MEM_STRAP_1 ==> Component NC */
	PAD_NC(GPP_E2, NONE),
	/* GPP_E3 : MEM_STRAP_0 ==> Component NC */
	PAD_NC(GPP_E3, NONE),
	/* GPP_E4 : [NF1: DEVSLP0 NF6: USB_C_GPP_E4 NF7: SRCCLK_OE9#] ==> USB4_BB_RT_FORCE_PWR */
	PAD_CFG_GPO(GPP_E4, 1, DEEP),
	/* GPP_E5 : net NC is not present in the given design */
	PAD_NC(GPP_E5, NONE),
	/* GPP_E6 : GPP_E6_STRAP ==> Component NC */
	PAD_NC(GPP_E6, NONE),
	/* GPP_E7 : MEM_STRAP_3 ==> Component NC */
	PAD_NC(GPP_E7, NONE),
	/* GPP_E8 : [NF6: USB_C_GPP_E8] ==> WIFI_DISABLE_L */
	PAD_CFG_GPO(GPP_E8, 1, DEEP),
	/* GPP_E9 : [NF1: USB_OC0# NF2: ISH_GP4 NF6: USB_C_GPP_E9] ==> USB_C0_OC_ODL */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),
	/* GPP_E10 : [NF2: THC0_SPI1_CS# NF6: USB_C_GPP_E10 NF7: GSPI0_CS0#] ==> PCH_GSPI0_CS_L_UWB */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF7),
	/* GPP_E11 : [NF2: THC0_SPI1_CLK NF6: USB_C_GPP_E11 NF7: GSPI0_CLK] ==> PCH_GSPI0_CLK_UWB_R */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF7),
	/* GPP_E12 : [NF2: THC0_SPI1_IO1 NF5: I2C0A_SDA NF6: USB_C_GPP_E12 NF7: GSPI0_MISO] ==> PCH_GSPI0_DI_UWB_DO */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF7),
	/* GPP_E13 : [NF2: THC0_SPI1_IO0 NF5: I2C0A_SCL NF6: USB_C_GPP_E13 NF7: GSPI0_MOSI] ==> PCH_GSPI0_DO_UWB_DI_R */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF7),
	/* GPP_E14 : [NF1: DDSP_HPDA NF2: DISP_MISCA NF6: USB_C_GPP_E14] ==> SOC_EDP_HPD */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),
	/* GPP_E15 : No heuristic was found useful */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E16 : net NC is not present in the given design */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17 : net NC is not present in the given design */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18 : net NC is not present in the given design */
	PAD_NC(GPP_E18, NONE),
	/* GPP_E19 : GPP_E19_STRAP ==> Component NC */
	PAD_NC(GPP_E19, NONE),
	/* GPP_E20 : [NF1: DDP2_CTRLCLK NF4: TBT_LSX1_TXD NF5: BSSB_LS1_RX NF6: USB_C_GPP_E20] ==> USB_C1_LSX_TX */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF4),
	/* GPP_E21 : [NF1: DDP2_CTRLDATA NF4: TBT_LSX1_RXD NF5: BSSB_LS1_TX NF6: USB_C_GPP_E21] ==> USB_C1_LSX_RX_STRAP */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF4),
	/* GPP_E22 : DNX_STRAP ==> Component NC */
	PAD_NC(GPP_E22, NONE),
	/* GPP_E23 : net NC is not present in the given design */
	PAD_NC(GPP_E23, NONE),
	/* GPP_F0 : CNV_BRI_DT_STRAP ==> Component NC */
	PAD_NC(GPP_F0, NONE),
	/* GPP_F1 : No heuristic was found useful */
	PAD_NC(GPP_F1, NONE),
	/* GPP_F2 : CNV_RGI_DT_STRAP ==> Component NC */
	PAD_NC(GPP_F2, NONE),
	/* GPP_F3 : No heuristic was found useful */
	PAD_NC(GPP_F3, NONE),
	/* GPP_F4 : No heuristic was found useful */
	PAD_NC(GPP_F4, NONE),
	/* GPP_F5 : [NF2: MODEM_CLKREQ NF3: CRF_XTAL_CLKREQ NF6: USB_C_GPP_F5] ==> CNV_CLKREQ0 */
	PAD_CFG_NF(GPP_F5, NONE, DEEP, NF2),
	/* GPP_F6 : net NC is not present in the given design */
	PAD_NC(GPP_F6, NONE),
	/* GPP_F7 : GPP_F7_STRAP ==> Component NC */
	PAD_NC(GPP_F7, NONE),
	/* GPP_F9 : [] ==> SLP_S0_GATE_R */
	PAD_CFG_GPO(GPP_F9, 1, DEEP),
	/* GPP_F10 : GPP_F10_STRAP ==> Component NC */
	PAD_NC(GPP_F10, NONE),
	/* GPP_F11 : [NF3: THC1_SPI2_CLK NF4: GSPI1_CLK NF6: USB_C_GPP_F11] ==> GSPI_PCH_CLK_FPMCU_R */
	PAD_CFG_NF(GPP_F11, NONE, DEEP, NF4),
	/* GPP_F12 : [NF1: GSXDOUT NF3: THC1_SPI2_IO0 NF4: GSPI1_MOSI NF5: I2C1A_SCL NF6: USB_C_GPP_F12] ==> GSPI_PCH_DO_FPMCU_DI_R */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF4),
	/* GPP_F13 : [NF1: GSXSLOAD NF3: THC1_SPI2_IO1 NF4: GSPI1_MISO NF5: I2C1A_SDA NF6: USB_C_GPP_F13] ==> GSPI_PCH_DI_FPMCU_DO */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF4),
	/* GPP_F14 : [NF1: GSXDIN NF3: THC1_SPI2_IO2 NF6: USB_C_GPP_F14] ==> TCHPAD_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F14, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F15 : [NF1: GSXSRESET# NF3: THC1_SPI2_IO3 NF6: USB_C_GPP_F15] ==> FPMCU_PCH_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F15, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F16 : [NF1: GSXCLK NF3: THC1_SPI2_CS# NF4: GSPI_CS0# NF6: USB_C_GPP_F16] ==> GSPI_PCH_CS_FPMCU_R_L */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF4),
	/* GPP_F17 : [NF3: THC1_SPI2_RST# NF6: USB_C_GPP_F17] ==> EC_PCH_INT_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_F17, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_F18 : No heuristic was found useful */
	PAD_NC(GPP_F18, NONE),
	/* GPP_F19 : [NF1: SRCCLKREQ6# NF6: USB_C_GPP_F19] ==> GPP_F19 */
	PAD_CFG_NF(GPP_F19, NONE, DEEP, NF6),
	/* GPP_F20 : [] ==> UCAM_RST_L */
	PAD_CFG_GPO(GPP_F20, 0, DEEP),
	/* GPP_F21 : No heuristic was found useful */
	PAD_NC(GPP_F21, NONE),
	/* GPP_F22 : No heuristic was found useful */
	PAD_NC(GPP_F22, NONE),
	/* GPP_F23 : No heuristic was found useful */
	PAD_NC(GPP_F23, NONE),
	/* GPP_H0 : [NF6: USB_C_GPP_H0] ==> GPP_H0_BOOT_STRAP1 */
	PAD_CFG_NF(GPP_H0, NONE, DEEP, NF6),
	/* GPP_H1 : [NF6: USB_C_GPP_H1] ==> GPP_H1_BOOT_STRAP2 */
	PAD_CFG_NF(GPP_H1, NONE, DEEP, NF6),
	/* GPP_H2 : [NF6: USB_C_GPP_H2] ==> GPP_H2_BOOT_STRAP3 */
	PAD_CFG_NF(GPP_H2, NONE, DEEP, NF6),
	/* GPP_H3 : net NC is not present in the given design */
	PAD_NC(GPP_H3, NONE),
	/* GPP_H4 : [NF1: I2C0_SDA NF6: USB_C_GPP_H4] ==> PCH_I2C_AUD_R_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* GPP_H5 : [NF1: I2C0_SCL NF6: USB_C_GPP_H5] ==> PCH_I2C_AUD_R_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* GPP_H6 : [NF1: I2C1_SDA NF6: USB_C_GPP_H6] ==> PCH_I2C_TPM_R_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* GPP_H7 : [NF1: I2C1_SCL NF6: USB_C_GPP_H7] ==> PCH_I2C_TPM_R_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* GPP_H8 : net NC is not present in the given design */
	PAD_NC(GPP_H8, NONE),
	/* GPP_H9 : net NC is not present in the given design */
	PAD_NC(GPP_H9, NONE),
	/* GPP_H10 : [NF2: UART0_RXD NF3: M2_SKT2_CFG0 NF6: USB_C_GPP_H10] ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* GPP_H11 : [NF2: UART0_TXD NF3: M2_SKT2_CFG1 NF6: USB_C_GPP_H11] ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* GPP_H12 : net NC is not present in the given design */
	PAD_NC(GPP_H12, NONE),
	/* GPP_H13 : net NC is not present in the given design */
	PAD_NC(GPP_H13, NONE),
	/* GPP_H15 : net NC is not present in the given design */
	PAD_NC(GPP_H15, NONE),
	/* GPP_H17 : net NC is not present in the given design */
	PAD_NC(GPP_H17, NONE),
	/* GPP_H18 : [NF1: PROC_C10_GATE# NF6: USB_C_GPP_H18] ==> CPU_C10_GATE_L */
	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
	/* GPP_H19 : net NC is not present in the given design */
	PAD_NC(GPP_H19, NONE),
	/* GPP_H20 : net NC is not present in the given design */
	PAD_NC(GPP_H20, NONE),
	/* GPP_H21 : [NF1: IMGCLKOUT2 NF6: USB_C_GPP_H21] ==> UCAM_MCLK_R */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1),
	/* GPP_H22 : net NC is not present in the given design */
	PAD_NC(GPP_H22, NONE),
	/* GPP_H23 : net NC is not present in the given design */
	PAD_NC(GPP_H23, NONE),
	/* GPP_R0 : [NF1: HDA_BCLK NF2: I2S0_SCLK NF3: DMIC_CLK_B0 NF4: HDAPROC_BCLK] ==> I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),
	/* GPP_R1 : [NF1: HDA_SYNC NF2: I2S0_SFRM NF3: DMIC_CLK_B1] ==> I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),
	/* GPP_R2 : [NF1: HDA_SDO NF2: I2S0_TXD NF4: HDAPROC_SDO] ==> I2S_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF2),
	/* GPP_R3 : [NF1: HDA_SDI0 NF2: I2S0_RXD NF4: HDAPROC_SDI] ==> I2S_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),
	/* GPP_R4 : [NF1: HDA_RST# NF2: I2S2_SCLK NF3: DMIC_CLK_A0] ==> DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),
	/* GPP_R5 : [NF1: HDA_SDI1 NF2: I2S2_SFRM NF3: DMIC_DATA0] ==> DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),
	/* GPP_R6 : [NF2: I2S2_TXD NF3: DMIC_CLK_A1] ==> DMIC_CLK1_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),
	/* GPP_R7 : [NF2: I2S2_RXD NF3: DMIC_DATA1] ==> DMIC_DATA1_R */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),
	/* GPP_S0 : [NF1: SNDW0_CLK NF4: I2S1_SCLK] ==> I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),
	/* GPP_S1 : [NF1: SNDW0_DATA NF4: I2S1_SFRM] ==> I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),
	/* GPP_S2 : [NF1: SNDW1_CLK NF2: DMIC_CKLA0 NF4: I2S1_TXD] ==> I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),
	/* GPP_S3 : [NF1: SNDW1_DATA NF2: DMIC_DATA0 NF4: I2S1_RXD] ==> I2S_PCH_RX_SPKR_TX */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),
	/* GPP_S4 : net NC is not present in the given design */
	PAD_NC(GPP_S4, NONE),
	/* GPP_S5 : net NC is not present in the given design */
	PAD_NC(GPP_S5, NONE),
	/* GPP_S6 : net NC is not present in the given design */
	PAD_NC(GPP_S6, NONE),
	/* GPP_S7 : net NC is not present in the given design */
	PAD_NC(GPP_S7, NONE),
	/* GPP_T2 : No heuristic was found useful */
	PAD_NC(GPP_T2, NONE),
	/* GPP_T3 : No heuristic was found useful */
	PAD_NC(GPP_T3, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER_LOCK(GPP_E15, NONE, LOCK_CONFIG),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),

	/* TPM is on I2C port 1 */
	/* H6  : [NF1: I2C1_SDA NF6: USB_C_GPP_H6] ==> PCH_I2C_TPM_R_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : [NF1: I2C1_SCL NF6: USB_C_GPP_H7] ==> PCH_I2C_TPM_R_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),

	/* Mem straps */
	/* B3  : PROC_GP2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_B3, NONE, DEEP),
	/* E3  : PROC_GP0 ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),
	/* E2  : THC0_SPI1_IO3 ==> MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_E2, NONE, DEEP),
	/* E1  : THC0_SPI1_IO2 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_E1, NONE, DEEP),
	/* E7  : PROC_GP1 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_E7, NONE, DEEP),
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak variant_gpio_override_table(size_t *num)
{
	*num = 0;
	return NULL;
}

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}
