/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPIO_NAMES_METEORLAKE
#define GPIO_NAMES_METEORLAKE

#include "gpio_groups.h"

/* ----------------------------- Meteor Lake ----------------------------- */

const char *const meteorlake_pch_group_a_names[] = {
	"GPP_A00",		"ESPI_IO0",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A00",
	"GPP_A01",		"ESPI_IO1",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A01",
	"GPP_A02",		"ESPI_IO2",		"PRIMPWRDNACK",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A02",
	"GPP_A03",		"ESPI_IO3",		"PRIMACK#",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A03",
	"GPP_A04",		"ESPI_CS0#",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A04",
	"GPP_A05",		"ESPI_CLK",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A05",
	"GPP_A06",		"ESPI_RESET#",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A06",
	"GPP_A07",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A08",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A09",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A10",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A11",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A11",
	"GPP_A12",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A12",
	"GPP_A13",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A13",
	"GPP_A14",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A14",
	"GPP_A15",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A15",
	"GPP_A16",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A16",
	"GPP_A17",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A17",
	"GPP_A18",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A18",
	"GPP_A19",		"RSVD",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A19",
	"GPP_A20",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_A20",
	"GPP_A21",		"PMCALERT#",		"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A22",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A23",		"n/a",			"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"ESPI_CLK_LOOPBK",	"ESPI_CLK_LOOPBK",	"n/a",		"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group meteorlake_pch_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_a_names) / 8,
	.func_count	= 8,
	.pad_names	= meteorlake_pch_group_a_names,
};

const char *const meteorlake_pch_group_b_names[] = {
	"GPP_B00",		"RSVD",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B00",	"n/a",
	"GPP_B01",		"RSVD",			"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B01",	"n/a",
	"GPP_B02",		"n/a",			"n/a",		"ISH_I2C0_SDA",	"ISH_I3C0_SDA",	"n/a",	"n/a",	"USB-C_GPP_B02",	"I2C2A_SDA",
	"GPP_B03",		"n/a",			"n/a",		"ISH_I2C0_SCL",	"ISH_I3C0_SCL",	"n/a",	"n/a",	"USB-C_GPP_B03",	"I2C2A_SCL",
	"GPP_B04",		"n/a",			"BK0",		"n/a",		"ISH_GP4",	"n/a",	"SBK0",	"USB-C_GPP_B04",	"n/a",
	"GPP_B05",		"n/a",			"BK1",		"n/a",		"ISH_GP0",	"n/a",	"SBK1",	"USB-C_GPP_B05",	"n/a",
	"GPP_B06",		"n/a",			"BK2",		"n/a",		"ISH_GP1",	"n/a",	"SBK2",	"USB-C_GPP_B06",	"n/a",
	"GPP_B07",		"n/a",			"BK3",		"n/a",		"ISH_GP2",	"n/a",	"SBK3",	"USB-C_GPP_B07",	"n/a",
	"GPP_B08",		"n/a",			"BK4",		"n/a",		"ISH_GP3",	"n/a",	"SBK4",	"USB-C_GPP_B08",	"n/a",
	"GPP_B09",		"n/a",			"DDSP_HPD0",	"DISP_MISC1",	"n/a",		"n/a",	"n/a",	"USB-C_GPP_B09",	"n/a",
	"GPP_B10",		"n/a",			"DDSP_HPD1",	"DISP_MISC2",	"n/a",		"n/a",	"n/a",	"USB-C_GPP_B10",	"n/a",
	"GPP_B11",		"USB_OC1#",		"DDSP_HPD2",	"DISP_MISC3",	"n/a",		"n/a",	"n/a",	"USB-C_GPP_B11",	"n/a",
	"GPP_B12",		"SLP_S0#",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B12",	"n/a",
	"GPP_B13",		"PLTRST#",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B13",	"n/a",
	"GPP_B14",		"USB_OC2#",		"DDSP_HPD3",	"DISP_MISC4",	"n/a",		"n/a",	"n/a",	"USB-C_GPP_B14",	"n/a",
	"GPP_B15",		"USB_OC3#",		"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B15",	"n/a",
	"GPP_B16",		"n/a",			"DDSP_HPDB",	"DISP_MISCB",	"n/a",		"n/a",	"n/a",	"USB-C_GPP_B16",	"n/a",
	"GPP_B17",		"n/a",			"VDDEN2",	"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B17",	"n/a",
	"GPP_B18",		"ISH_I2C2_SDA",		"I2C4A_SDA",	"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B18",	"n/a",
	"GPP_B19",		"ISH_I2C2_SCL",		"I2C4A_SCL",	"n/a",		"n/a",		"n/a",	"n/a",	"USB-C_GPP_B19",	"n/a",
	"GPP_B20",		"n/a",			"I2C5A_SDA",	"n/a",		"ISH_GP8",	"n/a",	"n/a",	"USB-C_GPP_B20",	"n/a",
	"GPP_B21",		"n/a",			"I2C5A_SCL",	"n/a",		"ISH_GP9",	"n/a",	"n/a",	"USB-C_GPP_B21",	"n/a",
	"GPP_B22",		"TIME_SYNC0",		"n/a",		"n/a",		"ISH_GP5",	"n/a",	"n/a",	"USB-C_GPP_B22",	"n/a",
	"GPP_B23",		"TIME_SYNC1",		"n/a",		"n/a",		"ISH_GP6",	"n/a",	"n/a",	"USB-C_GPP_B23",	"n/a",
	"GPP_ACI3C0_CLK_LPBK",	"GPP_ACI3C0_CLK_LPBK",	"n/a",		"n/a",		"n/a",		"n/a",	"n/a",	"n/a",			"n/a",
};

const struct gpio_group meteorlake_pch_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_b_names) / 9,
	.func_count	= 9,
	.pad_names	= meteorlake_pch_group_b_names,
};

const char *const meteorlake_pch_group_c_names[] = {
	"GPP_C00",	"SMBCLK",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C00",
	"GPP_C01",	"SMBDATA",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C01",
	"GPP_C02",	"SMBALERT#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C02",
	"GPP_C03",	"SML0CLK",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C03",
	"GPP_C04",	"SML0DATA",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C04",
	"GPP_C05",	"SML0ALERT#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C05",
	"GPP_C06",	"SML1CLK",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C06",
	"GPP_C07",	"SML1DATA",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C07",
	"GPP_C08",	"SML1ALERT#",	"SOCHOT#",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C08",
	"GPP_C09",	"SRCCLKREQ0#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C09",
	"GPP_C10",	"SRCCLKREQ1#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C10",
	"GPP_C11",	"SRCCLKREQ2#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C11",
	"GPP_C12",	"SRCCLKREQ3#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C12",
	"GPP_C13",	"SRCCLKREQ4#",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C13",
	"GPP_C14",	"n/a",		"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C15",	"n/a",		"n/a",			"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C15",
	"GPP_C16",	"TBT_LSX0_TXD",	"DDP0_CTRLCLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C16",
	"GPP_C17",	"TBT_LSX0_RXD",	"DDP0_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C17",
	"GPP_C18",	"TBT_LSX1_TXD",	"DDP1_CTRLCLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C18",
	"GPP_C19",	"TBT_LSX1_RXD",	"DDP1_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C19",
	"GPP_C20",	"TBT_LSX2_TXD",	"DDP2_CTRLCLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C20",
	"GPP_C21",	"TBT_LSX2_RXD",	"DDP2_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C21",
	"GPP_C22",	"TBT_LSX3_TXD",	"DDP3_CTRLCLK",		"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C22",
	"GPP_C23",	"TBT_LSX3_RXD",	"DDP3_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_C23",
};

const struct gpio_group meteorlake_pch_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_c_names) / 8,
	.func_count	= 8,
	.pad_names	= meteorlake_pch_group_c_names,
};

const char *const meteorlake_pch_group_d_names[] = {
	"GPP_D00",		"IMGCLKOUT1",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D00",
	"GPP_D01",		"I2C3A_SDA",		"BKLTEN2",		"ISH_I2C2A_SDA",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_D01",
	"GPP_D02",		"I2C3A_SCL",		"BKLTCTL2",		"ISH_I2C2A_SCL",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_D02",
	"GPP_D03",		"PROC_GP1",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D03",
	"GPP_D04",		"IMGCLKOUT0",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D04",
	"GPP_D05",		"n/a",			"ISH_UART0_RXD",	"ISH_SPI_CS#",		"n/a",	"n/a",	"SML0BDATA",	"USB-C_GPP_D05",
	"GPP_D06",		"n/a",			"ISH_UART0_TXD",	"ISH_SPI_CLK",		"n/a",	"n/a",	"SML0BCLK",	"USB-C_GPP_D06",
	"GPP_D07",		"IMGCLKOUT4",		"ISH_UART0_RTS#",	"ISH_SPI_MISO",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D07",
	"GPP_D08",		"n/a",			"ISH_UART0_CTS#",	"ISH_SPI_MOSI",		"n/a",	"n/a",	"SML0BALERT#",	"USB-C_GPP_D08",
	"GPP_D09",		"n/a",			"I2S_MCLK1_OUT",	"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D09",
	"GPP_D10",		"HDA_BCLK",		"I2S0_SCLK",		"DMIC_CLK_A1",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D10",
	"GPP_D11",		"HDA_SYNC",		"I2S0_SFRM",		"DMIC_CLK_B1",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D11",
	"GPP_D12",		"HDA_SDO",		"I2S0_TXD",		"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D12",
	"GPP_D13",		"HDA_SDI0",		"I2S0_RXD",		"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D13",
	"GPP_D14",		"n/a",			"I2S2_SCLK",		"DMIC_CLK_A0",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D14",
	"GPP_D15",		"n/a",			"I2S2_SFRM",		"DMIC_DATA0",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D15",
	"GPP_D16",		"HDA_SDI1",		"I2S2_TXD",		"DMIC_CLK_B0",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D16",
	"GPP_D17",		"HDA_RST#",		"I2S2_RXD",		"DMIC_DATA1",		"n/a",	"n/a",	"n/a",		"USB-C_GPP_D17",
	"GPP_D18",		"SRCCLKREQ6#",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D18",
	"GPP_D19",		"SRCCLKREQ7#",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D19",
	"GPP_D20",		"SRCCLKREQ8#",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D20",
	"GPP_D21",		"RSVD",			"SRCCLKREQ5#",		"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D21",
	"GPP_D22",		"n/a",			"BSSB_LS0_RX",		"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D22",
	"GPP_D23",		"n/a",			"BSSB_LS0_TX",		"n/a",			"n/a",	"n/a",	"n/a",		"USB-C_GPP_D23",
	"GPP_BOOTHALT_B",	"GPP_BOOTHALT_B",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",		"n/a",
};

const struct gpio_group meteorlake_pch_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_d_names) / 8,
	.func_count	= 8,
	.pad_names	= meteorlake_pch_group_d_names,
};

const char *const meteorlake_pch_group_e_names[] = {
	"GPP_E00",			"SATAXPCIE0",			"SATAGP0",		"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E00",	"n/a",
	"GPP_E01",			"USB-C_SMLACLK",		"THC0_SPI1_IO2",	"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E01",	"n/a",
	"GPP_E02",			"USB-C_SMLADATA",		"THC0_SPI1_IO3",	"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E02",	"n/a",
	"GPP_E03",			"PROC_GP0",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E03",	"n/a",
	"GPP_E04",			"SATA_DEVSLP0",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E04",	"n/a",
	"GPP_E05",			"SATA_DEVSLP1",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E05",	"ISH_GP7",
	"GPP_E06",			"n/a",				"THC0_SPI1_RST#",	"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E06",	"n/a",
	"GPP_E07",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E07",	"n/a",
	"GPP_E08",			"DDPA_CTRLDATA",		"SATALED#",		"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E08",	"n/a",
	"GPP_E09",			"USB_OC0#",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E09",	"n/a",
	"GPP_E10",			"n/a",				"THC0_SPI1_CS#",	"n/a",	"n/a",	"n/a",	"GSPI0_CS0#",	"USB-C_GPP_E10",	"n/a",
	"GPP_E11",			"n/a",				"THC0_SPI1_CLK",	"n/a",	"n/a",	"n/a",	"GSPI0_CLK",	"USB-C_GPP_E11",	"n/a",
	"GPP_E12",			"n/a",				"THC0_SPI1_IO1",	"n/a",	"n/a",	"n/a",	"GSPI0_MISO",	"USB-C_GPP_E12",	"I2C4_SDA",
	"GPP_E13",			"n/a",				"THC0_SPI1_IO0",	"n/a",	"n/a",	"n/a",	"GSPI0_MOSI",	"USB-C_GPP_E13",	"I2C4_SCL",
	"GPP_E14",			"DDSP_HPDA",			"DISP_MISCA",		"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E14",	"n/a",
	"GPP_E15",			"PROC_GP2",			"RSVD",			"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E15",	"ISH_GP5A",
	"GPP_E16",			"PROC_GP3",			"VRALERT#",		"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E16",	"ISH_GP10",
	"GPP_E17",			"n/a",				"THC0_SPI1_INT#",	"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E17",	"n/a",
	"GPP_E18",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
	"GPP_E19",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
	"GPP_E20",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
	"GPP_E21",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
	"GPP_E22",			"DDPA_CTRLCLK",			"DNX_FORCE_RELOAD",	"n/a",	"n/a",	"n/a",	"n/a",		"USB-C_GPP_E22",	"n/a",
	"GPP_E23",			"n/a",				"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
	"GPP_THC0_GSPI_CLK_LPBK",	"GPP_THC0_GSPI_CLK_LPBK",	"n/a",			"n/a",	"n/a",	"n/a",	"n/a",		"n/a",			"n/a",
};

const struct gpio_group meteorlake_pch_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_e_names) / 9,
	.func_count	= 9,
	.pad_names	= meteorlake_pch_group_e_names,
};

const char *const meteorlake_pch_group_f_names[] = {
	"GPP_F00",			"CNV_BRI_DT",			"UART2_RTS#",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F00",	"n/a",
	"GPP_F01",			"CNV_BRI_RSP",			"UART2_RXD",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F01",	"n/a",
	"GPP_F02",			"CNV_RGI_DT",			"UART2_TXD",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F02",	"n/a",
	"GPP_F03",			"CNV_RGI_RSP",			"UART2_CTS#",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F03",	"n/a",
	"GPP_F04",			"CNV_RF_RESET#",		"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F04",	"n/a",
	"GPP_F05",			"n/a",				"n/a",			"MODEM_CLKREQ",		"n/a",			"n/a",		"n/a",		"USB-C_GPP_F05",	"n/a",
	"GPP_F06",			"CNV_PA_BLANKING",		 "n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F06",	"n/a",
	"GPP_F07",			"RSVD",				"IMGCLKOUT2",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F07",	"n/a",
	"GPP_F08",			"RSVD",				"IMGCLKOUT3",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F08",	"n/a",
	"GPP_F09",			"RSVD",				"SX_EXIT_HOLDOFF#",	"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F09",	"ISH_GP11",
	"GPP_F10",			"SATAXPCIE1",			"SATAGP1",		"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F10",	"ISH_GP6A",
	"GPP_F11",			"n/a",				"n/a",			"THC1_SPI2_CLK",	"ISH_SPIA_CLK",		"n/a",		"GSPI1_CLK",	"USB-C_GPP_F11",	"n/a",
	"GPP_F12",			"GSXDOUT",			"n/a",			"THC1_SPI2_IO0",	"ISH_SPIA_MISO",	"n/a",		"GSPI1_MOSI",	"USB-C_GPP_F12",	"I2C5_SCL",
	"GPP_F13",			"GSXSLOAD",			 "n/a",			"THC1_SPI2_IO1",	"ISH_SPIA_MOSI",	"n/a",		"GSPI1_MISO",	"USB-C_GPP_F13",	"I2C5_SDA",
	"GPP_F14",			"GSXDIN",			 "USB-C_SMLCLK",	"THC1_SPI2_IO2",	"n/a",			"n/a",		"n/a",		"USB-C_GPP_F14",	"GSPI0A_MOSI",
	"GPP_F15",			"GSXSRESET#",			"USB-C_SMLDATA",	"THC1_SPI2_IO3",	"n/a",			"n/a",		"n/a",		"USB-C_GPP_F15",	"GSPI0A_MISO",
	"GPP_F16",			"GSXCLK",			 "n/a",			"THC1_SPI2_RST#",	"n/a",			"n/a",		"n/a",		"USB-C_GPP_F16",	"GSPI0A_CLK",
	"GPP_F17",			"n/a",				"n/a",			"THC1_SPI2_CS#",	"ISH_SPIA_CS#",		"n/a",		"GSPI1_CS0#",	"USB-C_GPP_F17",	"n/a",
	"GPP_F18",			"n/a",				"n/a",			"THC1_SPI2_INT#",	"n/a",			"n/a",		"n/a",		"USB-C_GPP_F18",	"GSPI0A_CS0#",
	"GPP_F19",			"n/a",				"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F19",	"n/a",
	"GPP_F20",			"n/a",				"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F20",	"n/a",
	"GPP_F21",			"n/a",				"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F21",	"n/a",
	"GPP_F22",			"n/a",				"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F22",	"ISH_GP8A",
	"GPP_F23",			"n/a",				"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"USB-C_GPP_F23",	"ISH_GP9A",
	"GPP_THC1_GSPI1_CLK_LPBK",	"GPP_THC1_GSPI1_CLK_LPBK",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"n/a",			"n/a",
	"GPP_GSPI0A_CLK_LOOPBK",	"GPP_GSPI0A_CLK_LOOPBK",	"n/a",			"n/a",			"n/a",			"n/a",		"n/a",		"n/a",			"n/a",
};

const struct gpio_group meteorlake_pch_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_f_names) / 9,
	.func_count	= 9,
	.pad_names	= meteorlake_pch_group_f_names,
};

const char *const meteorlake_pch_group_h_names[] = {
	"GPP_H00",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H00",	"n/a",
	"GPP_H01",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H01",	"n/a",
	"GPP_H02",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H02",	"n/a",
	"GPP_H03",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_H04",		"I2C2_SDA",		"CNV_MFUART2_RXD",	"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H04",	"n/a",
	"GPP_H05",		"I2C2_SCL",		"CNV_MFUART2_TXD",	"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H05",	"n/a",
	"GPP_H06",		"I2C3_SDA",		"UART1_RXD",		"ISH_UART1A_RXD",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_H06",	"n/a",
	"GPP_H07",		"I2C3_SCL",		"UART1_TXD",		"ISH_UART1A_TXD",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_H07",	"n/a",
	"GPP_H08",		"UART0_RXD",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H08",	"n/a",
	"GPP_H09",		"UART0_TXD",		"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H09",	"n/a",
	"GPP_H10",		"UART0_RTS#",		"I3C1A_SDA",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H10",	"ISH_GP10A",
	"GPP_H11",		"UART0_CTS#",		"I3C1A_SCL",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H11",	"ISH_GP11A",
	"GPP_H12",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_H13",		"PROC_C10_GATE#",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H13",	"n/a",
	"GPP_H14",		"ISH_UART1_RXD",	"UART1A_RXD",		"ISH_I2C1_SDA",		"n/a",	"n/a",	"n/a",	"USB-C_GPP_H14",	"n/a",
	"GPP_H15",		"ISH_UART1_TXD",	"UART1A_TXD",		"ISH_I2C1_SCL",		"n/a",	"n/a",	"n/a",	"USB-C_GPP_H15",	"n/a",
	"GPP_H16",		"DDPB_CTRLCLK",		"n/a",			"PCIE_LINK_DOWN",	"n/a",	"n/a",	"n/a",	"USB-C_GPP_H16",	"n/a",
	"GPP_H17",		"DDPB_CTRLDATA",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H17",	"n/a",
	"GPP_H18",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_H19",		"I2C0_SDA",		"I3C0_SDA",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H19",	"n/a",
	"GPP_H20",		"I2C0_SCL",		"I3C0_SCL",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H20",	"n/a",
	"GPP_H21",		"I2C1_SDA",		"I3C1_SDA",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H21",	"n/a",
	"GPP_H22",		"I2C1_SCL",		"I3C1_SCL",		"n/a",			"n/a",	"n/a",	"n/a",	"USB-C_GPP_H22",	"n/a",
	"GPP_H23",		"n/a",			"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_LPI3C1_CLK_LPBK",	"GPP_LPI3C1_CLK_LPBK",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
	"GPP_LPI3C0_CLK_LPBK",	"GPP_LPI3C0_CLK_LPBK",	"n/a",			"n/a",			"n/a",	"n/a",	"n/a",	"n/a",			"n/a",
};

const struct gpio_group meteorlake_pch_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_h_names) / 9,
	.func_count	= 9,
	.pad_names	= meteorlake_pch_group_h_names,
};

const char *const meteorlake_pch_group_s_names[] = {
	"GPP_S00",	"SNDW0_CLK",	"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"I2S1_SCLK",
	"GPP_S01",	"SNDW0_DATA0",	"n/a",		"n/a",		"n/a",	"n/a",	"n/a",	"I2S1_SFRM",
	"GPP_S02",	"SNDW1_CLK",	"SNDW0_DATA1",	"DMIC_CLK_A0",	"n/a",	"n/a",	"n/a",	"I2S1_TXD",
	"GPP_S03",	"SNDW1_DATA",	"SNDW0_DATA2",	"DMIC_DATA0",	"n/a",	"n/a",	"n/a",	"I2S1_RXD",
	"GPP_S04",	"SNDW2_CLK",	"SNDW0_DATA3",	"DMIC_CLK_B0",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_S05",	"SNDW2_DATA",	"n/a",		"DMIC_CLK_B1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_S06",	"SNDW3_CLK",	"n/a",		"DMIC_CLK_A1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_S07",	"SNDW3_DATA",	"n/a",		"DMIC_DATA1",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group meteorlake_pch_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_s_names) / 8,
	.func_count	= 8,
	.pad_names	= meteorlake_pch_group_s_names,
};

const char *const meteorlake_pch_group_spi0_names[] = {
	"SPI0_IO_2",		"SPI0_IO_2",
	"SPI0_IO_3",		"SPI0_IO_3",
	"SPI0_MOSI_IO_0",	"SPI0_MOSI_IO_0",
	"SPI0_MOSI_IO_1",	"SPI0_MOSI_IO_1",
	"SPI0_TPM_CS_B",	"SPI0_TPM_CS_B",
	"SPI0_FLASH_0_CS_B",	"SPI0_FLASH_0_CS_B",
	"SPI0_FLASH_1_CS_B",	"SPI0_FLASH_1_CS_B",
	"SPI0_CLK",		"SPI0_CLK",
	"BKLTEN",		"BKLTEN",
	"BKLTCTL",		"BKLTCTL",
	"VDDEN",		"VDDEN",
	"SYS_PWROK",		"SYS_PWROK",
	"SYS_RESET_B",		"SYS_RESET_B",
	"MLK_RST_B",		"MLK_RST_B",
	"SPI0_CLK_LOOPBK",	"SPI0_CLK_LOOPBK",
};

const struct gpio_group meteorlake_pch_group_spi0 = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_spi0_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_spi0_names,
};

const char *const meteorlake_pch_group_cpu_names[] = {
	"PECI",		"PECI",
	"UFS_RST_B",	"UFS_RST_B",
	"VIDSOUT",	"VIDSOUT",
	"VIDSCK",	"VIDSCK",
	"VIDALERT_B",	"VIDALERT_B",
};

const struct gpio_group meteorlake_pch_group_cpu = {
	.display	= "------- GPIO Group CPU -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_cpu_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_cpu_names,
};

const char *const meteorlake_pch_group_v_names[] = {
	"GPP_V00",	"BATLOW#",
	"GPP_V01",	"ACPRESENT",
	"GPP_V02",	"SOC_WAKE#",
	"GPP_V03",	"PWRBTN#",
	"GPP_V04",	"SLP_S3#",
	"GPP_V05",	"SLP_S4#",
	"GPP_V06",	"SLP_A#",
	"GPP_V07",	"n/a",
	"GPP_V08",	"SUSCLK",
	"GPP_V09",	"SLP_WLAN#",
	"GPP_V10",	"SLP_S5#",
	"GPP_V11",	"LANPHYPC",
	"GPP_V12",	"SLP_LAN#",
	"GPP_V13",	"n/a",
	"GPP_V14",	"WAKE#",
	"GPP_V15",	"n/a",
	"GPP_V16",	"n/a",
	"GPP_V17",	"n/a",
	"GPP_V18",	"n/a",
	"GPP_V19",	"n/a",
	"GPP_V20",	"n/a",
	"GPP_V21",	"n/a",
	"GPP_V22",	"n/a",
	"GPP_V23",	"n/a",
};

const struct gpio_group meteorlake_pch_group_v = {
	.display	= "------- GPIO Group V -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_v_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_v_names,
};

const char *const meteorlake_pch_group_vgpio3_names[] = {
	"GPP_VGPIO3_USB0",	"GPP_VGPIO3_USB0",
	"GPP_VGPIO3_USB1",	"GPP_VGPIO3_USB1",
	"GPP_VGPIO3_USB2",	"GPP_VGPIO3_USB2",
	"GPP_VGPIO3_USB3",	"GPP_VGPIO3_USB3",
	"GPP_VGPIO3_USB4",	"GPP_VGPIO3_USB4",
	"GPP_VGPIO3_USB5",	"GPP_VGPIO3_USB5",
	"GPP_VGPIO3_USB6",	"GPP_VGPIO3_USB6",
	"GPP_VGPIO3_USB7",	"GPP_VGPIO3_USB7",
	"GPP_VGPIO3_TS0",	"GPP_VGPIO3_TS0",
	"GPP_VGPIO3_TS1",	"GPP_VGPIO3_TS1",
	"GPP_VGPIO3_THC0",	"GPP_VGPIO3_THC0",
	"GPP_VGPIO3_THC1",	"GPP_VGPIO3_THC1",
	"GPP_VGPIO3_THC2",	"GPP_VGPIO3_THC2",
	"GPP_VGPIO3_THC3",	"GPP_VGPIO3_THC3",
};

const struct gpio_group meteorlake_pch_group_vgpio3 = {
	.display	= "------- GPIO Group VGPIO3 -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_vgpio3_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_vgpio3_names,
};

const char *const meteorlake_pch_group_jtag_names[] = {
	"JTAG_MBPB0",	"JTAG_MBPB0",
	"JTAG_MBPB1",	"JTAG_MBPB1",
	"JTAG_MBPB2",	"JTAG_MBPB2",
	"JTAG_MBPB3",	"JTAG_MBPB3",
	"JTAG_TD0",	"JTAG_TD0",
	"PRDY_B",	"PRDY_B",
	"PREQ_B",	"PREQ_B",
	"JTAG_TDI",	"JTAG_TDI",
	"JTAG_TMS",	"JTAG_TMS",
	"JTAG_TCK",	"JTAG_TCK",
	"DBG_PMODE",	"DBG_PMODE",
	"JTAG_TRST_B",	"JTAG_TRST_B",
};

const struct gpio_group meteorlake_pch_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_jtag_names,
};

const char *const meteorlake_pch_group_vgpio_names[] = {
	"GPP_VGPIO00",	"GPP_VGPIO00",
	"GPP_VGPIO04",	"GPP_VGPIO04",
	"GPP_VGPIO05",	"GPP_VGPIO05",
	"GPP_VGPIO06",	"GPP_VGPIO06",
	"GPP_VGPIO07",	"GPP_VGPIO07",
	"GPP_VGPIO08",	"GPP_VGPIO08",
	"GPP_VGPIO09",	"GPP_VGPIO09",
	"GPP_VGPIO10",	"GPP_VGPIO10",
	"GPP_VGPIO11",	"GPP_VGPIO11",
	"GPP_VGPIO12",	"GPP_VGPIO12",
	"GPP_VGPIO13",	"GPP_VGPIO13",
	"GPP_VGPIO18",	"GPP_VGPIO18",
	"GPP_VGPIO19",	"GPP_VGPIO19",
	"GPP_VGPIO20",	"GPP_VGPIO20",
	"GPP_VGPIO21",	"GPP_VGPIO21",
	"GPP_VGPIO22",	"GPP_VGPIO22",
	"GPP_VGPIO23",	"GPP_VGPIO23",
	"GPP_VGPIO24",	"GPP_VGPIO24",
	"GPP_VGPIO25",	"GPP_VGPIO25",
	"GPP_VGPIO30",	"GPP_VGPIO30",
	"GPP_VGPIO31",	"GPP_VGPIO31",
	"GPP_VGPIO32",	"GPP_VGPIO32",
	"GPP_VGPIO33",	"GPP_VGPIO33",
	"GPP_VGPIO34",	"GPP_VGPIO34",
	"GPP_VGPIO35",	"GPP_VGPIO35",
	"GPP_VGPIO36",	"GPP_VGPIO36",
	"GPP_VGPIO37",	"GPP_VGPIO37",
	"GPP_VGPIO40",	"GPP_VGPIO40",
	"GPP_VGPIO41",	"GPP_VGPIO41",
	"GPP_VGPIO42",	"GPP_VGPIO42",
	"GPP_VGPIO43",	"GPP_VGPIO43",
	"GPP_VGPIO44",	"GPP_VGPIO44",
	"GPP_VGPIO45",	"GPP_VGPIO45",
	"GPP_VGPIO46",	"GPP_VGPIO46",
	"GPP_VGPIO47",	"GPP_VGPIO47",
};

const struct gpio_group meteorlake_pch_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(meteorlake_pch_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= meteorlake_pch_group_vgpio_names,
};

const struct gpio_group *const meteorlake_pch_community_0_groups[] = {
	&meteorlake_pch_group_cpu,
	&meteorlake_pch_group_v,
	&meteorlake_pch_group_c,
};

const struct gpio_community meteorlake_pch_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0xd1,
	.group_count	= ARRAY_SIZE(meteorlake_pch_community_0_groups),
	.groups		= meteorlake_pch_community_0_groups,
};

const struct gpio_group *const meteorlake_pch_community_1_groups[] = {
	&meteorlake_pch_group_a,
	&meteorlake_pch_group_e,
};

const struct gpio_community meteorlake_pch_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0xd2,
	.group_count	= ARRAY_SIZE(meteorlake_pch_community_1_groups),
	.groups		= meteorlake_pch_community_1_groups,
};

const struct gpio_group *const meteorlake_pch_community_3_groups[] = {
	&meteorlake_pch_group_h,
	&meteorlake_pch_group_f,
	&meteorlake_pch_group_spi0,
	&meteorlake_pch_group_vgpio3,
};

const struct gpio_community meteorlake_pch_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0xd3,
	.group_count	= ARRAY_SIZE(meteorlake_pch_community_3_groups),
	.groups		= meteorlake_pch_community_3_groups,
};

const struct gpio_group *const meteorlake_pch_community_4_groups[] = {
	&meteorlake_pch_group_s,
	&meteorlake_pch_group_jtag
};

const struct gpio_community meteorlake_pch_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0xd4,
	.group_count	= ARRAY_SIZE(meteorlake_pch_community_4_groups),
	.groups		= meteorlake_pch_community_4_groups,
};

const struct gpio_group *const meteorlake_pch_community_5_groups[] = {
	&meteorlake_pch_group_b,
	&meteorlake_pch_group_d,
	&meteorlake_pch_group_vgpio,
};

const struct gpio_community meteorlake_pch_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0xd5,
	.group_count	= ARRAY_SIZE(meteorlake_pch_community_5_groups),
	.groups		= meteorlake_pch_community_5_groups,
};

const struct gpio_community *const meteorlake_pch_communities[] = {
	&meteorlake_pch_community_0,
	&meteorlake_pch_community_1,
	&meteorlake_pch_community_3,
	&meteorlake_pch_community_4,
	&meteorlake_pch_community_5,
};

#endif
