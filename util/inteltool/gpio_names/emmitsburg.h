#ifndef GPIO_NAMES_EMMITSBURG_H
#define GPIO_NAMES_EMMITSBURG_H

#include "gpio_groups.h"

static const char *const emmitsburg_group_a_names[] = {
	"GPPC_A0",	"ESPI_ALERT0_N",		"n/a",
	"GPPC_A1",	"ESPI_ALERT1_N",		"n/a",
	"GPPC_A2",	"ESPI_IO_0",		"n/a",
	"GPPC_A3",	"ESPI_IO_1",		"n/a",
	"GPPC_A4",	"ESPI_IO_2",		"n/a",
	"GPPC_A5",	"ESPI_IO_3",		"n/a",
	"GPPC_A6",	"ESPI_CS0_N",		"ESPI_CS1_N",
	"GPPC_A7",	"ESPI_CS1_N",		"ESPI_CS0_N",
	"GPPC_A8",	"ESPI_RESET_N",		"n/a",
	"GPPC_A9",	"ESPI_CLK",		"n/a",
	"GPPC_A10",	"SRCCLKREQ0_N",		"n/a",
	"GPPC_A11",	"SRCCLKREQ1_N",		"n/a",
	"GPPC_A12",	"SRCCLKREQ2_N",		"n/a",
	"GPPC_A13",	"SRCCLKREQ3_N",		"n/a",
	"GPPC_A14",	"SRCCLKREQ4_N",		"n/a",
	"GPPC_A15",	"SRCCLKREQ5_N",		"n/a",
	"GPPC_A16",	"SRCCLKREQ6_N",		"n/a",
	"GPPC_A17",	"SRCCLKREQ7_N",		"n/a",
	"GPPC_A18",	"SRCCLKREQ8_N",		"n/a",
	"GPPC_A19",	"SRCCLKREQ9_N",		"n/a",
	"ESPI_CLK_LOOPBK",	"n/a",		"n/a",
};

static const char *const emmitsburg_group_b_names[] = {
	"GPPC_B0","GSXDOUT","n/a","1P8_VISA_CH0_CLK","1P8_PTI_CLK0","n/a","n/a","n/a","n/a",
	"GPPC_B1","GSXSLOAD","n/a","1P8_VISA_CH0_D0","1P8_PTI_DATA0","n/a","n/a","n/a","n/a",
	"GPPC_B2","GSXDIN","n/a","1P8_VISA_CH0_D1","1P8_PTI_DATA1","n/a","n/a","n/a","n/a",
	"GPPC_B3","GSXRESET_N","n/a","1P8_VISA_CH0_D2","1P8_PTI_DATA2","n/a","n/a","n/a","n/a",
	"GPPC_B4","GSXCLK","n/a","1P8_VISA_CH0_D3","1P8_PTI_DATA3","n/a","n/a","n/a","n/a",
	"GPPC_B5","USB2_OC0_N","n/a","1P8_VISA_CH0_D4","1P8_PTI_DATA4","n/a","n/a","n/a","n/a",
	"GPPC_B6","USB2_OC1_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B7","USB2_OC2_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B8","USB2_OC3_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B9","USB2_OC4_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B10","USB2_OC5_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B11","USB2_OC6_N","n/a","n/a","n/a","n/a","n/a","n/a","n/a",
	"GPPC_B12","HS_UART0_RXD","n/a","1P8_VISA_CH0_D5","1P8_PTI_DATA5","n/a","n/a","n/a","n/a",
	"GPPC_B13","HS_UART0_TXD","n/a","1P8_VISA_CH0_D6","1P8_PTI_DATA6","n/a","n/a","n/a","n/a",
	"GPPC_B14","HS_UART0_RTS_N","n/a","1P8_VISA_CH0_D7","1P8_PTI_DATA7","n/a","n/a","n/a","n/a",
	"GPPC_B15","HS_UART0_CTS_N","n/a","1P8_VISA_CH1_D0","1P8_PTI_DATA8","n/a","n/a","n/a","n/a",
	"GPPC_B16","HS_UART1_RXD","n/a","1P8_VISA_CH1_D1","1P8_PTI_DATA9","n/a","n/a","n/a","n/a",
	"GPPC_B17","HS_UART1_TXD","n/a","1P8_VISA_CH1_D2","1P8_PTI_DATA10","n/a","n/a","n/a","n/a",
	"GPPC_B18","HS_UART1_RTX_N","n/a","1P8_VISA_CH1_D3","1P8_PTI_DATA11","n/a","n/a","n/a","n/a",
	"GPPC_B19","HS_UART1_CTS_N","n/a","1P8_VISA_CH1_D4","1P8_PTI_DATA12","n/a","n/a","SBK0","BK0",
	"GPPC_B20","n/a","n/a","1P8_VISA_CH1_D5","1P8_PTI_DATA13","n/a","n/a","SBK1","BK1",
	"GPPC_B21","n/a","n/a","1P8_VISA_CH1_D6","1P8_PTI_DATA14","n/a","n/a","SBK2","BK2",
	"GPPC_B22","n/a","n/a","1P8_VISA_CH1_D7","1P8_PTI_DATA15","n/a","n/a","SBK3","BK3",
	"GPPC_B23","PS_ONB","n/a","1P8_VISA_CH1_CLK","n/a","n/a","GPPB_EC_23","SBK4","BK4",
};

static const char *const emmitsburg_group_s_names[] = {
	"GPPC_S0","TIME_SYNC_0","n/a",
	"GPPC_S1","SPRK","TIME_SYNC_1",
	"GPPC_S2","CPU_GP_0","n/a",
	"GPPC_S3","CPU_GP_1","n/a",
	"GPPC_S4","CPU_GP_2","n/a",
	"GPPC_S5","CPU_GP_3","n/a",
	"GPPC_S6","SUSWARN_N_SUSWRDNACK","n/a",
	"GPPC_S7","SUSACK_N","n/a",
	"GPPC_S8","NMI_N","n/a",
	"GPPC_S9","SMI_N","n/a",
	"GPPC_S10","n/a","n/a",
	"GPPC_S11","n/a","n/a",
	"SPI_CLK_LOOPBK",	"n/a",		"n/a",
};

static const struct gpio_group emmitsburg_group_a = {
	.display	= "------- GPIO Group GPPC_A -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_a_names) / 3,
	.func_count	= 3,
	.pad_names	= emmitsburg_group_a_names,
};

static const struct gpio_group emmitsburg_group_b = {
	.display	= "------- GPIO Group GPPC_B -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_b_names) / 9,
	.func_count	= 9,
	.pad_names	= emmitsburg_group_b_names,
};

static const struct gpio_group emmitsburg_group_s = {
	.display	= "------- GPIO Group GPPC_S -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_s_names) / 3,
	.func_count	= 3,
	.pad_offset = 0xA50,
	.pad_names	= emmitsburg_group_s_names,
};

static const struct gpio_group *const emmitsburg_community0_abs_groups[] = {
	&emmitsburg_group_a,
	&emmitsburg_group_b,
	&emmitsburg_group_s,
};

static const struct gpio_community emmitsburg_community0_abs = {
	.name		= "------- GPIO Community 0 -------",
	.pcr_port_id	= 0x6e,
	.group_count	= ARRAY_SIZE(emmitsburg_community0_abs_groups),
	.groups		= emmitsburg_community0_abs_groups,
};

static const char *const emmitsburg_group_c_names[] = {
	"GPPC_C0",	"ME_SML0CLK",
	"GPPC_C1",	"ME_SML0DATA",
	"GPPC_C2",	"ME_SML0ALERT_N",
	"GPPC_C3",	"ME_SML0BDATA",
	"GPPC_C4",	"ME_SML0BCLK",
	"GPPC_C5",	"ME_SML0BALART_N",
	"GPPC_C6",	"ME_SML1CLK",
	"GPPC_C7",	"ME_SML1DATA",
	"GPPC_C8",	"ME_SML1ALERT_N",
	"GPPC_C9",	"ME_SML2CLK",
	"GPPC_C10",	"ME_SML2DATA",
	"GPPC_C11",	"ME_SML2ALERT_N",
	"GPPC_C12",	"ME_SML3CLK",
	"GPPC_C13",	"ME_SML3DATA",
	"GPPC_C14",	"ME_SML3ALERT_N",
	"GPPC_C15",	"ME_SML4CLK",
	"GPPC_C16",	"ME_SML4DATA",
	"GPPC_C17",	"ME_SML4ALERT_N",
	"GPPC_C18",	"n/a",
	"GPPC_C19",	"MC_SMBCLK",
	"GPPC_C20",	"MC_SMBDATA",
	"GPPC_C21",	"MC_SMBALERT_N",
};

static const struct gpio_group emmitsburg_group_c = {
	.display	= "------- GPIO Group GPPC_C -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_c_names) / 2,
	.func_count	= 2,
	.pad_names	= emmitsburg_group_c_names,
};

static const struct gpio_group *const emmitsburg_community1_c_groups[] = {
	&emmitsburg_group_c,
};

static const char *const emmitsburg_group_d_names[] = {
	"GPP_D0",	"HS_SMBCLK",
	"GPP_D1",	"HS_SMBDATA",
	"GPP_D2",	"HS_SMBALERT_N",
	"GPP_D3",	"n/a", // Not described in EDS, add here to take space
	"GPP_D4",	"n/a", // Not described in EDS, add here to take space
	"GPP_D5",	"n/a", // Not described in EDS, add here to take space
	"GPP_D6",	"n/a",
	"GPP_D7",	"n/a",
	"GPP_D8",	"CRASHLOG_TRIG_N",
	"GPP_D9",	"PME_N",
	"GPP_D10",	"BM_BUSY_N",
	"GPP_D11",	"PLTRST_N",
	"GPP_D12",	"PCHHOT_N",
	"GPP_D13",	"ADR_COMPLETE",
	"GPP_D14",	"ADR_TRIGGER",
	"GPP_D15",	"VRALERT_N",
	"GPP_D16",	"ADR_ACK",
	"GPP_D17",	"THERMTRIP_N",
	"GPP_D18",	"MEMTRIP_N",
	"GPP_D19",	"MSMI_N",
	"GPP_D20",	"CATERR_N",
	"GPP_D21",	"GLB_RST_WARN_N",
	"GPP_D22",	"USB2_OCB_7",
	"GPP_D23",	"n/a",
};

static const struct gpio_group emmitsburg_group_d = {
	.display	= "------- GPIO Group GPP_D -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_d_names) / 2,
	.func_count	= 2,
	.pad_names	= emmitsburg_group_d_names,
};

static const struct gpio_group *const emmitsburg_community1_cd_groups[] = {
	&emmitsburg_group_c,
	&emmitsburg_group_d,
};

static const struct gpio_community emmitsburg_community1_cd = {
	.name		= "------- GPIO Community 1 -------",
	.pcr_port_id	= 0x6d,
	.group_count	= ARRAY_SIZE(emmitsburg_community1_cd_groups),
	.groups		= emmitsburg_community1_cd_groups,
};

static const char *const emmitsburg_group_e_names[] = {
	"GPP_E0",	"SATA1_XPCIE_0", "n/a",
	"GPP_E1",	"SATA1_XPCIE_1", "n/a",
	"GPP_E2",	"SATA1_XPCIE_2", "n/a",
	"GPP_E3",	"SATA1_XPCIE_3", "n/a",
	"GPP_E4",	"SATA0_XPCIE_2", "n/a",
	"GPP_E5",	"SATA0_XPCIE_1", "n/a",
	"GPP_E6",	"SATA0_USB3_XPCIE_0", "n/a",
	"GPP_E7",	"SATA0_USB3_XPCIE_1", "n/a",
	"GPP_E8",	"SATA0_SCLOCK", "SATA0_LED_N",
	"GPP_E9",	"SATA0_SLOAD", "SATA0_DEVSLP",
	"GPP_E10",	"SATA0_SDATAOUT", "SATA1_GP",
	"GPP_E11",	"SATA1_SCLOCK", "SATA1_LED_N",
	"GPP_E12",	"SATA1_SLOAD", "SATA1_GP",
	"GPP_E13",	"SATA1_SDATAOUT", "SATA1_DEVSLP",
	"GPP_E14",	"SATA2_SCLOCK", "SATA2_LED_N",
	"GPP_E15",	"SATA2_SLOAD", "SATA2_GP",
	"GPP_E16",	"SATA2_SDATAOUT", "SATA2_DEVSLP",
	"GPP_E17",	"n/a", "ERR0_N",
	"GPP_E18",	"n/a", "ERR1_N",
	"GPP_E19",	"n/a", "ERR2_N",
};

static const struct gpio_group emmitsburg_group_e = {
	.display	= "------- GPIO Group GPP_E -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_e_names) / 3,
	.func_count	= 3,
	.pad_names	= emmitsburg_group_e_names,
};

static const char *const emmitsburg_group_jtag_names[] = {
	"JTAG_0",
	"JTAG_1",
	"JTAG_2",
	"JTAG_3",
	"JTAG_4",
	"JTAG_5",
	"JTAG_6",
	"JTAG_7",
	"JTAG_8",
	"JTAG_9",
};

static const struct gpio_group emmitsburg_group_jtag = {
	.display	= "------- GPIO Group JTAG -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_jtag_names) / 1,
	.func_count	= 1,
	.pad_offset = 0x880,
	.pad_names	= emmitsburg_group_jtag_names,
};

static const struct gpio_group *const emmitsburg_community3_e_jtag_groups[] = {
	&emmitsburg_group_e,
	&emmitsburg_group_jtag,
};

static const struct gpio_community emmitsburg_community3_e_jtag = {
	.name		= "------- GPIO Community 3 -------",
	.pcr_port_id	= 0x6b,
	.group_count	= ARRAY_SIZE(emmitsburg_community3_e_jtag_groups),
	.groups		= emmitsburg_community3_e_jtag_groups,
};

static const char *const emmitsburg_group_h_names[] = {
	"GPPC_H0",	"n/a", "n/a",
	"GPPC_H1",	"n/a", "n/a",
	"GPPC_H2",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H3",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H4",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H5",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H6",	"n/a", "n/a",
	"GPPC_H7",	"n/a", "n/a",
	"GPPC_H8",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H9",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H10",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H11",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H12",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H13",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H14",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPPC_H15",	"n/a", "ISCLK_CLK_OUT_0",
	"GPPC_H16",	"n/a", "ISCLK_CLK_OUT_1",
	"GPPC_H17",	"n/a", "ISCLK_CLK_OUT_2",
	"GPPC_H18",	"n/a", "PMCALERT_N",
	"GPPC_H19",	"n/a", "n/a",
};

static const struct gpio_group emmitsburg_group_h = {
	.display	= "------- GPIO Group GPPC_H -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_h_names) / 3,
	.func_count	= 3,
	.pad_names	= emmitsburg_group_h_names,
};

static const char *const emmitsburg_group_j_names[] = {
	"GPPC_J0",	"CPUPWRGD",
	"GPPC_J1",	"CPU_THRMTRIP_N",
	"GPPC_J2",	"PLTRST_CPU_N",
	"GPPC_J3",	"TRIGGER0_N",
	"GPPC_J4",	"TRIGGER1_N",
	"GPPC_J5",	"CPU_PWER_DEBUG_N",
	"GPPC_J6",	"CPU_MEMTRIP_N",
	"GPPC_J7",	"CPU_MSMI_N",
	"GPPC_J8",	"ME_PECI",
	"GPPC_J9",	"n/a", // Not described in EDS, add here to take space
	"GPPC_J10",	"n/a", // Not described in EDS, add here to take space
	"GPPC_J11",	"n/a", // Not described in EDS, add here to take space
	"GPPC_J12",	"CPU_ERR0_N",
	"GPPC_J13",	"CPU_CATERR_N",
	"GPPC_J14",	"CPU_ERR1_N",
	"GPPC_J15",	"CPU_ERR2_N",
};

static const struct gpio_group emmitsburg_group_j = {
	.display	= "------- GPIO Group GPP_J -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_j_names) / 2,
	.func_count	= 2,
	.pad_names	= emmitsburg_group_j_names,
};

static const struct gpio_group *const emmitsburg_community4_hj_groups[] = {
	&emmitsburg_group_h,
	&emmitsburg_group_j,
};

static const struct gpio_community emmitsburg_community4_hj = {
	.name		= "------- GPIO Community 4 -------",
	.pcr_port_id	= 0x6a,
	.group_count	= ARRAY_SIZE(emmitsburg_community4_hj_groups),
	.groups		= emmitsburg_community4_hj_groups,
};

static const char *const emmitsburg_group_i_names[] = {
	"GPP_I0",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I1",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I2",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I3",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I4",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I5",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I6",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I7",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I8",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I9",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I10",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I11",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I12",	"n/a", "HDA_BCLK",
	"GPP_I13",	"n/a", "HDA_RST_N",
	"GPP_I14",	"n/a", "HDA_SYNC",
	"GPP_I15",	"n/a", "HDA_SDO",
	"GPP_I16",	"n/a", "HDA_SDI_0",
	"GPP_I17",	"n/a", "HDA_SDI_1",
	"GPP_I18",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I19",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I20",	"n/a", "n/a", // Not described in EDS, add here to take space
	"GPP_I21",	"n/a", "n/a",
	"GPP_I22",	"n/a", "n/a",
	"GPP_I23",	"n/a", "n/a",
};

static const struct gpio_group emmitsburg_group_i = {
	.display	= "------- GPIO Group GPP_I -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_i_names) / 3,
	.func_count	= 3,
	.pad_names	= emmitsburg_group_i_names,
};

static const char *const emmitsburg_group_l_names[] = {
	"GPP_L0",	"PM_SYNC_0",
	"GPP_L1",	"PM_DOWN_0",
	"GPP_L2",	"SUSCLK_CPU",
	"GPP_L3",	"n/a",
	"GPP_L4",	"n/a",
	"GPP_L5",	"n/a",
	"GPP_L6",	"n/a",
	"GPP_L7",	"n/a",
	"GPP_L8",	"n/a",
};

static const struct gpio_group emmitsburg_group_l = {
	.display	= "------- GPIO Group GPP_L -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_l_names) / 2,
	.func_count	= 2,
	.pad_names	= emmitsburg_group_l_names,
};

static const char *const emmitsburg_group_m_names[] = {
	"GPP_M0",
	"GPP_M1",
	"GPP_M2",
	"GPP_M3",
	"GPP_M4",
	"GPP_M5",
	"GPP_M6",
	"GPP_M7",
	"GPP_M8",
	"GPP_M9",
	"GPP_M10",
	"GPP_M11",
	"GPP_M12",
	"GPP_M13",
	"GPP_M14",
	"GPP_M15",
	"GPP_M16",
	"GPP_M17",
};

static const struct gpio_group emmitsburg_group_m = {
	.display	= "------- GPIO Group GPP_M -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_m_names) / 1,
	.func_count	= 1,
	.pad_offset = 0x9A0,
	.pad_names	= emmitsburg_group_m_names,
};

static const char *const emmitsburg_group_n_names[] = {
	"GPP_N0",
	"GPP_N1",
	"GPP_N2",
	"GPP_N3",
	"GPP_N4",
};

static const struct gpio_group emmitsburg_group_n = {
	.display	= "------- GPIO Group GPP_N -------",
	.pad_count	= ARRAY_SIZE(emmitsburg_group_n_names) / 1,
	.func_count	= 1,
	.pad_names	= emmitsburg_group_n_names,
};

static const struct gpio_group *const emmitsburg_community5_ilmn_groups[] = {
	&emmitsburg_group_i,
	&emmitsburg_group_l,
	&emmitsburg_group_m,
	&emmitsburg_group_n,
};

static const struct gpio_community emmitsburg_community5_ilmn = {
	.name		= "------- GPIO Community 5 -------",
	.pcr_port_id	= 0x69,
	.group_count	= ARRAY_SIZE(emmitsburg_community5_ilmn_groups),
	.groups		= emmitsburg_community5_ilmn_groups,
};

static const struct gpio_community *const emmitsburg_communities[] = {
	&emmitsburg_community0_abs,
	&emmitsburg_community1_cd,
	&emmitsburg_community3_e_jtag,
	&emmitsburg_community4_hj,
	&emmitsburg_community5_ilmn,
};

#endif
