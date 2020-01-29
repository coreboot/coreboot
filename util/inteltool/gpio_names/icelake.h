#ifndef GPIO_NAMES_ICELAKE_H
#define GPIO_NAMES_ICELAKE_H

#include "gpio_groups.h"

static const char *const icelake_pch_h_group_g_names[] = {
	/* GPP_G */
	"GPP_G0", "SD3_CMD",
	"GPP_G1", "SD3_D0",
	"GPP_G2", "SD3_D1",
	"GPP_G3", "SD3_D2",
	"GPP_G4", "SD3_D3",
	"GPP_G5", "SD3_CDB",
	"GPP_G6", "SD3_CLK",
	"GPP_G7", "SD3_WP",
};

static const char *const icelake_pch_h_group_b_names[] = {
	/* GPP_B */
	"GPP_B0", "CORE_VID_0",
	"GPP_B1", "CORE_VID_1",
	"GPP_B2", "VRALERTB",
	"GPP_B3", "CPU_GP_2",
	"GPP_B4", "CPU_GP_3",
	"GPP_B5", "ISH_I2C0_SDA",
	"GPP_B6", "ISH_I2C0_SCL",
	"GPP_B7", "ISH_I2C1_SDA",
	"GPP_B8", "ISH_I2C1_SCL",
	"GPP_B9", "I2C5_SDA",
	"GPP_B10", "I2C5_SCL",
	"GPP_B11", "PMCALERTB",
	"GPP_B12", "SLP_S0B",
	"GPP_B13", "PLTRSTB",
	"GPP_B14", "SPKR",
	"GPP_B15", "GSPI0_CS0B",
	"GPP_B16", "GSPI0_CLK",
	"GPP_B17", "GSPI0_MISO",
	"GPP_B18", "GSPI0_MOSI",
	"GPP_B19", "GSPI1_CS0B",
	"GPP_B20", "GSPI1_CLK",
	"GPP_B21", "GSPI1_MISO",
	"GPP_B22", "GSPI1_MOSI",
	"GPP_B23", "SML1ALERTB",
	"GPP_B24", "GSPI0_CLK_LOOPBK",
	"GPP_B25", "GSPI1_CLK_LOOPBK",
};

static const char *const icelake_pch_h_group_a_names[] = {
	/* GPP_A */
	"GPP_A0", "ESPI_IO_0",
	"GPP_A1", "ESPI_IO_1",
	"GPP_A2", "ESPI_IO_2",
	"GPP_A3", "ESPI_IO_3",
	"GPP_A4", "ESPI_CSB",
	"GPP_A5", "ESPI_CLK",
	"GPP_A6", "ESPI_RESETB",
	"GPP_A7", "I2S2_SCLK",
	"GPP_A8", "I2S2_SFRM",
	"GPP_A9", "I2S2_TXD",
	"GPP_A10", "I2S2_RXD",
	"GPP_A11", "SATA_DEVSLP_2",
	"GPP_A12", "SATAXPCIE_1",
	"GPP_A13", "SATAXPCIE_2",
	"GPP_A14", "USB2_OCB_1",
	"GPP_A15", "USB2_OCB_2",
	"GPP_A16", "USB2_OCB_3",
	"GPP_A17", "DDSP_HPD_C",
	"GPP_A18", "DDSP_HPD_B",
	"GPP_A19", "DDSP_HPD_1",
	"GPP_A20", "DDSP_HPD_2",
	"GPP_A21", "I2S5_TXD",
	"GPP_A22", "I2S5_RXD",
	"GPP_A23", "I2S1_SCLK",
	"GPP_A24", "ESPI_CLK_LOOPBK",
};

static const char *const icelake_pch_h_group_h_names[] = {
	/* GPP_H */
	"GPP_H0", "SD_1P8_SEL",
	"GPP_H1", "SD_PWR_EN_B",
	"GPP_H2", "GPPC_H_2",
	"GPP_H3", "SX_EXIT_HOLDOFFB",
	"GPP_H4", "I2C2_SDA",
	"GPP_H5", "I2C2_SCL",
	"GPP_H6", "I2C3_SDA",
	"GPP_H7", "I2C3_SCL",
	"GPP_H8", "I2C4_SDA",
	"GPP_H9", "I2C4_SCL",
	"GPP_H10", "SRCCLKREQB_4",
	"GPP_H11", "SRCCLKREQB_5",
	"GPP_H12", "M2_SKT2_CFG_0",
	"GPP_H13", "M2_SKT2_CFG_1",
	"GPP_H14", "M2_SKT2_CFG_2",
	"GPP_H15", "M2_SKT2_CFG_3",
	"GPP_H16", "DDPB_CTRLCLK",
	"GPP_H17", "DDPB_CTRLDATA",
	"GPP_H18", "CPU_VCCIO_PWR_GATEB",
	"GPP_H19", "TIME_SYNC_0",
	"GPP_H20", "IMGCLKOUT_1",
	"GPP_H21", "IMGCLKOUT_2",
	"GPP_H22", "IMGCLKOUT_3",
	"GPP_H23", "IMGCLKOUT_4",
};

static const char *const icelake_pch_h_group_d_names[] = {
	/* GPP_D */
	"GPP_D0", "ISH_GP_0",
	"GPP_D1", "ISH_GP_1",
	"GPP_D2", "ISH_GP_2",
	"GPP_D3", "ISH_GP_3",
	"GPP_D4", "IMGCLKOUT_0",
	"GPP_D5", "SRCCLKREQB_0",
	"GPP_D6", "SRCCLKREQB_1",
	"GPP_D7", "SRCCLKREQB_2",
	"GPP_D8", "SRCCLKREQB_3",
	"GPP_D9", "ISH_SPI_CSB",
	"GPP_D10", "ISH_SPI_CLK",
	"GPP_D11", "ISH_SPI_MISO",
	"GPP_D12", "ISH_SPI_MOSI",
	"GPP_D13", "ISH_UART0_RXD",
	"GPP_D14", "ISH_UART0_TXD",
	"GPP_D15", "ISH_UART0_RTSB",
	"GPP_D16", "ISH_UART0_CTSB",
	"GPP_D17", "ISH_GP_4",
	"GPP_D18", "ISH_GP_5",
	"GPP_D19", "I2S_MCLK",
	"GPP_D10", "GSPI2_CLK_LOOPBK",
};

static const char *const icelake_pch_h_group_f_names[] = {
	/* GPP_F */
	"GPP_F0", "CNV_BRI_DT",
	"GPP_F1", "CNV_BRI_RSP",
	"GPP_F2", "CNV_RGI_DT",
	"GPP_F3", "CNV_RGI_RSP",
	"GPP_F4", "CNV_RF_RESET_B",
	"GPP_F5", "EMMC_HIP_MON",
	"GPP_F6", "CNV_PA_BLANKING",
	"GPP_F7", "EMMC_CMD",
	"GPP_F8", "EMMC_DATA0",
	"GPP_F9", "EMMC_DATA1",
	"GPP_F10", "EMMC_DATA2",
	"GPP_F11", "EMMC_DATA3",
	"GPP_F12", "EMMC_DATA4",
	"GPP_F13", "EMMC_DATA5",
	"GPP_F14", "EMMC_DATA6",
	"GPP_F15", "EMMC_DATA7",
	"GPP_F16", "EMMC_RCLK",
	"GPP_F17", "EMMC_CLK",
	"GPP_F18", "EMMC_RESETB",
	"GPP_F19", "A4WP_PRESENT",
};

static const char *const icelake_pch_h_group_vgpio_names[] = {
	/* vGPIO */
	"CNV_BTEN", "",
	"CNV_WCEN", "",
	"CNV_BT_HOST_WAKEB", "",
	"CNV_BT_IF_SELECT", "",
	"vCNV_BT_UART_TXD", "",
	"vCNV_BT_UART_RXD", "",
	"vCNV_BT_UART_CTS_B", "",
	"vCNV_BT_UART_RTS_B", "",
	"vCNV_MFUART1_TXD", "",
	"vCNV_MFUART1_RXD", "",
	"vCNV_MFUART1_CTS_B", "",
	"vCNV_MFUART1_RTS_B", "",
	"vUART0_TXD", "",
	"vUART0_RXD", "",
	"vUART0_CTS_B", "",
	"vUART0_RTS_B", "",
	"vISH_UART0_TXD", "",
	"vISH_UART0_RXD", "",
	"vISH_UART0_CTS_B", "",
	"vISH_UART0_RTS_B", "",
	"vCNV_BT_I2S_BCLK", "",
	"vCNV_BT_I2S_WS_SYNC", "",
	"vCNV_BT_I2S_SDO", "",
	"vCNV_BT_I2S_SDI", "",
	"vI2S2_SCLK", "",
	"vI2S2_SFRM", "",
	"vI2S2_TXD", "",
	"vI2S2_RXD", "",
	"vSD3_CD_B", "",
};

static const char *const icelake_pch_h_group_c_names[] = {
	/* GPP_C */
	"GPP_C0", "SMBCLK",
	"GPP_C1", "SMBDATA",
	"GPP_C2", "SMBALERTB",
	"GPP_C3", "SML0CLK",
	"GPP_C4", "SML0DATA",
	"GPP_C5", "SML0ALERTB",
	"GPP_C6", "SML1CLK",
	"GPP_C7", "SML1DATA",
	"GPP_C8", "UART0_RXD",
	"GPP_C9", "UART0_TXD",
	"GPP_C10", "UART0_RTSB",
	"GPP_C11", "UART0_CTSB",
	"GPP_C12", "UART1_RXD",
	"GPP_C13", "UART1_TXD",
	"GPP_C14", "UART1_RTSB",
	"GPP_C15", "UART1_CTSB",
	"GPP_C16", "I2C0_SDA",
	"GPP_C17", "I2C0_SCL",
	"GPP_C18", "I2C1_SDA",
	"GPP_C19", "I2C1_SCL",
	"GPP_C20", "UART2_RXD",
	"GPP_C21", "UART2_TXD",
	"GPP_C22", "UART2_RTSB",
	"GPP_C23", "UART2_CTSB",
};

static const char *const icelake_pch_h_group_hvcmos_names[] = {
	/* HVCMOS */
	"L_BKLTEN", "",
	"L_BKLTCTL", "",
	"L_VDDEN", "",
	"SYS_PWROK", "",
	"SYS_RESETB", "",
	"MLK_RSTB", "",
};

static const char *const icelake_pch_h_group_e_names[] = {
	/* GPP_E */
	"GPP_E0", "SATAXPCIE_0",
	"GPP_E1", "SPI1_IO_2",
	"GPP_E2", "SPI1_IO_3",
	"GPP_E3", "CPU_GP_0",
	"GPP_E4", "SATA_DEVSLP_0",
	"GPP_E5", "SATA_DEVSLP_1",
	"GPP_E6", "GPPC_E_6",
	"GPP_E7", "CPU_GP_1",
	"GPP_E8", "SATA_LEDB",
	"GPP_E9", "USB2_OCB_0",
	"GPP_E10", "SPI1_CSB",
	"GPP_E11", "SPI1_CLK",
	"GPP_E12", "SPI1_MISO_IO_1",
	"GPP_E13", "SPI1_MOSI_IO_0",
	"GPP_E14", "DDSP_HPD_A",
	"GPP_E15", "ISH_GP_6",
	"GPP_E16", "ISH_GP_7",
	"GPP_E17", "DISP_MISC_4",
	"GPP_E18", "DDP1_CTRLCLK",
	"GPP_E19", "DDP1_CTRLDATA",
	"GPP_E20", "DDP2_CTRLCLK",
	"GPP_E21", "DDP2_CTRLDATA",
	"GPP_E22", "DDPA_CTRLCLK",
	"GPP_E23", "DDPA_CTRLDATA",
};

static const char *const icelake_pch_h_group_jtag_names[] = {
	/* JTAG */
	"JTAG0", "JTAG_TDO",
	"JTAG1", "JTAGX",
	"JTAG2", "PRDYB",
	"JTAG3", "PREQB",
	"JTAG4", "CPU_TRSTB",
	"JTAG5", "JTAG_TDI",
	"JTAG6", "JTAG_TMS",
	"JTAG7", "JTAG_TCK",
	"JTAG8", "ITP_PMODE",
};

static const char *const icelake_pch_h_group_r_names[] = {
	/* GPP_R */
	"GPP_R0", "HDA_BCLK",
	"GPP_R1", "HDA_SYNC",
	"GPP_R2", "HDA_SDO",
	"GPP_R3", "HDA_SDI_0",
	"GPP_R4", "HDA_RSTB",
	"GPP_R5", "HDA_SDI_1",
	"GPP_R6", "I2S1_TXD",
	"GPP_R7", "I2S1_RXD",
};

static const char *const icelake_pch_h_group_s_names[] = {
	/* GPP_S */
	"GPP_S0", "SNDW1_CLK",
	"GPP_S1", "SNDW1_DATA",
	"GPP_S2", "SNDW2_CLK",
	"GPP_S3", "SNDW2_DATA",
	"GPP_S4", "SNDW3_CLK",
	"GPP_S5", "SNDW3_DATA",
	"GPP_S6", "SNDW4_CLK",
	"GPP_S7", "SNDW4_DATA",
};

static const char *const icelake_pch_h_group_spi_names[] = {
	/* SPI */
	"SPIP0", "SPI0_IO_2",
	"SPIP1", "SPI0_IO_3",
	"SPIP2", "SPI0_MOSI_IO_0",
	"SPIP3", "SPI0_MISO_IO_1",
	"SPIP4", "SPI0_TPM_CSB",
	"SPIP5", "SPI0_FLASH_0_CSB",
	"SPIP6", "SPI0_FLASH_1_CSB",
	"SPIP7", "SPI0_CLK",
	"SPIP8", "SPI0_CLK_LOOPBK",
};

/* Ice Lake-LP */
static const struct gpio_group icelake_pch_h_group_g = {
	.display	= "------- GPIO Group GPP_G -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_g_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_g_names,
};

static const struct gpio_group icelake_pch_h_group_b = {
	.display	= "------- GPIO Group GPP_B -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_b_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_b_names,
};

static const struct gpio_group icelake_pch_h_group_a = {
	.display	= "------- GPIO Group GPP_A -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_a_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_a_names,
};

static const struct gpio_group *const icelake_pch_h_community_0_groups[] = {
	&icelake_pch_h_group_g,
	&icelake_pch_h_group_b,
	&icelake_pch_h_group_a,
};

static	const struct gpio_community icelake_pch_h_community_0 = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(icelake_pch_h_community_0_groups),
	.groups		= icelake_pch_h_community_0_groups,
};

static const struct gpio_group icelake_pch_h_group_h = {
	.display	= "------- GPIO Group GPP_H -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_h_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_h_names,
};

static const struct gpio_group icelake_pch_h_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_d_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_d_names,
};

static const struct gpio_group icelake_pch_h_group_f = {
	.display	= "------- GPIO Group GPP_F -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_f_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_f_names,
};

static const struct gpio_group icelake_pch_h_group_vgpio_0 = {
	.display	= "------- GPIO Group vGPIO_0 -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_vgpio_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_vgpio_names,
};

static const struct gpio_group *const icelake_pch_h_community_1_groups[] = {
	&icelake_pch_h_group_h,
	&icelake_pch_h_group_d,
	&icelake_pch_h_group_f,
	&icelake_pch_h_group_vgpio_0,
};

static	const struct gpio_community icelake_pch_h_community_1 = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(icelake_pch_h_community_1_groups),
	.groups		= icelake_pch_h_community_1_groups,
};


static	const struct gpio_community icelake_pch_h_community_2 = {
	.name		= "------- GPIO Community 2 (skipped)-------",
	.pcr_port_id	= 0x6c,
	.group_count	= 0,
};

static	const struct gpio_community icelake_pch_h_community_3 = {
	.name		= "------- GPIO Community 3 (skipped)-------",
	.pcr_port_id	= 0x6b,
	.group_count	= 0,
};

static const struct gpio_group icelake_pch_h_group_c = {
	.display	= "------- GPIO Group GPP_C -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_c_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_c_names,
};

static const struct gpio_group icelake_pch_h_group_hvcmos = {
	.display	= "------- GPIO Group HVCMOS -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_hvcmos_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_hvcmos_names,
};

static const struct gpio_group icelake_pch_h_group_e = {
	.display	= "------- GPIO Group E -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_e_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_e_names,
};

static const struct gpio_group icelake_pch_h_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_jtag_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_jtag_names,
};

static const struct gpio_group *const icelake_pch_h_community_4_groups[] = {
	&icelake_pch_h_group_c,
	&icelake_pch_h_group_hvcmos,
	&icelake_pch_h_group_e,
	&icelake_pch_h_group_jtag,
};

static	const struct gpio_community icelake_pch_h_community_4 = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(icelake_pch_h_community_4_groups),
	.groups		= icelake_pch_h_community_4_groups,
};

static const struct gpio_group icelake_pch_h_group_r = {
	.display	= "------- GPIO Group R -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_r_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_r_names,
};

static const struct gpio_group icelake_pch_h_group_s = {
	.display	= "------- GPIO Group S -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_s_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_s_names,
};

static const struct gpio_group icelake_pch_h_group_spi = {
	.display	= "------- GPIO Group SPI -------",
	.pad_count	= ARRAY_SIZE(icelake_pch_h_group_spi_names) / 2,
	.func_count	= 2,
	.pad_names	= icelake_pch_h_group_spi_names,
};

static const struct gpio_group *const icelake_pch_h_community_5_groups[] = {
	&icelake_pch_h_group_r,
	&icelake_pch_h_group_s,
	&icelake_pch_h_group_spi,
};

static const struct gpio_community icelake_pch_h_community_5 = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(icelake_pch_h_community_5_groups),
	.groups		= icelake_pch_h_community_5_groups,
};

static const struct gpio_community *const icelake_pch_h_communities[] = {
	&icelake_pch_h_community_0,
	&icelake_pch_h_community_1,
	&icelake_pch_h_community_2,
	&icelake_pch_h_community_3,
	&icelake_pch_h_community_4,
	&icelake_pch_h_community_5,
};

#endif
