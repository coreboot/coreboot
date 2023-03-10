#ifndef GPIO_NAMES_JASPERLAKE
#define GPIO_NAMES_JASPERLAKE

#include "gpio_groups.h"

/* ----------------------------- Jasper Lake ----------------------------- */

const char *const jasperlake_pch_group_a_names[] = {
	"GPP_A00",		"ESPI_IO_0",		"n/a",
	"GPP_A01",		"ESPI_IO_1",		"n/a",
	"GPP_A02",		"ESPI_IO_2",		"n/a",
	"GPP_A03",		"ESPI_IO_3",		"n/a",
	"GPP_A04",		"ESPI_CS_N",		"n/a",
	"GPP_A05",		"ESPI_CLK",		"n/a",
	"GPP_A06",		"ESPI_RESET_N",		"n/a",
	"GPP_A07",		"SMB_CLK",		"n/a",
	"GPP_A08",		"SMB_DATA",		"n/a",
	"GPP_A09",		"SMB_ALERT_N",		"n/a",
	"GPP_A10",		"CPU_GP_0",		"n/a",
	"GPP_A11",		"CPU_GP_1",		"n/a",
	"GPP_A12",		"USB_OC1_N",		"n/a",
	"GPP_A13",		"USB_OC2_N",		"n/a",
	"GPP_A14",		"USB_OC3_N",		"n/a",
	"GPP_A15",		"n/a",	   	 	"n/a",
	"GPP_A16",		"DDI1_HPD",		"TIME_SYNC_1",
	"GPP_A17",		"DDI0_HPD",		"n/a",
	"GPP_A18",		"USB_OC0_N",		"n/a",
	"GPP_A19",		"PCHHOT_N",		"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",
};

const struct gpio_group jasperlake_pch_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_a_names) / 3,
	.func_count	= 3,
	.pad_names	= jasperlake_pch_group_a_names,
};

const char *const jasperlake_pch_group_b_names[] = {
	"GPP_B00",		"PMC_CORE_VID0",	"n/a",		"n/a",
	"GPP_B01",		"PMC_CORE_VID1",	"n/a",		"n/a",
	"GPP_B02",		"PMC_VRALERT_N",	"n/a",		"n/a",
	"GPP_B03",		"CPU_GP_2",		"n/a",		"n/a",
	"GPP_B04",		"CPU_GP_3",		"n/a",		"n/a",
	"GPP_B05",		"PCIE_CLKREQ0_N",	"n/a",		"n/a",
	"GPP_B06",		"PCIE_CLKREQ1_N",	"n/a",		"n/a",
	"GPP_B07",		"PCIE_CLKREQ2_N",	"n/a",		"n/a",
	"GPP_B08",		"PCIE_CLKREQ3_N",	"n/a",		"n/a",
	"GPP_B09",		"PCIE_CLKREQ4_N",	"n/a",		"n/a",
	"GPP_B10",		"PCIE_CLKREQ5_N",	"n/a",		"n/a",
	"GPP_B11",		"PMCALERT_N",		"n/a",		"n/a",
	"GPP_B12",		"PMC_SLP_S0_N",		"n/a",		"n/a",
	"GPP_B13",		"PMC_PLTRST_N",		"n/a",		"n/a",
	"GPP_B14",		"SPKR",			"GSPI0_CS1_N",	"n/a",
	"GPP_B15",		"GSPI0_CS0_N",		"n/a",		"n/a",
	"GPP_B16",		"GSPI0_CLK",		"n/a",		"n/a",
	"GPP_B17",		"GSPI0_MISO",		"UART2A_RXD",	"n/a",
	"GPP_B18",		"GSPI0_MOSI",		"UART2A_TXD",	"n/a",
	"GPP_B19",		"GSPI1_CS0_N",		"n/a",		"n/a",
	"GPP_B20",		"GSPI1_CLK",		"NFC_CLK",	"n/a",
	"GPP_B21",		"GSPI1_MISO",		"NFC_CLKREQ",	"n/a",
	"GPP_B22",		"GSPI1_MOSI",		"n/a",		"n/a",
	"GPP_B23",		"DDI2_HPD",		"TIME_SYNC_0",	"GSPI1_CS1_N",
	"GSPI0_CLK_LOOPBK",	"GSPI0_CLK_LOOPBK",	"n/a",		"n/a",
	"GSPI1_CLK_LOOPBK",	"GSPI1_CLK_LOOPBK",	"n/a",		"n/a",
};

const struct gpio_group jasperlake_pch_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_b_names) / 4,
	.func_count	= 4,
	.pad_names	= jasperlake_pch_group_b_names,
};

const char *const jasperlake_pch_group_c_names[] = {
	"GPP_C00",	"n/a",		"n/a",					"n/a",
	"GPP_C01",	"n/a",		"n/a",					"n/a",
	"GPP_C02",	"n/a",		"n/a",					"n/a",
	"GPP_C03",	"n/a",		"n/a",					"n/a",
	"GPP_C04",	"n/a",		"n/a",					"n/a",
	"GPP_C05",	"n/a",		"n/a",					"n/a",
	"GPP_C06",	"n/a",		"PMC_SUSWARN_N_PMC_SUSPWRDNACK",	"n/a",
	"GPP_C07",	"n/a",		"PMC_SUSACK_N",				"n/a",
	"GPP_C08",	"UART0_RXD",	"n/a",					"n/a",
	"GPP_C09",	"UART0_TXD",	"n/a",					"n/a",
	"GPP_C10",	"UART0_RTS_N",	"n/a",					"n/a",
	"GPP_C11",	"UART0_CTS_N",	"n/a",					"n/a",
	"GPP_C12",	"UART1_RXD",  	"n/a",					"n/a",
	"GPP_C13",	"UART1_TXD",	"n/a",					"n/a",
	"GPP_C14",	"UART1_RTS_N",	"n/a",					"n/a",
	"GPP_C15",	"UART1_CTS_N",	"n/a",					"n/a",
	"GPP_C16",	"I2C0_SDA",	"n/a",					"n/a",
	"GPP_C17",	"I2C0_SCL",	"n/a",					"n/a",
	"GPP_C18",	"I2C1_SDA",	"n/a",					"n/a",
	"GPP_C19",	"I2C1_SCL",	"n/a",					"n/a",
	"GPP_C20",	"UART2_RXD",	"n/a",					"CNV_MFUART0_RXD",
	"GPP_C21",	"UART2_TXD",	"n/a",					"CNV_MFUART0_TXD",
	"GPP_C22",	"UART2_RTS_N",	"n/a",					"CNV_MFUART0_RTS_N",
	"GPP_C23",	"UART2_CTS_N",	"n/a",					"CNV_MFUART0_CTS_N",
};

const struct gpio_group jasperlake_pch_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_c_names) / 4,
	.func_count	= 4,
	.pad_names	= jasperlake_pch_group_c_names,
};

const char *const jasperlake_pch_group_d_names[] = {
	"GPP_D00",		"n/a",			"BK_0",			"SBK_0",	"n/a",
	"GPP_D01",		"n/a",			"BK_1",			"SBK_1",	"n/a",
	"GPP_D02",		"n/a",			"BK_2",			"SBK_2",	"n/a",
	"GPP_D03",		"n/a",			"BK_3",			"SBK_3",	"n/a",
	"GPP_D04",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D05",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D06",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D07",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D08",		"n/a",			"GSPI2_SPI2_CS0_N",	"UART0A_RXD",	"n/a",
	"GPP_D09",		"n/a",			"GSPI2_CLK",		"UART0A_TXD",	"n/a",
	"GPP_D10",		"n/a",			"GSPI2_MISO",		"UART0A_RTS_N",	"n/a",
	"GPP_D11",		"n/a",			"GSPI2_MOSI",		"UART0A_CTS_N",	"n/a",
	"GPP_D12",		"n/a",			"n/a",			"I2C4B_SDA",	"n/a",
	"GPP_D13",		"n/a",			"n/a",			"I2C4B_SCL",	"n/a",
	"GPP_D14",		"n/a",			"GSPI2_CS1_N",		"n/a",		"n/a",
	"GPP_D15",		"n/a",			"n/a",			"n/a",		"CNV_WCEN",
	"GPP_D16",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D17",		"n/a",			"n/a",			"n/a",		"n/a",
	"GPP_D18",		"AVS_I2S_MCLK",		"n/a",			"n/a",		"n/a",
	"GPP_D19",		"CNV_MFUART2_RXD",	"n/a",			"n/a",		"n/a",
	"GPP_D20",		"CNV_MFUART2_TXD",	"n/a",			"n/a",		"n/a",
	"GPP_D21",		"CNV_PA_BLANKING",	"n/a",			"n/a",		"n/a",
	"GPP_D22",		"I2C5_SDA",		"n/a",			"n/a",		"n/a",
	"GPP_D23",		"I2C5_SCL",		"n/a",			"n/a",		"n/a",
	"GSPI2_CLK_LOOPBK",	"GSPI2_CLK_LOOPBK",	"n/a",			"n/a",		"n/a",
	"SPI1_CLK_LOOPBK",	"SPI1_CLK_LOOPBK",	"n/a",			"n/a",		"n/a",

};

const struct gpio_group jasperlake_pch_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_d_names) / 5,
	.func_count	= 5,
	.pad_names	= jasperlake_pch_group_d_names,
};


const char *const jasperlake_pch_hvcmos_names[] = {
	"L_BKLTEN",	"L_BKLTEN",
	"L_BKLTCTL",	"L_BKLTCTL",
	"LVDDEN",	"LVDDEN",
	"SYS_PWROK",	"SYS_PWROK",
	"SYS_RESETB",	"SYS_RESETB",
	"MLK_RSTB",	"MLK_RSTB",
};

const struct gpio_group jasperlake_pch_hvcmos = {
	.display	= "------- GPIO Group HVCMOS -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_hvcmos_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_hvcmos_names,
	};

const char *const jasperlake_pch_group_e_names[] = {
	"GPP_E00",	 "n/a",			"IMGCLKOUT_0",		"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E01",	 "n/a",			"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E02",	 "IMGCLKOUT_1",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E03",	 "n/a",			"SATA_0_DEVSLP",	"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E04",	 "IMGCLKOUT_2",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E05",	 "SATA_LED_N",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E06",	 "IMGCLKOUT_3",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E07",	 "n/a",			"SATA_1_DEVSLP",	"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E08",	 "n/a",			"Reserved",		"n/a",	"n/a",			"SATA_0_GP",	"n/a",
	"GPP_E09",	 "n/a",			"SML_CLK0",		"n/a",	"n/a",			"SATA_1_GP",	"n/a",
	"GPP_E10",	 "n/a",			"SML_DATA0",		"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E11",	 "n/a",			"n/a",			"n/a",	"n/a",			"n/a",		"BSSB_LS3_RX",
	"GPP_E12",	 "IMGCLKOUT_4",		"n/a",			"n/a",	"n/a",			"n/a",		"BSSB_LS3_TX",
	"GPP_E13",	 "DDI0_DDC_SCL",	"n/a",			"n/a",	"n/a",			"BSSB_LS0_RX",	"n/a",
	"GPP_E14",	 "DDI0_DDC_SDA",	"n/a",			"n/a",	"n/a",			"BSSB_LS0_TX",	"n/a",
	"GPP_E15",	 "DDI1_DDC_SCL",	"n/a",			"n/a",	"n/a",			"BSSB_LS1_RX",	"n/a",
	"GPP_E16",	 "DDI1_DDC_SDA",	"n/a",			"n/a",	"n/a",			"BSSB_LS1_TX",	"n/a",
	"GPP_E17",	 "DDI2_DDC_SCL",	"n/a",			"n/a",	"n/a",			"BSSB_LS2_RX",	"n/a",
	"GPP_E18",	 "DDI2_DDC_SDA",	"n/a",			"n/a",	"n/a",			"BSSB_LS2_TX",	"n/a",
	"GPP_E19",	 "IMGCLKOUT_5",		"Reserved",		"n/a",	"PCIE_LNK_DOWN",	"n/a",		"n/a",
	"GPP_E20",	 "CNV_BRI_DT",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E21",	 "CNV_BRI_RSP",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E22",	 "CNV_RGI_DT",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
	"GPP_E23",	 "CNV_RGI_RSP",		"n/a",			"n/a",	"n/a",			"n/a",		"n/a",
};

const struct gpio_group jasperlake_pch_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_e_names) / 7,
	.func_count	= 7,
	.pad_names	= jasperlake_pch_group_e_names,
};

const char *const jasperlake_pch_jtag_names[] = {
      	"JTAG_TD0",	"JTAG_TD0",
      	"JTAGX",   	"JTAGX",
      	"PRDYB",   	"PRDYB",
      	"PREQB",   	"PREQB",
      	"CPU_TRSTB",	"CPU_TRSTB",
      	"JTAG_TDI",	"JTAG_TDI",
      	"JTAG_TMS",	"JTAG_TMS",
      	"JTAG_TCK",	"JTAG_TCK",
      	"ITP_PMODE",	"ITP_PMODE",
};

const struct gpio_group jasperlake_pch_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_jtag_names,
};

const char *const jasperlake_pch_group_f_names[] = {
	"GPP_F00",	"n/a",
	"GPP_F01",	"n/a",
	"GPP_F02",	"n/a",
	"GPP_F03",	"n/a",
	"GPP_F04",	"CNV_RF_RESET_N",
	"GPP_F05",	"n/a",
	"GPP_F06",	"n/a",
	"GPP_F07",	"EMMC_CMD",
	"GPP_F08",	"EMMC_DATA0",
	"GPP_F09",	"EMMC_DATA1",
	"GPP_F10",	"EMMC_DATA2",
	"GPP_F11",	"EMMC_DATA3",
	"GPP_F12",	"EMMC_DATA4",
	"GPP_F13",	"EMMC_DATA5",
	"GPP_F14",	"EMMC_DATA6",
	"GPP_F15",	"EMMC_DATA7",
	"GPP_F16",	"EMMC_RCLK",
	"GPP_F17",	"EMMC_CLK",
	"GPP_F18",	"EMMC_RESET_N",
	"GPP_F19",	"n/a",
};

const struct gpio_group jasperlake_pch_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_f_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_f_names,
};

const char *const jasperlake_pch_group_g_names[] = {
	"GPP_G00",	"SD_SDIO_CMD",
	"GPP_G01", 	"SD_SDIO_D0",
	"GPP_G02", 	"SD_SDIO_D1",
	"GPP_G03", 	"SD_SDIO_D2",
	"GPP_G04", 	"SD_SDIO_D3",
	"GPP_G05", 	"SD_SDIO_CD_N",
	"GPP_G06", 	"SD_SDIO_CLK",
	"GPP_G07", 	"SD_SDIO_WP",
};

const struct gpio_group jasperlake_pch_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_g_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_g_names,
};

const char *const jasperlake_pch_group_h_names[] = {
	"GPP_H00",	"n/a",			"n/a",			"n/a",
	"GPP_H01", 	"SD_SDIO_PWR_EN_N",	"n/a",			"CNV_RF_RESET_N",
	"GPP_H02", 	"n/a",			"n/a",			"MODEM_CLKREQ",
	"GPP_H03", 	"SX_EXIT_HOLDOFF_N",	"n/a",			"n/a",
	"GPP_H04", 	"I2C2_SDA",		"n/a",			"n/a",
	"GPP_H05", 	"I2C2_SCL",		"n/a",			"n/a",
	"GPP_H06", 	"I2C3_SDA",		"n/a",			"n/a",
	"GPP_H07", 	"I2C3_SCL",		"n/a",			"n/a",
	"GPP_H08", 	"I2C4_SDA",		"n/a",			"n/a",
	"GPP_H09", 	"I2C4_SCL",		"n/a",			"n/a",
	"GPP_H10", 	"CPU_C10_GATE_N",	"n/a",			"n/a",
	"GPP_H11", 	"AVS_I2S2_SCLK",	"n/a",			"n/a",
	"GPP_H12", 	"AVS_I2S2_SFRM",	"CNV_RF_RESET_N",	"n/a",
	"GPP_H13", 	"AVS_I2S2_TXD",		"MODEM_CLKREQ",		"n/a",
	"GPP_H14", 	"AVS_I2S2_RXD",		"n/a",			"n/a",
	"GPP_H15",	"AVS_I2S1_SCLK",	"n/a",			"n/a",
	"GPP_H16", 	"n/a",			"n/a",			"n/a",
	"GPP_H17", 	"n/a",			"n/a",			"n/a",
	"GPP_H18", 	"n/a",			"n/a",			"n/a",
	"GPP_H19", 	"n/a",			"n/a",			"n/a",
	"GPP_H20", 	"n/a",			"n/a",			"n/a",
	"GPP_H21", 	"n/a",			"n/a",			"n/a",
	"GPP_H22", 	"n/a",			"n/a",			"n/a",
	"GPP_H23", 	"n/a",			"n/a",			"n/a",
};

const struct gpio_group jasperlake_pch_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_h_names) / 4,
	.func_count	= 4,
	.pad_names	= jasperlake_pch_group_h_names,
};

const char *const jasperlake_pch_group_spi_names[] = {
	"SPI0_IO_2",		"SPI0_IO_2",
	"SPI0_IO_3",		"SPI0_IO_3",
	"SPI0_MOSI_IO_0",	"SPI0_MOSI_IO_0",
	"SPI0_MISO_IO_0",	"SPI0_MISO_IO_0",
	"SPI0_TPM_CSB",		"SPI0_TPM_CSB",
	"SPI0_FLASH_0_CSB",	"SPI0_FLASH_0_CSB",
	"SPI0_FLASH_1_CSB",	"SPI0_FLASH_1_CSB",
	"SPI0_CLK",		"SPI0_CLK",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",
};

const struct gpio_group jasperlake_pch_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_spi_names,
};

const char *const jasperlake_pch_group_r_names[] = {
	"GPP_R00",	"HDA_BCLK",		"AVS_I2S0_SCLK",
	"GPP_R01",	"HDA_SYNC",		"AVS_I2S0_SFRM",
	"GPP_R02",	"HDA_SDO",		"AVS_I2S0_TXD",
	"GPP_R03",	"HDA_SDI0",		"AVS_I2S0_RXD",
	"GPP_R04",	"HDA_RST_N",		"n/a",
	"GPP_R05",	"HDA_SDI1",		"AVS_I2S1_RXD",
	"GPP_R06",	"AVS_I2S1_SFRM",	"n/a",
	"GPP_R07",	"AVS_I2S1_TXD",		"n/a",
};

const struct gpio_group jasperlake_pch_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_r_names) / 3,
	.func_count	= 3,
	.pad_names	= jasperlake_pch_group_r_names,
};

const char *const jasperlake_pch_group_s_names[] = {
	"GPP_S00",	"n/a",		"n/a",
	"GPP_S01",	"n/a",		"n/a",
	"GPP_S02",	"n/a",		"DMIC_CLK_1",
	"GPP_S03",	"n/a",		"DMIC_DATA_1",
	"GPP_S04",	"SNDW1_CLK",	"n/a",
	"GPP_S05",	"SNDW1_DATA",	"n/a",
	"GPP_S06",	"n/a",		"DMIC_CLK_0",
	"GPP_S07",	"n/a",		"DMIC_DATA_0",
};

const struct gpio_group jasperlake_pch_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_s_names) / 3,
	.func_count	= 3,
	.pad_names	= jasperlake_pch_group_s_names,
};

const char *const jasperlake_pch_group_cpu_names[] = {
	"HDACPU_SDI",    	"HDACPU_SDI",
	"HDACPU_SDO",    	"HDACPU_SDO",
	"HDACPU_SCLK",   	"HDACPU_SCLK",
	"PM_SYNC",       	"PM_SYNC",
	"PECI",          	"PECI",
	"CPUPWRGD",      	"CPUPWRGD",
	"THRMTRIPB",     	"THRMTRIPB",
	"PLTRST_CPUB",   	"PLTRST_CPUB",
	"PM_DOWN",       	"PM_DOWN",
	"TRIGGER_IN",    	"TRIGGER_IN",
	"TRIGGER_OUT",   	"TRIGGER_OUT",
	"UFS_RESETB",    	"UFS_RESETB",
	"CLKOUT_CPURTC", 	"CLKOUT_CPURTC",
	"VCCST_PWRGD",		"VCCST_PWRGD",
	"C10_WAKE",		"C10_WAKE",
};

const struct gpio_group jasperlake_pch_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_cpu_names,
};

const char *const jasperlake_pch_group_vgpio_pcie_names[] = {
	"VGPIO_PCIE0", 	"VGPIO_PCIE0",
	"VGPIO_PCIE1", 	"VGPIO_PCIE1",
	"VGPIO_PCIE2", 	"VGPIO_PCIE2",
	"VGPIO_PCIE3", 	"VGPIO_PCIE3",
	"VGPIO_PCIE4", 	"VGPIO_PCIE4",
	"VGPIO_PCIE5", 	"VGPIO_PCIE5",
	"VGPIO_PCIE6", 	"VGPIO_PCIE6",
	"VGPIO_PCIE7", 	"VGPIO_PCIE7",
	"VGPIO_PCIE8", 	"VGPIO_PCIE8",
	"VGPIO_PCIE9", 	"VGPIO_PCIE9",
	"VGPIO_PCIE10",	"VGPIO_PCIE10",
	"VGPIO_PCIE11",	"VGPIO_PCIE11",
	"VGPIO_PCIE12",	"VGPIO_PCIE12",
	"VGPIO_PCIE13",	"VGPIO_PCIE13",
	"VGPIO_PCIE14",	"VGPIO_PCIE14",
	"VGPIO_PCIE15",	"VGPIO_PCIE15",
	"VGPIO_PCIE16",	"VGPIO_PCIE16",
	"VGPIO_PCIE17",	"VGPIO_PCIE17",
	"VGPIO_PCIE18",	"VGPIO_PCIE18",
	"VGPIO_PCIE19",	"VGPIO_PCIE19",
	"VGPIO_PCIE20",	"VGPIO_PCIE20",
	"VGPIO_PCIE21",	"VGPIO_PCIE21",
	"VGPIO_PCIE22",	"VGPIO_PCIE22",
	"VGPIO_PCIE23",	"VGPIO_PCIE23",
	"VGPIO_PCIE24",	"VGPIO_PCIE24",
	"VGPIO_PCIE25",	"VGPIO_PCIE25",
	"VGPIO_PCIE26",	"VGPIO_PCIE26",
	"VGPIO_PCIE27",	"VGPIO_PCIE27",
	"VGPIO_PCIE28",	"VGPIO_PCIE28",
	"VGPIO_PCIE29",	"VGPIO_PCIE29",
	"VGPIO_PCIE30",	"VGPIO_PCIE30",
	"VGPIO_PCIE31",	"VGPIO_PCIE31",
	"VGPIO_PCIE32",	"VGPIO_PCIE32",
	"VGPIO_PCIE33",	"VGPIO_PCIE33",
	"VGPIO_PCIE34",	"VGPIO_PCIE34",
	"VGPIO_PCIE35",	"VGPIO_PCIE35",
	"VGPIO_PCIE36",	"VGPIO_PCIE36",
	"VGPIO_PCIE37",	"VGPIO_PCIE37",
	"VGPIO_PCIE38",	"VGPIO_PCIE38",
	"VGPIO_PCIE39",	"VGPIO_PCIE39",
	"VGPIO_PCIE40",	"VGPIO_PCIE40",
	"VGPIO_PCIE41",	"VGPIO_PCIE41",
	"VGPIO_PCIE42",	"VGPIO_PCIE42",
	"VGPIO_PCIE43",	"VGPIO_PCIE43",
	"VGPIO_PCIE44",	"VGPIO_PCIE44",
	"VGPIO_PCIE45",	"VGPIO_PCIE45",
	"VGPIO_PCIE46",	"VGPIO_PCIE46",
	"VGPIO_PCIE47",	"VGPIO_PCIE47",
	"VGPIO_PCIE48",	"VGPIO_PCIE48",
	"VGPIO_PCIE49",	"VGPIO_PCIE49",
	"VGPIO_PCIE50",	"VGPIO_PCIE50",
	"VGPIO_PCIE51",	"VGPIO_PCIE51",
	"VGPIO_PCIE52",	"VGPIO_PCIE52",
	"VGPIO_PCIE53",	"VGPIO_PCIE53",
	"VGPIO_PCIE54",	"VGPIO_PCIE54",
	"VGPIO_PCIE55",	"VGPIO_PCIE55",
	"VGPIO_PCIE56",	"VGPIO_PCIE56",
	"VGPIO_PCIE57",	"VGPIO_PCIE57",
	"VGPIO_PCIE58",	"VGPIO_PCIE58",
	"VGPIO_PCIE59",	"VGPIO_PCIE59",
	"VGPIO_PCIE60",	"VGPIO_PCIE60",
	"VGPIO_PCIE61",	"VGPIO_PCIE61",
	"VGPIO_PCIE62",	"VGPIO_PCIE62",
	"VGPIO_PCIE63",	"VGPIO_PCIE63",
	"VGPIO_PCIE64",	"VGPIO_PCIE64",
	"VGPIO_PCIE65",	"VGPIO_PCIE65",
	"VGPIO_PCIE66",	"VGPIO_PCIE66",
	"VGPIO_PCIE67",	"VGPIO_PCIE67",
	"VGPIO_PCIE68",	"VGPIO_PCIE68",
	"VGPIO_PCIE69",	"VGPIO_PCIE69",
	"VGPIO_PCIE70",	"VGPIO_PCIE70",
	"VGPIO_PCIE71",	"VGPIO_PCIE71",
	"VGPIO_PCIE72",	"VGPIO_PCIE72",
	"VGPIO_PCIE73",	"VGPIO_PCIE73",
	"VGPIO_PCIE74",	"VGPIO_PCIE74",
	"VGPIO_PCIE75",	"VGPIO_PCIE75",
	"VGPIO_PCIE76",	"VGPIO_PCIE76",
	"VGPIO_PCIE77",	"VGPIO_PCIE77",
	"VGPIO_PCIE78",	"VGPIO_PCIE78",
	"VGPIO_PCIE79",	"VGPIO_PCIE79",
};
const struct gpio_group jasperlake_pch_group_vgpio_pcie = {
	.display	= "------- GPIO Group VGPIO_PCIE -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_vgpio_pcie_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_vgpio_pcie_names,
};

const char *const jasperlake_pch_group_vgpio_usb_names[] = {
	"VGPIO_USB0",  	"VGPIO_USB0",
	"VGPIO_USB1",  	"VGPIO_USB1",
	"VGPIO_USB2",  	"VGPIO_USB2",
	"VGPIO_USB3",  	"VGPIO_USB3",
	"VGPIO_USB4",  	"VGPIO_USB4",
	"VGPIO_USB5",  	"VGPIO_USB5",
	"VGPIO_USB6",  	"VGPIO_USB6",
	"VGPIO_USB7",  	"VGPIO_USB7",
	"VGPIO_USB8",  	"VGPIO_USB8",
	"VGPIO_USB9",  	"VGPIO_USB9",
	"VGPIO_USB10", 	"VGPIO_USB10",
	"VGPIO_USB11", 	"VGPIO_USB11",
};

const struct gpio_group jasperlake_pch_group_vgpio_usb = {
	.display	= "------- GPIO Group VGPIO_USB -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_vgpio_usb_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_vgpio_usb_names,
};

const char *const jasperlake_pch_vgpio_lnk_dn_names[] = {
	"VGPIO_LNK_DN_0",	"VGPIO_LNK_DN_0",
	"VGPIO_LNK_DN_1",	"VGPIO_LNK_DN_1",
	"VGPIO_LNK_DN_2",	"VGPIO_LNK_DN_2",
	"VGPIO_LNK_DN_3",	"VGPIO_LNK_DN_3",
};

const struct gpio_group jasperlake_pch_vgpio_lnk_dn = {
	.display	= "------- GPIO Group VGPIO_LNK -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_vgpio_lnk_dn_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_vgpio_lnk_dn_names,
};

const char *const jasperlake_pch_group_vgpio_names [] ={
	"VGPIO0",	"VGPIO0",
	"VGPIO3",	"VGPIO3",
	"VGPIO4",	"VGPIO4",
	"VGPIO5",	"VGPIO5",
	"VGPIO6",	"VGPIO6",
	"VGPIO7",	"VGPIO7",
	"VGPIO8",	"VGPIO8",
	"VGPIO9",	"VGPIO9",
	"VGPIO10",	"VGPIO10",
	"VGPIO11",	"VGPIO11",
	"VGPIO12",	"VGPIO12",
	"VGPIO13",	"VGPIO13",
	"VGPIO18",	"VGPIO18",
	"VGPIO19",	"VGPIO19",
	"VGPIO20",	"VGPIO20",
	"VGPIO21",	"VGPIO21",
	"VGPIO22",	"VGPIO22",
	"VGPIO23",	"VGPIO23",
	"VGPIO24",	"VGPIO24",
	"VGPIO25",	"VGPIO25",
	"VGPIO30",	"VGPIO30",
	"VGPIO31",	"VGPIO31",
	"VGPIO32",	"VGPIO32",
	"VGPIO33",	"VGPIO33",
	"VGPIO34",	"VGPIO34",
	"VGPIO35",	"VGPIO35",
	"VGPIO36",	"VGPIO36",
	"VGPIO37",	"VGPIO37",
	"VGPIO39",	"VGPIO39",
};

const struct gpio_group jasperlake_pch_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_vgpio_names,
};

const char *const jasperlake_pch_group_gpd_names[] = {
	"GPD0",		"BATLOW#",
	"GPD1",		"PMC_ACPRESENT",
	"GPD2",		"LAN_WAKE#",
	"GPD3",		"PWRBTN#",
	"GPD4",		"SLP_S3#",
	"GPD5",		"SLP_S4#",
	"GPD6",		"SLP_A#",
	"GPD7",		"n/a",
	"GPD8",		"SUSCLK",
	"GPD9",		"SLP_WLAN#",
	"GPD10",	"SLP_S5#",
	"INPUT3VSEL",	"INPUT3VSEL",
	"SLP_SUS#",	"SLP_SUS#",
	"WAKE#",	"WAKE#",
	"DRAM_RESET#",	"DRAM_RESET#",
};

const struct gpio_group jasperlake_pch_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(jasperlake_pch_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= jasperlake_pch_group_gpd_names,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 1				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_0_groups[] = {
	&jasperlake_pch_group_g,
};

const struct gpio_community jasperlake_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(jasperlake_community_0_groups),
	.groups		= jasperlake_community_0_groups,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 2				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_1_groups[] = {
	&jasperlake_pch_hvcmos,
	&jasperlake_pch_group_e,
	&jasperlake_pch_jtag,
	&jasperlake_pch_vgpio_lnk_dn,
};

const struct gpio_community jasperlake_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(jasperlake_community_1_groups),
	.groups		= jasperlake_community_1_groups,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 3				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_2_groups[] = {
	&jasperlake_pch_group_cpu,
	&jasperlake_pch_group_vgpio_pcie,
	&jasperlake_pch_group_vgpio_usb,
};

const struct gpio_community jasperlake_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(jasperlake_community_2_groups),
	.groups		= jasperlake_community_2_groups,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 4				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_3_groups[] = {
	&jasperlake_pch_group_gpd,
};

const struct gpio_community jasperlake_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(jasperlake_community_3_groups),
	.groups		= jasperlake_community_3_groups,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 5				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_4_groups[] = {
	&jasperlake_pch_group_h,
	&jasperlake_pch_group_d,
	&jasperlake_pch_group_vgpio,
	&jasperlake_pch_group_c,
};

const struct gpio_community jasperlake_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(jasperlake_community_4_groups),
	.groups		= jasperlake_community_4_groups,
};

/* ------------------------------------------------------------ */
/* 			GPIO Group 6				*/
/* ------------------------------------------------------------ */
const struct gpio_group *const jasperlake_community_5_groups[] = {
	&jasperlake_pch_group_f,
	&jasperlake_pch_group_spi,
	&jasperlake_pch_group_b,
	&jasperlake_pch_group_a,
	&jasperlake_pch_group_s,
	&jasperlake_pch_group_r,
};

const struct gpio_community jasperlake_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(jasperlake_community_5_groups),
	.groups		= jasperlake_community_5_groups,
};

const struct gpio_community *const jasperlake_pch_communities[] = {
	&jasperlake_community_0,
	&jasperlake_community_1,
	&jasperlake_community_2,
	&jasperlake_community_3,
	&jasperlake_community_4,
	&jasperlake_community_5,
};

#endif
