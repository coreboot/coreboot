/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPIO_NAMES_ELKHARTLAKE
#define GPIO_NAMES_ELKHARTLAKE

#include "gpio_groups.h"

/* ----------------------------- Elkhart Lake ----------------------------- */

const char *const elkhartlake_pch_group_a_names[] = {
	"GPP_A0",	"PSE_GBE0_RGMII_TXD3",	"n/a",
	"GPP_A1",	"PSE_GBE0_RGMII_TXD2",	"n/a",
	"GPP_A2",	"PSE_GBE0_RGMII_TXD1",	"n/a",
	"GPP_A3",	"PSE_GBE0_RGMII_TXD0",	"n/a",
	"GPP_A4",	"PSE_GBE0_RGMII_TXCLK",	"n/a",
	"GPP_A5",	"PSE_GBE0_RGMII_TXCTL",	"n/a",
	"GPP_A6",	"PSE_GBE0_RGMII_RXCLK",	"n/a",
	"GPP_A7",	"PSE_GBE0_RGMII_RXD3",	"n/a",
	"GPP_A8",	"PSE_GBE0_RGMII_RXD2",	"n/a",
	"GPP_A9",	"PSE_GBE0_RGMII_RXD1",	"n/a",
	"GPP_A10",	"PSE_GBE0_RGMII_RXD0",	"n/a",
	"GPP_A11",	"PSE_GBE1_RGMII_TXD3",	"n/a",
	"GPP_A12",	"PSE_GBE1_RGMII_TXD2",	"n/a",
	"GPP_A13",	"PSE_GBE1_RGMII_TXD1",	"n/a",
	"GPP_A14",	"PSE_GBE1_RGMII_TXD0",	"n/a",
	"GPP_A15",	"PSE_GBE1_RGMII_TXCLK",	"n/a",
	"GPP_A16",	"PSE_GBE1_RGMII_TXCTL",	"n/a",
	"GPP_A17",	"PSE_GBE1_RGMII_RXCLK",	"n/a",
	"GPP_A18",	"PSE_GBE1_RGMII_RXCTL",	"n/a",
	"GPP_A19",	"PSE_GBE1_RGMII_RXD3",	"AVS_I2S5_SCLK",
	"GPP_A20",	"PSE_GBE1_RGMII_RXD2",	"AVS_I2S5_SFRM",
	"GPP_A21",	"PSE_GBE1_RGMII_RXD1",	"AVS_I2S5_TXD",
	"GPP_A22",	"PSE_GBE1_RGMII_RXD0",	"AVS_I2S5_RXD",
	"GPP_A23",	"PSE_GBE0_RGMII_RXCTL",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_a_names) / 3,
	.func_count	= 3,
	.pad_names	= elkhartlake_pch_group_a_names,
};

const char *const elkhartlake_pch_group_b_names[] = {
	"GPP_B0"	,"PMC_CORE_VID0",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B1"	,"PMC_CORE_VID1",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B2"	,"PMC_VRALERT_N",	"n/a",			"n/a",			"ESPI_ALERT2_N",	"n/a",		"n/a",	"n/a",	"PSE_TGPIO25",
	"GPP_B3"	,"CPU_GP2",		"n/a",			"n/a",			"ESPI_ALERT0_N",	"n/a",		"n/a",	"n/a",	"PSE_TGPIO26",
	"GPP_B4"	,"CPU_GP3",		"n/a",			"n/a",			"ESPI_ALERT1_N",	"n/a",		"n/a",	"n/a",	"PSE_TGPIO27",
	"GPP_B5"	,"PSE_I2C0_SCL",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"PSE_TGPIO06",
	"GPP_B6"	,"PSE_I2C0_SDA",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"PSE_TGPIO07",
	"GPP_B7"	,"PSE_I2C1_SCL",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"PSE_TGPIO08",
	"GPP_B8"	,"PSE_I2C1_SDA",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"PSE_TGPIO09",
	"GPP_B9"	,"SIO_I2C5_SDA",	"PSE_I2C2_SDA",		"n/a",			"ESPI_CS3_N",		"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B10"	,"SIO_I2C5_SCL",	"PSE_I2C2_SCL",		"n/a",			"ESPI_ALERT3_N",	"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B11"	,"PMC_ALERT_N",		"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"PSE_TGPIO06",
	"GPP_B12"	,"PMC_SLP_S0_N",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B13"	,"PMC_PLTRST_N",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B14"	,"SPKR",		"PMC_TGPIO1",		"SIO_SPI0_CS1_N",	"PSE_SPI2_CS1_N",	"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B15"	,"SIO_SPI0_CS0_N",	"n/a",			"PSE_SPI2_CS0_N",	"n/a",			"ESPI_CS1_N",	"n/a",	"n/a",	"n/a",
	"GPP_B16"	,"SIO_SPI0_CLK",	"n/a",			"PSE_SPI2_CLK",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B17"	,"SIO_SPI0_MISO",	"n/a",			"PSE_SPI2_MISO",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B18"	,"SIO_SPI0_MOSI",	"n/a",			"PSE_SPI2_MOSI",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B19"	,"SIO_SPI1_CS0_N",	"n/a",			"PSE_SPI3_CS0_N",	"n/a",			"ESPI_CS2_N",	"n/a",	"n/a",	"n/a",
	"GPP_B20"	,"SIO_SPI1_CLK",	"RSVD",			"PSE_SPI3_CLK",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B21"	,"SIO_SPI1_MISO",	"RSVD",			"PSE_SPI3_MISO",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B22"	,"SIO_SPI1_MOSI",	"n/a",			"PSE_SPI3_MOSI",	"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPP_B23"	,"n/a",			"PCHHOT_N",		"SIO_SPI1_CS1_N",	"PSE_SPI3_CS1_N",	"n/a",		"n/a",	"n/a",	"PSE_TGPIO28",
	"GPIO_RSVD_0"	,"n/a",			"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_1"	,"n/a",			"n/a",			"n/a",			"n/a",			"n/a",		"n/a",	"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_b_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_b_names,
};

const char *const elkhartlake_pch_group_c_names[] = {
	"GPP_C0",	"SMB_CLK",		"PSE_I2C3_SCL",			"n/a",			"n/a",			"n/a",	"n/a",		"n/a",	"PSE_TGPIO18",
	"GPP_C1",	"SMB_DATA",		"PSE_I2C3_SDA",			"n/a",			"n/a",			"n/a",	"n/a",		"n/a",	"PSE_TGPIO19",
	"GPP_C2",	"PSE_PWM00",		"SMB_ALERT_N",			"n/a",			"n/a",			"n/a",	"n/a",		"n/a",	"PSE_TGPIO29",
	"GPP_C3",	"PSE_GBE0_MDC",		"n/a",				"PSE_HSUART3_EN",	"n/a",			"n/a",	"RSVD",		"n/a",	"n/a",
	"GPP_C4",	"PSE_GBE0_MDIO",	"n/a",				"PSE_UART3_RTS_N",	"PSE_HSUART3_DE",	"n/a",	"RSVD",		"n/a",	"n/a",
	"GPP_C5",	"PSE_PWM01",		"n/a",				"PSE_UART3_CTS_N",	"n/a",			"n/a",	"SML_ALERT0_N",	"n/a",	"PSE_TGPIO30",
	"GPP_C6",	"PSE_GBE1_MDC",		"n/a",				"n/a",			"n/a",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C7",	"PSE_GBE1_MDIO",	"n/a",				"PSE_HSUART3_RE",	"n/a",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C8",	"PSE_TGPIO04",		"DNX_FORCE_RELOAD",		"n/a",			"RSVD",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C9",	"PSE_HSUART0_EN",	"n/a",				"n/a",			"RSVD",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C10",	"PSE_TGPIO05",		"n/a",				"n/a",			"RSVD",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C11",	"PSE_HSUART0_RE",	"n/a",				"n/a",			"RSVD",			"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C12",	"PSE_UART0_RXD",	"n/a",				"n/a",			"SIO_UART1_RXD",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C13",	"PSE_UART0_TXD",	"n/a",				"n/a",			"SIO_UART1_TXD",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C14",	"PSE_UART0_RTS_N",	"PSE_HSUART0_DE",		"n/a",			"SIO_UART1_RTS_N",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C15",	"PSE_UART0_CTS_N",	"n/a",				"n/a",			"SIO_UART1_CTS_N",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C16",	"GBE_MDIO",		"n/a",				"PSE_UART3_RXD",	"SIO_I2C0_SDA",		"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C17",	"GBE_MDC",		"n/a",				"PSE_UART3_TXD",	"SIO_I2C0_SCL",		"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C18",	"PSE_I2C4_SDA",		"n/a",				"SML_DATA0",		"SIO_I2C1_SDA",		"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C19",	"PSE_I2C4_SCL",		"n/a",				"SML_CLK0",		"SIO_I2C1_SCL",		"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C20",	"PSE_UART4_RXD",	"n/a",				"RSVD",			"SIO_UART2_RXD",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C21",	"PSE_UART4_TXD",	"n/a",				"RSVD",			"SIO_UART2_TXD",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C22",	"PSE_UART4_RTS_N",	"A0: N/A B0: ISI_SPIM_MOSI",	"RSVD",			"SIO_UART2_RTS_N",	"n/a",	"n/a",		"n/a",	"n/a",
	"GPP_C23",	"PSE_UART4_CTS_N",	"A0: N/A B0: ISI_SPIS_MOSI",	"RSVD",			"SIO_UART2_CTS_N",	"n/a",	"n/a",		"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_c_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_c_names,
};

const char *const elkhartlake_pch_group_d_names[] = {
	"GPP_D0",	"PSE_QEPA0",		"n/a",				"RSVD",	"PSE_SPI1_CS0_N",	"n/a",	"n/a",	"n/a",			"PSE_TGPIO32",
	"GPP_D1",	"PSE_QEPB0",		"n/a",				"RSVD",	"PSE_SPI1_CLK",		"n/a",	"n/a",	"n/a",			"PSE_TGPIO33",
	"GPP_D2",	"PSE_QEPI0",		"n/a",				"RSVD",	"PSE_SPI1_MISO",	"n/a",	"n/a",	"n/a",			"PSE_TGPIO34",
	"GPP_D3",	"PSE_PWM06",		"n/a",				"RSVD",	"PSE_SPI1_MOSI",	"n/a",	"n/a",	"n/a",			"PSE_TGPIO35",
	"GPP_D4",	"PSE_PWM02",		"n/a",				"RSVD",	"PSE_SPI1_CS1_N",	"n/a",	"n/a",	"n/a",			"PSE_TGPIO36",
	"GPP_D5",	"PCIE_CLKREQ0_N",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_D6",	"PCIE_CLKREQ1_N",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_D7",	"PCIE_CLKREQ2_N",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_D8",	"PCIE_CLKREQ3_N",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_D9",	"PSE_SPI0_CS0_N",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"SIO_SPI2_CS0_N",	"PSE_TGPIO10",
	"GPP_D10",	"PSE_SPI0_CLK",		"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"SIO_SPI2_CLK",		"PSE_TGPIO11",
	"GPP_D11",	"PSE_SPI0_MISO",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"SIO_SPI2_MISO",	"PSE_TGPIO12",
	"GPP_D12",	"PSE_SPI0_MOSI",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"SIO_SPI2_MOSI",	"PSE_TGPIO13",
	"GPP_D13",	"PSE_QEPA1",		"n/a",				"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO37",
	"GPP_D14",	"PSE_QEPB1",		"n/a",				"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO38",
	"GPP_D15",	"PSE_PWM03",		"SIO_SPI2_CS1_N",		"RSVD",	"PSE_SPI0_CS1_N",	"n/a",	"n/a",	"n/a",			"PSE_TGPIO39",
	"GPP_D16",	"PSE_QEPI1",		"n/a"			,	"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO40",
	"GPP_D17",	"PSE_PWM04",		"A0: N/A B0: ISI_SPIM_MOSI",	"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO41",
	"GPP_D18",	"PSE_PWM05",		"A0: N/A B0: ISI_SPIS_MOSI",	"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO42",
	"GPP_D19",	"AVS_I2S_MCLK1",	"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"PSE_TGPIO43",
	"GPIO_RSVD_3",	"n/a",			"n/a",				"n/a",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",
};

const struct gpio_group elkhartlake_pch_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_d_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_d_names,
};

const char *const elkhartlake_pch_group_gpd_names[] = {
	"GPD0",		"PMC_BATLOW_N",
	"GPD1",		"PMC_ACPRESENT",
	"GPD2",		"RSVD",
	"GPD3",		"PMC_PWRBTN_N",
	"GPD4",		"PMC_SLP_S3_N",
	"GPD5",		"PMC_SLP_S4_N",
	"GPD7",		"RSVD",
	"GPD8",		"PMC_SUSCLK",
	"GPD9",		"RSVD",
	"GPD10",	"PMC_SLP_S5_N",
	"GPD11",	"RSVD",
	"GPIO_RSVD_8",	"n/a",
	"GPIO_RSVD_9",	"n/a",
	"GPIO_RSVD_10",	"n/a",
	"GPIO_RSVD_11",	"n/a",
	"GPIO_RSVD_12",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_gpd = {
	.display	= "------- GPIO Group GPD -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_gpd_names) / 2,
	.func_count	= 2,
	.pad_names	= elkhartlake_pch_group_gpd_names,
};

const char *const elkhartlake_pch_group_e_names[] = {
	"GPP_E0",	"SATA_LED_N",		"SATAXPCIE_0",		"RSVD",	"RSVD",	"RSVD",			"RSVD",	"SATA_0_GP",	"n/a",
	"GPP_E1",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"RSVD",		"n/a",
	"GPP_E2",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"RSVD",		"n/a",
	"GPP_E3",	"DDI1_HPD",		"PNL_MISC_DDI1",	"RSVD",	"RSVD",	"RSVD",			"RSVD",	"CPU_GP0",	"PSE_TGPIO15",
	"GPP_E4",	"SATA_0_DEVSLP",	"n/a",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"PSE_PWM08",	"PSE_TGPIO44",
	"GPP_E5",	"DDI1_DDC_SDA",		"n/a",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"PSE_PWM09",	"PSE_TGPIO17",
	"GPP_E6",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"PSE_PWM10",	"PSE_TGPIO18",
	"GPP_E7",	"DDI1_DDC_SCL",		"n/a",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"CPU_GP1",	"PSE_TGPIO16",
	"GPP_E8",	"n/a",			"SATA_1_DEVSLP",	"RSVD",	"RSVD",	"n/a",			"RSVD",	"RSVD",		"PSE_TGPIO45",
	"GPP_E9",	"USB2_OC0_N",		"n/a",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"n/a",		"n/a",
	"GPP_E10",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"RSVD"	,	"n/a",
	"GPP_E11",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"RSVD",		"n/a",
	"GPP_E12",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"RSVD",		"n/a",
	"GPP_E13",	"n/a",			"RSVD",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"RSVD",		"n/a",
	"GPP_E14",	"DDI0_HPD",		"PNL_MISC_DDI0",	"RSVD",	"RSVD",	"n/a",			"RSVD",	"n/a",		"PSE_TGPIO19",
	"GPP_E15",	"PSE_I2S0_RXD",		"RSVD",			"RSVD",	"RSVD",	"n/a",			"RSVD",	"PSE_CAN0_TX",	"PSE_TGPIO17",
	"GPP_E16",	"PSE_I2S0_TXD",		"RSVD",			"RSVD",	"RSVD",	"RSVD",			"RSVD",	"PSE_CAN0_RX",	"PSE_TGPIO16",
	"GPP_E17",	"PNL1_VDDEN",		"RSVD",			"RSVD",	"RSVD",	"PNL_MISC_DDI2",	"RSVD",	"PSE_PWM11",	"PSE_TGPIO46",
	"GPP_E18",	"DDI0_DDC_SDA",		"n/a",			"n/a",	"n/a",	"RSVD",			"n/a",	"PSE_PWM12",	"PSE_TGPIO23",
	"GPP_E19",	"DDI0_DDC_SCL",		"n/a",			"n/a",	"n/a",	"RSVD",			"n/a",	"PSE_PWM13",	"PSE_TGPIO24",
	"GPP_E20",	"PSE_I2S0_SCLK",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",	"PSE_CAN1_TX",	"PSE_TGPIO14",
	"GPP_E21",	"PSE_I2S0_SFRM",	"n/a",			"n/a",	"n/a",	"n/a",			"n/a",	"PSE_CAN1_RX",	"PSE_TGPIO15",
	"GPP_E22",	"PNL1_BKLTCTL",		"n/a",			"n/a",	"n/a",	"n/a",			"n/a",	"PSE_PWM14",	"PSE_TGPIO18",
	"GPP_E23",	"PNL1_BKLTEN",		"n/a",			"n/a",	"n/a",	"n/a",			"n/a",	"PSE_PWM15",	"PSE_TGPIO19",
};

const struct gpio_group elkhartlake_pch_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_e_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_e_names,
};

const char *const elkhartlake_pch_group_f_names[] = {
	"GPP_F0",	"RSVD",			"SIO_UART0_RTS_N",	"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F1",	"RSVD",			"SIO_UART0_RXD",	"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F2",	"RSVD",			"SIO_UART0_TXD",	"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F3",	"RSVD",			"SIO_UART0_CTS_N",	"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F4",	"RSVD",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F5",	"RSVD",			"RSVD",			"RSVD",	"RSVD",			"n/a",			"n/a",	"n/a",	"PSE_TGPIO14",
	"GPP_F6",	"RSVD",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"PSE_TGPIO47",
	"GPP_F7",	"PSE_I2S1_SCLK",	"n/a",			"n/a",	"n/a",			"AVS_I2S4_SCLK",	"n/a",	"n/a",	"PSE_TGPIO14",
	"GPP_F8",	"AVS_I2S_MCLK2",	"n/a",			"n/a",	"PSE_TRACEDATA_0",	"ISI_TRACEDATA_0",	"n/a",	"n/a",	"PSE_TGPIO48",
	"GPP_F9",	"BOOT_PWR_EN",		"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F10",	"PSE_I2S1_SFRM",	"n/a",			"n/a",	"n/a",			"AVS_I2S4_SFRM",	"n/a",	"n/a",	"PSE_TGPIO15",
	"GPP_F11",	"n/a",			"n/a",			"RSVD",	"PSE_TRACECLK",		"ISI_TRACECLK",		"n/a",	"n/a",	"PSE_TGPIO49",
	"GPP_F12",	"RSVD",			"AVS_I2S4_TXD",		"RSVD",	"PSE_TRACESWO",		"ISI_TRACESWO",		"n/a",	"n/a",	"n/a",
	"GPP_F13",	"RSVD",			"AVS_I2S4_SFRM",	"RSVD",	"PSE_SWDIO",		"ISI_SWDIO",		"n/a",	"n/a",	"n/a",
	"GPP_F14",	"RSVD",			"AVS_I2S4_RXD",		"RSVD",	"PSE_TRACEDATA_1",	"ISI_TRACEDATA_1",	"n/a",	"n/a",	"n/a",
	"GPP_F15",	"RSVD",			"n/a",			"RSVD",	"PSE_TRACEDATA_2",	"ISI_TRACEDATA_2",	"n/a",	"n/a",	"n/a",
	"GPP_F16",	"RSVD",			"AVS_I2S4_SCLK",	"RSVD",	"PSE_SWCLK",		"ISI_SWCLK",		"n/a",	"n/a",	"n/a",
	"GPP_F17",	"n/a",			"n/a",			"RSVD",	"PSE_TRACEDATA_3",	"ISI_TRACEDATA_3",	"n/a",	"n/a",	"PSE_TGPIO50",
	"GPP_F18",	"PSE_I2S1_TXD",		"n/a",			"n/a",	"n/a",			"AVS_I2S4_TXD",		"n/a",	"n/a",	"PSE_TGPIO16",
	"GPP_F19",	"PSE_I2S1_RXD",		"n/a",			"n/a",	"n/a",			"AVS_I2S4_RXD",		"n/a",	"n/a",	"PSE_TGPIO17",
	"GPP_F20",	"RSVD",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F21",	"RSVD",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F22",	"PMC_VNN_CTRL",		"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPP_F23",	"PMC_V1P05_CTRL",	"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_30",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_31",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_32",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_33",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_34",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_35",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_36",	"n/a",			"n/a",			"n/a",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_f_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_f_names,
};

const char *const elkhartlake_pch_group_g_names[] = {
	"GPP_G0",	"SD_SDIO_CMD",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G1",	"SD_SDIO_D0",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G2",	"SD_SDIO_D1",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G3",	"SD_SDIO_D2",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G4",	"SD_SDIO_D3",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G5",	"SD_SDIO_CD_N",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G6",	"SD_SDIO_CLK",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G7",	"AVS_I2S2_SCLK",	"n/a",		"n/a",		"DMIC_CLK_A0",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G8",	"AVS_I2S2_SFRM",	"RSVD",		"n/a",		"DMIC_DATA0",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G9",	"AVS_I2S2_TXD",		"RSVD",		"RSVD",		"DMIC_CLK_A1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G10",	"AVS_I2S2_RXD",		"n/a",		"n/a",		"DMIC_DATA1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G11",	"AVS_I2S3_SCLK",	"n/a",		"n/a",		"DMIC_DATA0",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO07",
	"GPP_G12",	"AVS_I2S3_SFRM",	"SATA_1_GP",	"SATAXPCIE_1",	"DMIC_DATA1",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO31",
	"GPP_G13",	"AVS_I2S3_TXD",		"n/a",		"n/a",		"DMIC_CLK_B0",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO08",
	"GPP_G14",	"AVS_I2S3_RXD",		"n/a",		"n/a",		"DMIC_CLK_B1",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO09",
	"GPP_G15",	"ESPI_IO0",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G16",	"ESPI_IO1",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G17",	"ESPI_IO2",		"Reserved",	"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G18",	"ESPI_IO3",		"Reserved",	"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G19",	"AVS_I2S1_SCLK",	"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G20",	"ESPI_CS0_N",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G21",	"ESPI_CLK",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G22",	"ESPI_RST0_N",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_G23",	"SD_SDIO_WP",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_2",	"n/a",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_g_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_g_names,
};

const char *const elkhartlake_pch_group_h_names[] = {
	"GPP_H0",	"PSE_GBE1_INT",		"n/a",			"n/a",	"PSE_UART5_RXD",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H1",	"PSE_GBE1_RST_N",	"n/a",			"n/a",	"PSE_UART5_TXD",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H2",	"PSE_GBE1_AUXTS",	"n/a",			"n/a",	"PSE_UART5_RTS_N",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H3",	"PSE_GBE1_PPS",		"n/a",			"n/a",	"PSE_UART5_CTS_N",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO21",
	"GPP_H4",	"SIO_I2C2_SDA",		"n/a",			"n/a",	"PSE_PWM08",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO10",
	"GPP_H5",	"SIO_I2C2_SCL",		"n/a",			"n/a",	"PSE_PWM09",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO11",
	"GPP_H6",	"SIO_I2C3_SDA",		"PSE_I2C5_SDA",		"n/a",	"PSE_PWM10",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H7",	"SIO_I2C3_SCL",		"PSE_I2C5_SCL",		"n/a",	"PSE_PWM11",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H8",	"SIO_I2C4_SDA",		"RSVD",			"n/a",	"PSE_PWM12",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H9",	"SIO_I2C4_SCL",		"RSVD",			"n/a",	"PSE_PWM13",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H10",	"PCIE_CLKREQ4_N",	"n/a",			"n/a",	"PSE_PWM14",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H11",	"PCIE_CLKREQ5_N",	"n/a",			"n/a",	"PSE_PWM15",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H12",	"PSE_UART1_RXD",	"n/a",			"n/a",	"M2_SKT2_CFG0",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO51",
	"GPP_H13",	"PSE_UART1_TXD",	"n/a",			"n/a",	"M2_SKT2_CFG1",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO52",
	"GPP_H14",	"n/a",			"n/a",			"n/a",	"M2_SKT2_CFG2",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO53",
	"GPP_H15",	"PSE_UART1_CTS_N",	"n/a",			"n/a",	"M2_SKT2_CFG3",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO54",
	"GPP_H16",	"PCIE_LNK_DOWN",	"DDI2_DDC_SCL",		"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H17",	"SD_SDIO_PWR_EN_N",	"n/a",			"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H18",	"PMC_CPU_C10_GATE_N",	"n/a",			"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H19",	"n/a",			"DDI2_DDC_SDA",		"n/a",	"PMC_TGPIO0",		"n/a",	"n/a",	"n/a",	"PSE_TGPIO20",
	"GPP_H20",	"PSE_PWM07",		"DDI2_HPD",		"n/a",	"RSVD",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO55",
	"GPP_H21",	"PSE_HSUART1_DE",	"PSE_UART1_RTS_N",	"n/a",	"RSVD",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO56",
	"GPP_H22",	"PSE_HSUART1_RE",	"n/a",			"n/a",	"RSVD",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO57",
	"GPP_H23",	"PSE_HSUART1_EN",	"n/a",			"n/a",	"RSVD",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO58",
};

const struct gpio_group elkhartlake_pch_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_h_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_h_names,
};

const char *const elkhartlake_pch_group_r_names[] = {
	"GPP_R0",	"HDA_BCLK",	"AVS_I2S0_SCLK",	"PSE_I2S0_SCLK",	"RSVD",	"n/a",
	"GPP_R1",	"HDA_SYNC",	"AVS_I2S0_SFRM",	"PSE_I2S0_SFRM",	"n/a",	"n/a",
	"GPP_R2",	"HDA_SDO",	"AVS_I2S0_TXD",		"PSE_I2S0_TXD",		"RSVD",	"DMIC_CLK_B0",
	"GPP_R3",	"HDA_SDI0",	"AVS_I2S0_RXD",		"PSE_I2S0_RXD",		"RSVD",	"DMIC_CLK_B1",
	"GPP_R4",	"HDA_RST_N",	"n/a",			"n/a",			"n/a",	"DMIC_CLK_A1",
	"GPP_R5",	"HDA_SDI1",	"AVS_I2S1_RXD",		"n/a",			"n/a",	"DMIC_DATA1",
	"GPP_R6",	"n/a",		"AVS_I2S1_TXD",		"n/a",			"n/a",	"DMIC_CLK_A0",
	"GPP_R7",	"n/a",		"AVS_I2S1_SFRM",	"n/a",			"n/a",	"DMIC_DATA0",
};

const struct gpio_group elkhartlake_pch_group_r = {
	.display	= "------- GPIO Group GPP_R -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_r_names) / 6,
	.func_count	= 6,
	.pad_names	= elkhartlake_pch_group_r_names,
};

const char *const elkhartlake_pch_group_s_names[] = {
	"GPIO_RSVD_13",	"n/a",
	"GPIO_RSVD_14",	"n/a",
	"GPIO_RSVD_15",	"n/a",
	"GPIO_RSVD_16",	"n/a",
	"GPIO_RSVD_17",	"n/a",
	"GPIO_RSVD_18",	"n/a",
	"GPIO_RSVD_19",	"n/a",
	"GPIO_RSVD_20",	"n/a",
	"GPIO_RSVD_21",	"n/a",
	"GPIO_RSVD_22",	"n/a",
	"GPIO_RSVD_23",	"n/a",
	"GPIO_RSVD_24",	"n/a",
	"GPIO_RSVD_25",	"n/a",
	"GPIO_RSVD_26",	"n/a",
	"GPIO_RSVD_27",	"n/a",
	"GPIO_RSVD_28",	"n/a",
	"GPIO_RSVD_29",	"n/a",
	"GPP_S0",	"n/a",
	"GPP_S1",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_s_names) / 2,
	.func_count	= 2,
	.pad_names	= elkhartlake_pch_group_s_names,
};

const char *const elkhartlake_pch_group_t_names[] = {
	"GPP_T0",	"PSE_QEPA2",		"n/a",			"SIO_I2C6_SDA",		"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO08",
	"GPP_T1",	"PSE_QEPB2",		"n/a",			"SIO_I2C6_SCL",		"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO09",
	"GPP_T2",	"PSE_QEPI2",		"n/a",			"SIO_I2C7_SDA",		"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO07",
	"GPP_T3",	"n/a",			"n/a",			"SIO_I2C7_SCL",		"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO06",
	"GPP_T4",	"PSE_GBE0_INT",		"n/a",			"n/a",			"RSVD",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T5",	"PSE_GBE0_RST_N",	"n/a",			"n/a",			"RSVD",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T6",	"PSE_GBE0_AUXTS",	"USB2_OC1_N",		"n/a",			"RSVD",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T7",	"PSE_GBE0_PPS",		"n/a",			"n/a",			"RSVD",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO59",
	"GPP_T8",	"USB2_OC2_N",		"RSVD",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO22",
	"GPP_T9",	"PSE_HSUART2_EN",	"RSVD",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T10",	"PSE_HSUART2_RE",	"RSVD",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T11",	"USB2_OC3_N",		"RSVD",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO06",
	"GPP_T12",	"PSE_UART2_RXD",	"SIO_UART0_RXD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T13",	"PSE_UART2_TXD",	"SIO_UART0_TXD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T14",	"PSE_UART2_RTS_N",	"SIO_UART0_RTS_N",	"PSE_HSUART2_DE",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_T15",	"PSE_UART2_CTS_N",	"SIO_UART0_CTS_N",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_t = {
	.display	= "------- GPIO Group GPP_T -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_t_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_t_names,
};

const char *const elkhartlake_pch_group_u_names[] = {
	"GPP_U0",	"GBE_INT",		"PSE_I2C6_SCL",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U1",	"GBE_RST_N",		"PSE_I2C6_SDA",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U2",	"GBE_PPS",		"PSE_I2C7_SCL",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U3",	"GBE_AUXTS",		"PSE_I2C7_SDA",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U4",	"ISI_SPIM_CS",		"n/a",		"RSVD",	"PSE_SPI1_CS0_N",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U5",	"ISI_SPIM_SCLK",	"n/a",		"RSVD",	"PSE_SPI1_CLK",		"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U6",	"ISI_SPIM_MISO",	"n/a",		"RSVD",	"PSE_SPI1_MISO",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U7",	"PSE_QEPA3",		"n/a",		"RSVD",	"PSE_SPI1_MOSI",	"n/a",	"n/a",	"n/a",	"PSE_TGPIO10",
	"GPP_U8",	"ISI_SPIS_CS",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO10",
	"GPP_U9",	"ISI_SPIS_SCLK",	"ISI_I2CS_SCL",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO11",
	"GPP_U10",	"ISI_SPIS_MISO",	"ISI_I2CS_SDA",	"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO12",
	"GPP_U11",	"PSE_QEPB3",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO11",
	"GPP_U12",	"ISI_CHX_OKNOK_0",	"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U13",	"ISI_CHX_OKNOK_1",	"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U14",	"ISI_CHX_RLY_SWTCH",	"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U15",	"ISI_CHX_PMIC_EN",	"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO13",
	"GPP_U16",	"ISI_OKNOK_0",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U17",	"ISI_OKNOK_1",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U18",	"ISI_ALERT_N",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_U19",	"PSE_QEPI3",		"n/a",		"RSVD",	"n/a",			"n/a",	"n/a",	"n/a",	"PSE_TGPIO12",
	"GPIO_RSVD_4",	"n/a",			"n/a",		"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_5",	"n/a",			"n/a",		"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_6",	"n/a",			"n/a",		"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
	"GPIO_RSVD_7",	"n/a",			"n/a",		"n/a",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group elkhartlake_pch_group_u = {
	.display	= "------- GPIO Group GPP_U -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_u_names) / 9,
	.func_count	= 9,
	.pad_names	= elkhartlake_pch_group_u_names,
};

const char *const elkhartlake_pch_group_v_names[] = {
	"GPP_V0",	"EMMC_CMD",
	"GPP_V1",	"EMMC_DATA0",
	"GPP_V2",	"EMMC_DATA1",
	"GPP_V3",	"EMMC_DATA2",
	"GPP_V4",	"EMMC_DATA3",
	"GPP_V5",	"EMMC_DATA4",
	"GPP_V6",	"EMMC_DATA5",
	"GPP_V7",	"EMMC_DATA6",
	"GPP_V8",	"EMMC_DATA7",
	"GPP_V9",	"EMMC_RCLK",
	"GPP_V10",	"EMMC_CLK",
	"GPP_V11",	"EMMC_RST_N",
	"GPP_V12",	"PSE_TGPIO00",
	"GPP_V13",	"PSE_TGPIO01",
	"GPP_V14",	"PSE_TGPIO02",
	"GPP_V15",	"PSE_TGPIO03",
};

const struct gpio_group elkhartlake_pch_group_v = {
	.display	= "------- GPIO Group GPP_V -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_v_names) / 2,
	.func_count	= 2,
	.pad_names	= elkhartlake_pch_group_v_names,
};

const char *const elkhartlake_pch_group_vgpio_names[] = {
	"VGPIO_0",	"VGPIO_0",
	"VGPIO_4",	"VGPIO_4",
	"VGPIO_5",	"VGPIO_5",
	"VGPIO_6",	"VGPIO_6",
	"VGPIO_7",	"VGPIO_7",
	"VGPIO_8",	"VGPIO_8",
	"VGPIO_9",	"VGPIO_9",
	"VGPIO_10",	"VGPIO_10",
	"VGPIO_11",	"VGPIO_11",
	"VGPIO_12",	"VGPIO_12",
	"VGPIO_13",	"VGPIO_13",
	"VGPIO_18",	"VGPIO_18",
	"VGPIO_19",	"VGPIO_19",
	"VGPIO_20",	"VGPIO_20",
	"VGPIO_21",	"VGPIO_21",
	"VGPIO_22",	"VGPIO_22",
	"VGPIO_23",	"VGPIO_23",
	"VGPIO_24",	"VGPIO_24",
	"VGPIO_25",	"VGPIO_25",
	"VGPIO_30",	"VGPIO_30",
	"VGPIO_31",	"VGPIO_31",
	"VGPIO_32",	"VGPIO_32",
	"VGPIO_33",	"VGPIO_33",
	"VGPIO_34",	"VGPIO_34",
	"VGPIO_35",	"VGPIO_35",
	"VGPIO_36",	"VGPIO_36",
	"VGPIO_37",	"VGPIO_37",
	"VGPIO_39",	"VGPIO_39",
};

const struct gpio_group elkhartlake_pch_group_vgpio = {
	.display	= "------- GPIO Group GPP_VGPIO -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= elkhartlake_pch_group_vgpio_names,
};


const char *const elkhartlake_pch_group_vgpio_usb_names[] = {
	"VGPIO_USB_0",	"VGPIO_USB_0",
	"VGPIO_USB_1",	"VGPIO_USB_1",
	"VGPIO_USB_2",	"VGPIO_USB_2",
	"VGPIO_USB_3",	"VGPIO_USB_3",
};

const struct gpio_group elkhartlake_pch_group_vgpio_usb = {
	.display	= "------- GPIO Group GPP_VGPIO_USB -------",
	.pad_count	= ARRAY_SIZE(elkhartlake_pch_group_vgpio_usb_names) / 2,
	.func_count	= 2,
	.pad_names	= elkhartlake_pch_group_vgpio_usb_names,
};

const struct gpio_group *const elkhartlake_community_0_groups[] = {
	&elkhartlake_pch_group_b,
	&elkhartlake_pch_group_t,
	&elkhartlake_pch_group_g,
};

const struct gpio_community elkhartlake_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(elkhartlake_community_0_groups),
	.groups		= elkhartlake_community_0_groups,
};

const struct gpio_group *const elkhartlake_community_1_groups[] = {
	&elkhartlake_pch_group_v,
	&elkhartlake_pch_group_h,
	&elkhartlake_pch_group_d,
	&elkhartlake_pch_group_u,
	&elkhartlake_pch_group_vgpio,
};

const struct gpio_community elkhartlake_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(elkhartlake_community_1_groups),
	.groups		= elkhartlake_community_1_groups,
};

const struct gpio_group *const elkhartlake_community_2_groups[] = {
	&elkhartlake_pch_group_gpd,
};

const struct gpio_community elkhartlake_community_2 = {
	.name		= "------- GPIO Community 2 -------",
	.pcr_port_id	= 0x6c,
	.group_count	= ARRAY_SIZE(elkhartlake_community_2_groups),
	.groups		= elkhartlake_community_2_groups,
};

const struct gpio_group *const elkhartlake_community_3_groups[] = {
	&elkhartlake_pch_group_s,
	&elkhartlake_pch_group_a,
	&elkhartlake_pch_group_vgpio_usb,
};

const struct gpio_community elkhartlake_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(elkhartlake_community_3_groups),
	.groups		= elkhartlake_community_3_groups,
};

const struct gpio_group *const elkhartlake_community_4_groups[] = {
	&elkhartlake_pch_group_c,
	&elkhartlake_pch_group_f,
	&elkhartlake_pch_group_e,
};

const struct gpio_community elkhartlake_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(elkhartlake_community_4_groups),
	.groups		= elkhartlake_community_4_groups,
};

const struct gpio_group *const elkhartlake_community_5_groups[] = {
	&elkhartlake_pch_group_r,
};

const struct gpio_community elkhartlake_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(elkhartlake_community_5_groups),
	.groups		= elkhartlake_community_5_groups,
};

const struct gpio_community *const elkhartlake_pch_communities[] = {
	&elkhartlake_community_0,
	&elkhartlake_community_1,
	&elkhartlake_community_2,
	&elkhartlake_community_3,
	&elkhartlake_community_4,
	&elkhartlake_community_5,
};

#endif
