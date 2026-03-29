/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GPIO_NAMES_PANTHERLAKE
#define GPIO_NAMES_PANTHERLAKE

#include "gpio_groups.h"

/* ----------------------------- Panther Lake ----------------------------- */

/* Community 0: Groups V, C */

const char *const pantherlake_pch_group_v_names[] = {
	"GPP_V00",                "BATLOW#",
	"GPP_V01",                "ACPRESENT",
	"GPP_V02",                "SOC_WAKE#",
	"GPP_V03",                "PWRBTN#",
	"GPP_V04",                "SLP_S3#",
	"GPP_V05",                "SLP_S4#",
	"GPP_V06",                "SLP_A#",
	"GPP_V07",                "SUSCLK",
	"GPP_V08",                "RSVD",
	"GPP_V09",                "SLP_S5#",
	"GPP_V10",                "LANPHYPC",
	"GPP_V11",                "SLP_LAN#",
	"GPP_V12",                "WAKE#",
	"GPP_V13",                "n/a",
	"GPP_V14",                "n/a",
	"GPP_V15",                "n/a",
	"GPP_V16",                "VCCST_EN",
	"GPP_V17",                "n/a",
	"GPP_PWROK",              "GPP_PWROK",
	"GPP_RESET_B",            "GPP_RESET_B",
	"GPP_BKLTEN",             "GPP_BKLTEN",
	"GPP_BKLTCTL",            "GPP_BKLTCTL",
	"GPP_VDDEN",              "GPP_VDDEN",
	"GPP_RST_B",              "GPP_RST_B",
};

const struct gpio_group pantherlake_pch_group_v = {
	.display	= "------- GPIO Group GPP_V -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_v_names) / 2,
	.func_count	= 2,
	.pad_names	= pantherlake_pch_group_v_names,
};

const char *const pantherlake_pch_group_c_names[] = {
	"GPP_C00",                "SMBCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C01",                "SMBDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C02",                "SMBALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C03",                "SML0CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C04",                "SML0DATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C05",                "SML0ALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C06",                "SML1CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C07",                "SML1DATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C08",                "SML1ALERT#",	"SOCHOT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C09",                "SRCCLKREQ0#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C10",                "SRCCLKREQ1#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C11",                "SRCCLKREQ2#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C12",                "SRCCLKREQ3#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C13",                "SRCCLKREQ4#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C14",                "SRCCLKREQ5#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C15",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C16",                "TBT_LSX0_TXD",	"DDP0_CTRLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C17",                "TBT_LSX0_RXD",	"DDP0_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C18",                "TBT_LSX1_TXD",	"DDP1_CTRLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C19",                "TBT_LSX1_RXD",	"DDP1_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C20",                "TBT_LSX2_TXD",	"DDP2_CTRLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C21",                "TBT_LSX2_RXD",	"DDP2_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C22",                "TBT_LSX3_TXD",	"DDP3_CTRLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_C23",                "TBT_LSX3_RXD",	"DDP3_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_c_names) / 8,
	.func_count	= 8,
	.pad_names	= pantherlake_pch_group_c_names,
};

/* Community 1: Groups F, E */

const char *const pantherlake_pch_group_f_names[] = {
	"GPP_F00",                "CNV_BRI_DT",	"UART2_RTS#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F01",                "CNV_BRI_RSP",	"UART2_RXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F02",                "CNV_RGI_DT",	"UART2_TXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F03",                "CNV_RGI_RSP",	"UART2_CTS#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F04",                "CNV_RF_RESET#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F05",                "n/a",	"n/a",	"CRF_CLKREQ",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F06",                "CNV_PA_BLANKING",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F07",                "RSVD",	"IMGCLKOUT2",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F08",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F09",                "n/a",	"SX_EXIT_HOLDOFF#",	"n/a",	"ISH_GP11",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F10",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP6A",
	"GPP_F11",                "n/a",	"n/a",	"THC1_SPI2_CLK",	"ISH_SPIA_CLK",	"GSPI1_CLK",	"n/a",	"n/a",	"n/a",
	"GPP_F12",                "THC_I2C1_SCL",	"I3C2_SCL",	"THC1_SPI2_IO0",	"ISH_SPIA_MISO",	"GSPI1_MOSI",	"n/a",	"n/a",	"I2C5_SCL",
	"GPP_F13",                "THC_I2C1_SDA",	"I3C2_SDA",	"THC1_SPI2_IO1",	"ISH_SPIA_MOSI",	"GSPI1_MISO",	"n/a",	"n/a",	"I2C5_SDA",
	"GPP_F14",                "n/a",	"n/a",	"THC1_SPI2_IO2",	"n/a",	"n/a",	"n/a",	"n/a",	"GSPI0A_MOSI",
	"GPP_F15",                "n/a",	"n/a",	"THC1_SPI2_IO3",	"n/a",	"n/a",	"n/a",	"n/a",	"GSPI0A_MISO",
	"GPP_F16",                "n/a",	"n/a",	"THC1_RST#",	"n/a",	"n/a",	"n/a",	"n/a",	"GSPI0A_CLK",
	"GPP_F17",                "n/a",	"n/a",	"THC1_SPI2_CS#",	"ISH_SPIA_CS#",	"GSPI1_CS0#",	"n/a",	"n/a",	"n/a",
	"GPP_F18",                "n/a",	"n/a",	"THC1_INT#",	"n/a",	"n/a",	"n/a",	"n/a",	"GSPI0A_CS0#",
	"GPP_F19",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F20",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F21",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_F22",                "n/a",	"n/a",	"THC1_DSYNC",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP8A",
	"GPP_F23",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP9A",
	"GPP_THC1_GSPI1_CLK_LPBK", "GPP_THC1_GSPI1_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_GSPI0_CLK_LOOPBK",   "GPP_GSPI0_CLK_LOOPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_f_names) / 9,
	.func_count	= 9,
	.pad_names	= pantherlake_pch_group_f_names,
};

const char *const pantherlake_pch_group_e_names[] = {
	"GPP_E00",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E01",                "PROC_GP2",	"RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP5A",
	"GPP_E02",                "PROC_GP3",	"VRALERT#",	"n/a",	"ISH_GP10",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E03",                "PROC_GP0",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E04",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E05",                "n/a",	"n/a",	"n/a",	"ISH_GP7",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E06",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E07",                "DDPA_CTRLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E08",                "DDPA_CTRLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E09",                "USB_OC0#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E10",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E11",                "n/a",	"n/a",	"THC0_SPI1_CLK",	"n/a",	"GSPI0_CLK",	"n/a",	"n/a",	"n/a",
	"GPP_E12",                "THC_I2C0_SCL",	"n/a",	"THC0_SPI1_IO0",	"n/a",	"GSPI0_MOSI",	"n/a",	"n/a",	"I2C4_SCL",
	"GPP_E13",                "THC_I2C0_SDA",	"n/a",	"THC0_SPI1_IO1",	"n/a",	"GSPI0_MISO",	"n/a",	"n/a",	"I2C4_SDA",
	"GPP_E14",                "n/a",	"n/a",	"THC0_SPI1_IO2",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E15",                "n/a",	"n/a",	"THC0_SPI1_IO3",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E16",                "n/a",	"n/a",	"THC0_RST#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E17",                "n/a",	"n/a",	"THC0_SPI1_CS#",	"n/a",	"GSPI0_CS0#",	"n/a",	"n/a",	"n/a",
	"GPP_E18",                "n/a",	"n/a",	"THC0_INT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E19",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E20",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E21",                "PMCALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_E22",                "n/a",	"n/a",	"THC0_DSYNC",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_BOOTHALT_B",         "GPP_BOOTHALT_B",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_THC0_GSPI_CLK_LPBK", "GPP_THC0_GSPI_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_e_names) / 9,
	.func_count	= 9,
	.pad_names	= pantherlake_pch_group_e_names,
};

/* Community 3: Groups JTAG, H, A, vGPIO3 */

const char *const pantherlake_pch_group_jtag_names[] = {
	"GPP_EPD_ON",             "GPP_EPD_ON",
	"GPP_VDD2_PWRGD",         "GPP_VDD2_PWRGD",
	"GPP_JTAG_MBPB0",         "JTAG_MBPB0",
	"GPP_JTAG_MBPB1",         "JTAG_MBPB1",
	"GPP_JTAG_MBPB2",         "JTAG_MBPB2",
	"GPP_JTAG_MBPB3",         "JTAG_MBPB3",
	"GPP_JTAG_TD0",           "JTAG_TD0",
	"GPP_PRDY_B",             "PRDY_B",
	"GPP_PREQ_B",             "PREQ_B",
	"GPP_JTAG_TDI",           "JTAG_TDI",
	"GPP_JTAG_TMS",           "JTAG_TMS",
	"GPP_JTAG_TCK",           "JTAG_TCK",
	"GPP_DBG_PMODE",          "DBG_PMODE",
	"GPP_JTAG_TRST_B",        "JTAG_TRST_B",
	"GPP_DDSP_HPDALV",        "DDSP_HPDALV",
};

const struct gpio_group pantherlake_pch_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= pantherlake_pch_group_jtag_names,
};

const char *const pantherlake_pch_group_h_names[] = {
	"GPP_H00",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H01",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H02",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H03",                "MIC_MUTE",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H04",                "I2C2_SDA",	"CNV_MFUART2_RXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H05",                "I2C2_SCL",	"CNV_MFUART2_TXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H06",                "I2C3_SDA",	"UART1_RXD",	"ISH_UART1A_RXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H07",                "I2C3_SCL",	"UART1_TXD",	"ISH_UART1A_TXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H08",                "UART0_RXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H09",                "UART0_TXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H10",                "UART0_RTS#",	"I3C1A_SDA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP10A",
	"GPP_H11",                "UART0_CTS#",	"I3C1A_SCL",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"ISH_GP11A",
	"GPP_H12",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H13",                "PROC_C10_GATE#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H14",                "ISH_UART1_RXD",	"UART1A_RXD",	"ISH_I2C1_SDA",	"ISH_I3C1_SDA",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H15",                "ISH_UART1_TXD",	"UART1A_TXD",	"ISH_I2C1_SCL",	"ISH_I3C1_SCL",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H16",                "TBT_LSX2_OE",	"n/a",	"PCIE_LINK_DOWN",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H17",                "MIC_MUTE_LED",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H18",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H19",                "I2C0_SDA",	"I3C0_SDA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H20",                "I2C0_SCL",	"I3C0_SCL",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H21",                "I2C1_SDA",	"I3C1_SDA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H22",                "I2C1_SCL",	"I3C1_SCL",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H23",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_H24",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_LPI3C1_CLK_LPBK",    "GPP_LPI3C1_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_LPI3C0_CLK_LPBK",    "GPP_LPI3C0_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_ISHI3C1_CLK_LPBK",   "GPP_ISHI3C1_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_h_names) / 9,
	.func_count	= 9,
	.pad_names	= pantherlake_pch_group_h_names,
};

const char *const pantherlake_pch_group_a_names[] = {
	"GPP_A00",                "ESPI_IO0",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A01",                "ESPI_IO1",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A02",                "ESPI_IO2",	"PRIMPWRDNACK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A03",                "ESPI_IO3",	"PRIMACK#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A04",                "ESPI_CS0#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A05",                "ESPI_CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A06",                "ESPI_RESET#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A07",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A08",                "PSE_SMLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A09",                "PSE_SMLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A10",                "PSE_SMLALERT#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A11",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A12",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A13",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A14",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A15",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A16",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_A17",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_IO_2",          "SPI0_IO_2",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_IO_3",          "SPI0_IO_3",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_MOSI_IO_0",     "SPI0_MOSI_IO_0",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_MOSI_IO_1",     "SPI0_MOSI_IO_1",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_TPM_CS_B",      "SPI0_TPM_CS_B",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_FLASH_0_CS_B",  "SPI0_FLASH_0_CS_B",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_FLASH_1_CS_B",  "SPI0_FLASH_1_CS_B",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_CLK",           "SPI0_CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_ESPI_CLK_LPBK",      "ESPI_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_SPI0_CLK_LOOPBK",    "SPI0_CLK_LOOPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_a_names) / 8,
	.func_count	= 8,
	.pad_names	= pantherlake_pch_group_a_names,
};

const char *const pantherlake_pch_group_vgpio3_names[] = {
	"GPP_VGPIO3_USB0",        "GPP_VGPIO3_USB0",
	"GPP_VGPIO3_USB1",        "GPP_VGPIO3_USB1",
	"GPP_VGPIO3_USB2",        "GPP_VGPIO3_USB2",
	"GPP_VGPIO3_USB3",        "GPP_VGPIO3_USB3",
	"GPP_VGPIO3_USB4",        "GPP_VGPIO3_USB4",
	"GPP_VGPIO3_USB5",        "GPP_VGPIO3_USB5",
	"GPP_VGPIO3_USB6",        "GPP_VGPIO3_USB6",
	"GPP_VGPIO3_USB7",        "GPP_VGPIO3_USB7",
	"GPP_VGPIO3_TS0",         "GPP_VGPIO3_TS0",
	"GPP_VGPIO3_TS1",         "GPP_VGPIO3_TS1",
	"GPP_VGPIO3_THC0",        "GPP_VGPIO3_THC0",
	"GPP_VGPIO3_THC1",        "GPP_VGPIO3_THC1",
	"GPP_VGPIO3_THC2",        "GPP_VGPIO3_THC2",
	"GPP_VGPIO3_THC3",        "GPP_VGPIO3_THC3",
};

const struct gpio_group pantherlake_pch_group_vgpio3 = {
	.display	= "------- GPIO Group VGPIO3 -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_vgpio3_names) / 2,
	.func_count	= 2,
	.pad_names	= pantherlake_pch_group_vgpio3_names,
};

/* Community 4: Group S */

const char *const pantherlake_pch_group_s_names[] = {
	"GPP_S00",                "SNDW3_CLK",	"n/a",	"n/a",	"n/a",	"n/a",	"I2S1_TXD",	"n/a",
	"GPP_S01",                "SNDW3_DATA0",	"n/a",	"n/a",	"n/a",	"n/a",	"I2S1_RXD",	"n/a",
	"GPP_S02",                "SNDW3_DATA1",	"n/a",	"SNDW0_CLK",	"n/a",	"DMIC_CLK_A0",	"I2S1_SCLK",	"n/a",
	"GPP_S03",                "SNDW3_DATA2",	"SNDW2_DATA1",	"SNDW0_DATA0",	"n/a",	"DMIC_DATA0",	"I2S1_SFRM",	"n/a",
	"GPP_S04",                "n/a",	"SNDW2_CLK",	"n/a",	"n/a",	"DMIC_CLK_A0",	"I2S2_SCLK",	"n/a",
	"GPP_S05",                "n/a",	"SNDW2_DATA0",	"n/a",	"n/a",	"DMIC_DATA0",	"I2S2_SFRM",	"n/a",
	"GPP_S06",                "n/a",	"SNDW2_DATA1",	"SNDW1_CLK",	"n/a",	"DMIC_CLK_A1",	"I2S2_TXD",	"n/a",
	"GPP_S07",                "SNDW3_DATA3",	"SNDW2_DATA2",	"SNDW1_DATA0",	"n/a",	"DMIC_DATA1",	"I2S2_RXD",	"n/a",
};

const struct gpio_group pantherlake_pch_group_s = {
	.display	= "------- GPIO Group GPP_S -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_s_names) / 8,
	.func_count	= 8,
	.pad_names	= pantherlake_pch_group_s_names,
};

/* Community 5: Groups B, D, vGPIO */

const char *const pantherlake_pch_group_b_names[] = {
	"GPP_B00",                "USB-C_SMLCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B01",                "USB-C_SMLDATA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B02",                "n/a",	"n/a",	"ISH_I2C0_SDA",	"ISH_I3C0_SDA",	"n/a",	"n/a",	"n/a",	"I2C2A_SDA",
	"GPP_B03",                "n/a",	"n/a",	"ISH_I2C0_SCL",	"ISH_I3C0_SCL",	"n/a",	"n/a",	"n/a",	"I2C2A_SCL",
	"GPP_B04",                "n/a",	"BK0",	"SBK0",	"ISH_GP0",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B05",                "n/a",	"BK1",	"SBK1",	"ISH_GP1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B06",                "n/a",	"BK2",	"SBK2",	"ISH_GP2",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B07",                "n/a",	"BK3",	"SBK3",	"ISH_GP3",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B08",                "n/a",	"BK4",	"SBK4",	"ISH_GP4",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B09",                "n/a",	"DDSP_HPD0#",	"DISP_MISC1",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B10",                "n/a",	"DDSP_HPD1#",	"DISP_MISC2",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B11",                "USB_OC1#",	"DDSP_HPD2#",	"DISP_MISC3",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B12",                "SLP_S0#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B13",                "PLTRST#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B14",                "USB_OC2#",	"DDSP_HPD3#",	"DISP_MISC4",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B15",                "USB_OC3#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B16",                "TBT_LSX1_OE",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B17",                "TBT_LSX0_OE",	"VDDEN2",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B18",                "ISH_I2C2_SDA",	"I2C4A_SDA",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B19",                "ISH_I2C2_SCL",	"I2C4A_SCL",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B20",                "n/a",	"I2C5A_SDA",	"n/a",	"ISH_GP8",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B21",                "n/a",	"I2C5A_SCL",	"n/a",	"ISH_GP9",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B22",                "TIME_SYNC0",	"n/a",	"n/a",	"ISH_GP5",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B23",                "TIME_SYNC1",	"n/a",	"n/a",	"ISH_GP6",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B24",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_B25",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_ISHI3C0_CLK_LPBK",   "GPP_ISHI3C0_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_b_names) / 9,
	.func_count	= 9,
	.pad_names	= pantherlake_pch_group_b_names,
};

const char *const pantherlake_pch_group_d_names[] = {
	"GPP_D00",                "IMGCLKOUT1",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D01",                "I2C3A_SDA",	"BKLTEN2",	"ISH_I2C2A_SDA",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D02",                "I2C3A_SCL",	"BKLTCTL2",	"ISH_I2C2A_SCL",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D03",                "PROC_GP1",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D04",                "IMGCLKOUT0",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D05",                "n/a",	"ISH_UART0_RXD",	"ISH_SPI_CS#",	"n/a",	"SML0BDATA",	"n/a",	"n/a",
	"GPP_D06",                "n/a",	"ISH_UART0_TXD",	"ISH_SPI_CLK",	"n/a",	"SML0BCLK",	"n/a",	"n/a",
	"GPP_D07",                "n/a",	"ISH_UART0_RTS#",	"ISH_SPI_MISO",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D08",                "n/a",	"ISH_UART0_CTS#",	"ISH_SPI_MOSI",	"n/a",	"SML0BALERT#",	"n/a",	"n/a",
	"GPP_D09",                "n/a",	"I2S_MCLK1_OUT",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D10",                "HDA_BCLK",	"I2S0_SCLK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D11",                "HDA_SYNC",	"I2S0_SFRM",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D12",                "HDA_SDO",	"I2S0_TXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D13",                "HDA_SDI0",	"I2S0_RXD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D14",                "n/a",	"TBT_LSX3_OE",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D15",                "n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D16",                "HDA_RST#",	"n/a",	"DMIC_CLK_A1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D17",                "HDA_SDI1",	"n/a",	"DMIC_DATA1",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D18",                "SRCCLKREQ6#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D19",                "n/a",	"TBT_LSX0_OE",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D20",                "SRCCLKREQ7#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D21",                "RSVD",	"SRCCLKREQ8#",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D22",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D23",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D24",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_D25",                "RSVD",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
	"GPP_ISDI3C1_CLK_LPBK",   "GPP_ISDI3C1_CLK_LPBK",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",	"n/a",
};

const struct gpio_group pantherlake_pch_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_d_names) / 8,
	.func_count	= 8,
	.pad_names	= pantherlake_pch_group_d_names,
};

const char *const pantherlake_pch_group_vgpio_names[] = {
	"GPP_VGPIO0",             "GPP_VGPIO0",
	"GPP_VGPIO5",             "GPP_VGPIO5",
	"GPP_VGPIO30",            "GPP_VGPIO30",
	"GPP_VGPIO31",            "GPP_VGPIO31",
	"GPP_VGPIO32",            "GPP_VGPIO32",
	"GPP_VGPIO33",            "GPP_VGPIO33",
	"GPP_VGPIO34",            "GPP_VGPIO34",
	"GPP_VGPIO35",            "GPP_VGPIO35",
	"GPP_VGPIO36",            "GPP_VGPIO36",
	"GPP_VGPIO37",            "GPP_VGPIO37",
	"GPP_VGPIO40",            "GPP_VGPIO40",
	"GPP_VGPIO41",            "GPP_VGPIO41",
	"GPP_VGPIO42",            "GPP_VGPIO42",
	"GPP_VGPIO43",            "GPP_VGPIO43",
	"GPP_VGPIO44",            "GPP_VGPIO44",
	"GPP_VGPIO45",            "GPP_VGPIO45",
	"GPP_VGPIO46",            "GPP_VGPIO46",
	"GPP_VGPIO47",            "GPP_VGPIO47",
};

const struct gpio_group pantherlake_pch_group_vgpio = {
	.display	= "------- GPIO Group VGPIO -------",
	.pad_count	= ARRAY_SIZE(pantherlake_pch_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= pantherlake_pch_group_vgpio_names,
};

/* Community definitions */

const struct gpio_group *const pantherlake_pch_community_0_groups[] = {
	&pantherlake_pch_group_v,
	&pantherlake_pch_group_c,
};

const struct gpio_community pantherlake_pch_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x59,
	.group_count	= ARRAY_SIZE(pantherlake_pch_community_0_groups),
	.groups		= pantherlake_pch_community_0_groups,
};

const struct gpio_group *const pantherlake_pch_community_1_groups[] = {
	&pantherlake_pch_group_f,
	&pantherlake_pch_group_e,
};

const struct gpio_community pantherlake_pch_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x5a,
	.group_count	= ARRAY_SIZE(pantherlake_pch_community_1_groups),
	.groups		= pantherlake_pch_community_1_groups,
};

const struct gpio_group *const pantherlake_pch_community_3_groups[] = {
	&pantherlake_pch_group_jtag,
	&pantherlake_pch_group_h,
	&pantherlake_pch_group_a,
	&pantherlake_pch_group_vgpio3,
};

const struct gpio_community pantherlake_pch_community_3 = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x5b,
	.group_count	= ARRAY_SIZE(pantherlake_pch_community_3_groups),
	.groups		= pantherlake_pch_community_3_groups,
};

const struct gpio_group *const pantherlake_pch_community_4_groups[] = {
	&pantherlake_pch_group_s,
};

const struct gpio_community pantherlake_pch_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x5c,
	.group_count	= ARRAY_SIZE(pantherlake_pch_community_4_groups),
	.groups		= pantherlake_pch_community_4_groups,
};

const struct gpio_group *const pantherlake_pch_community_5_groups[] = {
	&pantherlake_pch_group_b,
	&pantherlake_pch_group_d,
	&pantherlake_pch_group_vgpio,
};

const struct gpio_community pantherlake_pch_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x5d,
	.group_count	= ARRAY_SIZE(pantherlake_pch_community_5_groups),
	.groups		= pantherlake_pch_community_5_groups,
};

const struct gpio_community *const pantherlake_pch_communities[] = {
	&pantherlake_pch_community_0,
	&pantherlake_pch_community_1,
	&pantherlake_pch_community_3,
	&pantherlake_pch_community_4,
	&pantherlake_pch_community_5,
};

#endif
