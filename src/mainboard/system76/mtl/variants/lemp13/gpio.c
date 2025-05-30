/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/gpio.h>

static const struct pad_config gpio_table[] = {
	PAD_CFG_NF(GPP_A00, UP_20K, DEEP, NF1), // ESPI_IO0_EC
	PAD_CFG_NF(GPP_A01, UP_20K, DEEP, NF1), // ESPI_IO1_EC
	PAD_CFG_NF(GPP_A02, UP_20K, DEEP, NF1), // ESPI_IO2_EC
	PAD_CFG_NF(GPP_A03, UP_20K, DEEP, NF1), // ESPI_IO3_EC
	PAD_CFG_NF(GPP_A04, UP_20K, DEEP, NF1), // ESPI_CS_EC#
	PAD_CFG_NF(GPP_A05, UP_20K, DEEP, NF1), // ESPI_CLK_EC
	PAD_CFG_NF(GPP_A06, NONE, DEEP, NF1), // ESPI_RESET#
	PAD_NC(GPP_A07, NONE),
	PAD_NC(GPP_A08, NONE),
	PAD_NC(GPP_A09, NONE),
	PAD_NC(GPP_A10, NONE),
	PAD_NC(GPP_A11, NONE),
	PAD_NC(GPP_A12, NONE),
	PAD_CFG_TERM_GPO(GPP_A13, 1, UP_20K, PLTRST), // M2_SSD2_RST#
	PAD_NC(GPP_A14, NONE),
	PAD_NC(GPP_A15, NONE),
	PAD_CFG_NF(GPP_A16, UP_20K, DEEP, NF1), // ESPI_ALRT0#
	PAD_CFG_GPI_INT(GPP_A17, NONE, PLTRST, LEVEL), // TP_ATTN#
	PAD_NC(GPP_A18, NONE),
	PAD_NC(GPP_A19, NONE),
	PAD_NC(GPP_A20, NONE),
	PAD_NC(GPP_A21, NONE),

	PAD_NC(GPP_B00, NONE),
	PAD_NC(GPP_B01, NONE),
	PAD_NC(GPP_B02, NONE),
	PAD_NC(GPP_B03, NONE),
	PAD_NC(GPP_B04, NONE),
	PAD_NC(GPP_B05, NONE),
	PAD_CFG_GPO(GPP_B06, 0, DEEP), // ROM_I2C_EN
	PAD_NC(GPP_B07, NONE),
	PAD_NC(GPP_B08, NONE),
	PAD_NC(GPP_B09, NONE), // 10k pull to 1.8V
	PAD_NC(GPP_B10, NONE), // 10k pull to 1.8V
	PAD_CFG_NF(GPP_B11, NONE, DEEP, NF2), // CPU_HDMI_HPD
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1), // SLP_S0#
	PAD_CFG_NF(GPP_B13, NONE, PLTRST, NF1), // PLT_RST#
	PAD_NC(GPP_B14, NONE), // 10k pull to 1.8V
	PAD_CFG_GPI(GPP_B15, NONE, DEEP), // USB_OC3#
	PAD_NC(GPP_B16, NONE), // 10k P/U to 1.8V
	PAD_CFG_GPO(GPP_B17, 1, PLTRST), // HDMI_EN
	PAD_CFG_GPO(GPP_B18, 1, PLTRST), // PCH_BT_EN
	PAD_CFG_GPO(GPP_B19, 1, PLTRST), // WIFI_RF_EN
	PAD_CFG_GPO(GPP_B20, 1, PLTRST), // 5G_PLT_RST_N
	PAD_CFG_GPO(GPP_B21, 0, PLTRST), // TBT_FORCE_PWR
	PAD_CFG_GPI(GPP_B22, NONE, DEEP), // MODEL_ID4
	PAD_CFG_GPO(GPP_B23, 1, DEEP), // NC or PCH_SPKR?

	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1), // SMB_CLK_DDR
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1), // SMB_DATA_DDR
	PAD_CFG_NF(GPP_C02, NONE, DEEP, NF1), // CSME TLS confidentiality strap
	PAD_CFG_NF(GPP_C03, UP_20K, DEEP, NF1), // SML0_CLK
	PAD_CFG_NF(GPP_C04, UP_20K, DEEP, NF1), // SML0_DATA
	PAD_CFG_NF(GPP_C05, UP_20K, DEEP, NF1), // eSPI disable strap
	PAD_CFG_NF(GPP_C06, UP_20K, DEEP, NF1), // TBT_I2C_SCL
	PAD_CFG_NF(GPP_C07, UP_20K, DEEP, NF1), // TBT_I2C_SDA
	PAD_NC(GPP_C08, NONE),
	PAD_CFG_NF(GPP_C09, NONE, DEEP, NF1), // CARD_CLKREQ
	PAD_CFG_GPO(GPP_C10, 0, PLTRST), // 5G_PCIE_CLKREQ
	PAD_CFG_NF(GPP_C11, NONE, PWROK, NF1), // WLAN_CLKREQ
	PAD_NC(GPP_C12, NONE),
	PAD_NC(GPP_C13, NONE),
	// GPP_C14 missing
	PAD_CFG_GPI(GPP_C15, NONE, DEEP), // strap
	// GPP_C16 (TBTA_LSX0_TXD) configured by FSP
	// GPP_C17 (TBTA_LSX0_RXD) configured by FSP
	PAD_NC(GPP_C18, NONE),
	PAD_NC(GPP_C19, NONE),
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF2), // HDMI_CTRLCLK
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF2), // HDMI_CTRLDATA
	PAD_NC(GPP_C22, NONE),
	PAD_NC(GPP_C23, NONE),

	PAD_CFG_GPO(GPP_D00, 1, PLTRST), // SB_BLON
	PAD_CFG_GPO(GPP_D01, 1, PLTRST), // SSD2_PWR_EN
	PAD_CFG_GPO(GPP_D02, 1, PLTRST), // M2_SSD1_RST#
	PAD_NC(GPP_D03, NONE),
	PAD_NC(GPP_D04, NONE),
	PAD_CFG_GPO(GPP_D05, 1, PLTRST), // SSD1_PWR_EN
	PAD_NC(GPP_D06, NONE),
	PAD_NC(GPP_D07, NONE),
	PAD_NC(GPP_D08, NONE),
	PAD_NC(GPP_D09, NONE),
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF1), // HDA_BITCLK
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1), // HDA_SYNC
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1), // HDA_SDOUT / CPU_ME_WE
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1), // HDA_SDIN0
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_CFG_GPO(GPP_D16, 0, DEEP), // PCH_MUTE#
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1), // HDA_RST#
	PAD_NC(GPP_D18, NONE),
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1), // SSD2_CLKREQ
	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1), // SSD1_CLKREQ
	PAD_NC(GPP_D21, NONE),
	PAD_CFG_NF(GPP_D22, NATIVE, DEEP, NF1), // 1.1k pull to 1.8V
	PAD_CFG_NF(GPP_D23, NATIVE, DEEP, NF1), // 1.1k pull to 1.8V

	PAD_NC(GPP_E00, NONE),
	_PAD_CFG_STRUCT(GPP_E01, 0x40100100, 0x1000), // TPM_PIRQ#
	PAD_CFG_GPI(GPP_E02, NONE, DEEP), // BOARD_ID2
	PAD_NC(GPP_E03, NONE), // CNVI_WAKE#
	PAD_NC(GPP_E04, NONE),
	PAD_NC(GPP_E05, NONE),
	PAD_CFG_GPI(GPP_E06, NONE, DEEP), // JTAG ODT strap
	PAD_NC(GPP_E07, NONE),
	PAD_NC(GPP_E08, NONE),
	PAD_CFG_GPI(GPP_E09, NONE, DEEP), // USB_OC0#
	PAD_NC(GPP_E10, NONE),
	PAD_CFG_GPI(GPP_E11, NONE, DEEP), // BOARD_ID1
	PAD_NC(GPP_E12, NONE),
	PAD_CFG_GPI(GPP_E13, NONE, DEEP), // BOARD_ID3
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1), // CPU_EDP_HPD
	PAD_NC(GPP_E15, NONE),
	PAD_NC(GPP_E16, NONE),
	PAD_NC(GPP_E17, NONE),
	// GPP_E18 missing
	// GPP_E19 missing
	// GPP_E20 missing
	// GPP_E21 missing
	PAD_NC(GPP_E22, NONE),

	PAD_CFG_NF(GPP_F00, NONE, DEEP, NF1), // CNVI_BRI_DT
	PAD_CFG_NF(GPP_F01, UP_20K, DEEP, NF1), // CNVI_BRI_RSP
	PAD_CFG_NF(GPP_F02, NONE, DEEP, NF1), // CNVI_RGI_DT
	PAD_CFG_NF(GPP_F03, UP_20K, DEEP, NF1), // CNVI_RGI_RSP
	PAD_CFG_NF(GPP_F04, NONE, DEEP, NF1), // CNVI_RST#
	PAD_CFG_NF(GPP_F05, NONE, DEEP, NF3), // CNVI_CLKREQ
	PAD_CFG_NF(GPP_F06, NONE, DEEP, NF1), // CNVI_GNSS_PA_BLANKING
	PAD_NC(GPP_F07, NONE),
	PAD_NC(GPP_F08, NONE),
	PAD_CFG_GPI(GPP_F09, NONE, DEEP), // TPM_DET
	PAD_NC(GPP_F10, NONE), // WAKE_ON_WWAN_N
	PAD_NC(GPP_F11, NONE),
	_PAD_CFG_STRUCT(GPP_F12, 0x44002300, 0x0000), // AMP_SMB_CLK
	_PAD_CFG_STRUCT(GPP_F13, 0x44002300, 0x0000), // AMP_SMB_DATA
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
	PAD_NC(GPP_F17, NONE),
	PAD_CFG_GPO(GPP_F18, 0, DEEP), // CCD_WP#
	PAD_NC(GPP_F19, NONE),
	PAD_CFG_GPI(GPP_F20, NONE, PLTRST), // VRs supporting SVID strap
	PAD_NC(GPP_F21, NONE),
	PAD_NC(GPP_F22, NONE),
	PAD_NC(GPP_F23, NONE),

	PAD_CFG_GPO(GPP_H00, 0, PLTRST), // MAFS strap
	PAD_CFG_GPO(GPP_H01, 0, PLTRST), // Flash Descriptor Recovery strap
	PAD_CFG_GPO(GPP_H02, 1, PLTRST), // WLAN_RST#
	// GPP_H03 missing
	PAD_NC(GPP_H04, NONE), // CNVI_MFUART2_RXD
	PAD_NC(GPP_H05, NONE), // CNVI_MFUART2_TXD
	PAD_NC(GPP_H06, NONE),
	PAD_NC(GPP_H07, NONE),
	PAD_NC(GPP_H08, NONE),
	PAD_NC(GPP_H09, NONE),
	PAD_NC(GPP_H10, NONE),
	PAD_NC(GPP_H11, NONE),
	// GPP_H12 missing
	PAD_CFG_NF(GPP_H13, NONE, DEEP, NF1), // CPU_C10_GATE#
	PAD_NC(GPP_H14, NONE),
	PAD_NC(GPP_H15, NONE),
	PAD_NC(GPP_H16, NONE),
	PAD_NC(GPP_H17, NONE),
	// GPP_H18 missing
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1), // I2C_SDA_TP
	PAD_CFG_NF(GPP_H20, NONE, DEEP, NF1), // I2C_SCL_TP
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1), // PCH_I2C_SDA
	PAD_CFG_NF(GPP_H22, NONE, DEEP, NF1), // PCH_I2C_SCL

	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_CFG_GPO(GPP_S02, 0, PLTRST), // DMIC_CLK
	PAD_CFG_GPO(GPP_S03, 0, PLTRST), // DMIC_DATA
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),

	PAD_CFG_NF(GPP_V00, NONE, DEEP, NF1), // 100k pull to 1.8V
	PAD_CFG_NF(GPP_V01, NONE, DEEP, NF1), // AC_PRESENT
	PAD_CFG_NF(GPP_V02, NONE, DEEP, NF1), // 10k pull to 1.8V
	PAD_CFG_NF(GPP_V03, NONE, DEEP, NF1), // CPU_PWR_BTN#
	PAD_CFG_NF(GPP_V04, NONE, DEEP, NF1), // SUSB#_PCH
	PAD_CFG_NF(GPP_V05, UP_20K, DEEP, NF1), // SUSC#_PCH
	PAD_CFG_NF(GPP_V06, NATIVE, DEEP, NF1), // SLP_A#
	// GPP_V07 missing
	PAD_CFG_NF(GPP_V08, NONE, DEEP, NF1), // SUS_CLK
	PAD_CFG_NF(GPP_V09, NONE, DEEP, NF1), // SLP_WLAN#
	PAD_NC(GPP_V10, NONE),
	PAD_NC(GPP_V11, NONE),
	PAD_NC(GPP_V12, NONE),
	// GPP_V13 missing
	PAD_CFG_NF(GPP_V14, NONE, DEEP, NF1), // PCH_WAKE#
	// GPP_V15 missing
	// GPP_V16 missing
	// GPP_V17 missing
	// GPP_V18 missing
	// GPP_V19 missing
	// GPP_V20 missing
	// GPP_V21 missing
	PAD_NC(GPP_V22, NONE),
	PAD_NC(GPP_V23, NONE),
};

void mainboard_configure_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
