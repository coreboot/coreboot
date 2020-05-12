/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>

/* South East Community */
static const struct soc_gpio_map gpse_gpio_map[] = {
	Native_M1,/* 00 MF_PLT_CLK0 */
	GPIO_NC, /* 01 PWM1 */
	GPIO_INPUT_NO_PULL, /* 02 MF_PLT_CLK1, RAMID2 */
	GPIO_NC, /* 03 MF_PLT_CLK4 */
	GPIO_NC, /* 04 MF_PLT_CLK3 */
	GPIO_NC, /* PWM0 05 */
	GPIO_NC, /* 06 MF_PLT_CLK5 */
	GPIO_NC, /* 07 MF_PLT_CLK2 */
	GPIO_NC, /* 15 SDMMC2_D3_CD_B */
	Native_M1, /* 16 SDMMC1_CLK */
	NATIVE_PU20K(1), /* 17 SDMMC1_D0 */
	GPIO_NC, /* 18 SDMMC2_D1 */
	GPIO_NC, /* 19 SDMMC2_CLK */
	NATIVE_PU20K(1),/* 20 SDMMC1_D2 */
	GPIO_NC, /* 21 SDMMC2_D2 */
	GPIO_NC, /* 22 SDMMC2_CMD  */
	NATIVE_PU20K(1), /* 23 SDMMC1_CMD */
	NATIVE_PU20K(1), /* 24 SDMMC1_D1 */
	GPIO_NC, /* 25 SDMMC2_D0 */
	NATIVE_PU20K(1), /* 26 SDMMC1_D3_CD_B */
	NATIVE_PU20K(1), /* 30 SDMMC3_D1 */
	Native_M1, /* 31 SDMMC3_CLK */
	NATIVE_PU20K(1), /* 32 SDMMC3_D3 */
	NATIVE_PU20K(1), /* 33 SDMMC3_D2 */
	NATIVE_PU20K(1), /* 34 SDMMC3_CMD */
	NATIVE_PU20K(1), /* 35 SDMMC3_D0 */
	NATIVE_PU20K(1), /* 45 MF_LPC_AD2 */
	NATIVE_PU20K(1), /* 46 LPC_CLKRUNB */
	NATIVE_PU20K(1), /* 47 MF_LPC_AD0 */
	Native_M1, /* 48 LPC_FRAMEB */
	Native_M1, /* 49 MF_LPC_CLKOUT1 */
	NATIVE_PU20K(1), /* 50 MF_LPC_AD3 */
	Native_M1, /* 51 MF_LPC_CLKOUT0 */
	NATIVE_PU20K(1), /* 52 MF_LPC_AD1 */
	Native_M1,/* SPI1_MISO */
	Native_M1, /* 61 SPI1_CS0_B */
	Native_M1, /* SPI1_CLK */
	NATIVE_PU20K(1), /* 63 MMC1_D6 */
	Native_M1, /* 62 SPI1_MOSI */
	NATIVE_PU20K(1), /* 65 MMC1_D5 */
	GPIO_NC, /* 66 SPI1_CS1_B */
	NATIVE_PU20K(1), /* 67 MMC1_D4_SD_WE */
	NATIVE_PU20K(1), /* 68 MMC1_D7 */
	GPIO_NC, /* 69 MMC1_RCLK */
	Native_M1, /* 75 GPO USB_OC1_B */
	Native_M1, /* 76 PMU_RESETBUTTON_B */
	GPIO_NC, /* 77 GPIO_ALERT */
	Native_M1, /* 78 SDMMC3_PWR_EN_B */
	Native_M1, /* 79 GPI ILB_SERIRQ */
	Native_M1, /* 80 USB_OC0_B */
	NATIVE_INT_PU20K(1, L1), /* 81 SDMMC3_CD_B */
	Native_M1, /* 82 SPKR */
	Native_M1, /* 83 SUSPWRDNACK */
	SPARE_PIN, /* 84 spare pin */
	Native_M1, /* 85 SDMMC3_1P8_EN */
	GPIO_END
};

/* South West Community */
static const struct soc_gpio_map  gpsw_gpio_map[] = {
	NATIVE_PU20K(1), /* 00 FST_SPI_D2 */
	NATIVE_PU20K(1), /* 01 FST_SPI_D0 */
	NATIVE_PU20K(1), /* 02 FST_SPI_CLK */
	NATIVE_PU20K(1), /* 03 FST_SPI_D3 */
	NATIVE_PU20K(1), /* 04 FST_SPI_CS1_B */
	NATIVE_PU20K(1), /* 05 FST_SPI_D1 */
	NATIVE_PU20K(1), /* 06 FST_SPI_CS0_B */
	GPIO_NC, /* 07 FST_SPI_CS2_B NC */
	GPIO_NC, /* 15 UART1_RTS_B */
	GPIO_NC, /* 16 UART1_RXD */
	GPIO_NC, /* 17 UART2_RXD */
	GPIO_NC, /* 18 UART1_CTS_B */
	GPIO_NC, /* 19 UART2_RTS_B */
	GPIO_NC, /* 20 UART1_TXD */
	GPIO_NC, /* 21 UART2_TXD */
	GPIO_NC, /* 22 UART2_CTS_B */
	Native_M2, /* 30 MF_HDA_CLK */
	Native_M2, /* 31 MF_HDA_RSTB */
	Native_M2, /* 32 MF_HDA_SDI0 */
	Native_M2, /* 33 MF_HDA_SDO */
	GPIO_NC, /* 34 MF_HDA_DOCKRSTB */
	Native_M2, /* 35 MF_HDA_SYNC */
	GPIO_NC, /* 36 MF_HDA_SDI1 */
	GPIO_NC, /* 37 MF_HDA_DOCKENB */
	GPIO_NC, /* 45 I2C5_SDA */
	GPIO_NC, /* 46 I2C4_SDA */
	GPIO_INPUT_NO_PULL, /* 47 I2C6_SDA SD_WP_1P8*/
	GPIO_NC, /* 48 I2C5_SCL */
	GPIO_NC, /* 49 I2C_NFC_SDA */
	GPIO_NC, /* 50 I2C4_SCL */
	GPIO_NC, /* 51 I2C6_SCL */
	GPIO_NC, /* 52 I2C_NFC_SCL */
	GPIO_NC, /* 60 I2C1_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 61 I2C0_SDA */
	GPIO_NC, /* 62 I2C2_SDA */
	GPIO_NC, /* 63 I2C1_SCL */
	GPIO_NC, /* 64 I2C3_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 65 I2C0_SCL */
	GPIO_NC, /* 66 I2C2_SCL */
	GPIO_NC, /* 67 I2C3_SCL */
	GPIO_NC, /* 75 SATA_GP0 */
	GPIO_NC, /* 76 GPI SATA_GP1 */
	Native_M1, /* 77 SATA_LEDN */
	GPIO_NC, /* 78 SATA_GP2 */
	Native_M1, /* 79 MF_SMB_ALERT_N */
	GPIO_INPUT_NO_PULL, /* 80 SATA_GP3, MMC1_RST */
	Native_M1, /* 81 MF_SMB_CLK */
	Native_M1, /* 82 MF_SMB_DATA */
	Native_M1, /* 90 PCIE_CLKREQ0B */
	Native_M1, /* 91 PCIE_CLKREQ1B */
	GPIO_NC, /* 92 GP_SSP_2_CLK */
	Native_M1, /* 93 PCIE_CLKREQ2B */
	GPIO_NC, /* 94 GP_SSP_2_RXD */
	Native_M1, /* 93 PCIE_CLKREQ3B */
	GPIO_NC, /* 96 GP_SSP_2_FS */
	GPIO_NC, /* 97 GP_SSP_2_TXD */
	GPIO_END
};

/* North Community */
static const struct soc_gpio_map  gpn_gpio_map[] = {
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data,
	UNMASK_WAKE, SCI), /* 00 GPIO_DFX0 SMC_EXTSMI_N */
	GPIO_NC, /* 01 GPIO_DFX3 */
	GPIO_NC, /* 02 GPIO_DFX7 */
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data,
	UNMASK_WAKE, SCI), /* 03 GPIO_DFX1 PM_THRM_N */
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data,
	UNMASK_WAKE, SCI), /* 04 GPIO_DFX5 LID_N */
	GPIO_NC, /* 05 GPIO_DFX4 */
	GPIO_NC, /* 06 GPIO_DFX8 */
	GPIO_NC, /* 07 GPIO_DFX2 */
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data,
	UNMASK_WAKE, SCI), /* 08 GPIO_DFX6 WAKE1_N */
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data,
	UNMASK_WAKE, SCI), /* 15 GPIO_SUS0 */
	GPIO_NC, /* 16 SEC_GPIO_SUS10 */
	GPI(trig_edge_low, L0, P_1K_H, non_maskable, NA, NA, NA),
	/* 17 GPIO_SUS3 */
	GPI(trig_edge_low, L1, P_1K_H, non_maskable, NA, UNMASK_WAKE, NA),
	/* 18 GPIO_SUS7 */
	GPI(trig_edge_low, L3, P_1K_H, non_maskable, NA, UNMASK_WAKE, NA),
	/* 19 GPIO_SUS1 */
	GPIO_NC, /* 20 GPIO_SUS5 */
	GPIO_NC, /* 21 SEC_GPIO_SUS11 */
	GPIO_NC, /* 22 GPIO_SUS4 */
	GPIO_NC, /* 23 SEC_GPIO_SUS8 */
	Native_M6, /* 24 GPIO_SUS2 */
	GPIO_INPUT_PU_5K,/* 25 GPIO_SUS6 */
	Native_M1, /* 26 CX_PREQ_B */
	GPIO_NC, /* 27 SEC_GPIO_SUS9 */
	Native_M1, /* 30 TRST_B */
	Native_M1, /* 31 TCK */
	GPIO_SKIP, /* 32 PROCHOT_B */
	GPIO_SKIP, /* 33 SVID0_DATA */
	Native_M1, /* 34 TMS */
	GPIO_NC, /* 35 CX_PRDY_B_2 */
	GPIO_NC, /* 36 TDO_2 */
	Native_M1, /* 37 CX_PRDY_B */
	GPIO_SKIP, /* 38 SVID0_ALERT_B */
	Native_M1, /* 39 TDO */
	GPIO_SKIP, /* 40 SVID0_CLK */
	Native_M1, /* 41 TDI */
	GPIO_NC, /* 45 GP_CAMERASB05 */
	GPIO_NC, /* 46 GP_CAMERASB02 */
	Native_M2, /* 47 GP_CAMERASB08 */
	GPIO_NC, /* 48 GP_CAMERASB00 */
	GPIO_NC, /* 49 GP_CAMERASBO6 */
	Native_M2, /* 50 GP_CAMERASB10 */
	GPIO_NC, /* 51 GP_CAMERASB03 */
	Native_M2, /* 52 GP_CAMERASB09 */
	GPIO_NC, /* 53 GP_CAMERASB01 */
	GPIO_NC, /* 54 GP_CAMERASB07 */
	Native_M2, /* 55 GP_CAMERASB11 */
	GPIO_NC, /* 56 GP_CAMERASB04 */
	GPIO_NC, /* 60 PANEL0_BKLTEN */
	Native_M1, /* 61 HV_DDI0_HPD */
	GPIO_NC, /* 62 HV_DDI2_DDC_SDA */
	GPIO_NC, /* 63 PANEL1_BKLTCTL */
	Native_M1, /* 64 HV_DDI1_HPD */
	Native_M1, /* 65 PANEL0_BKLTCTL */
	NATIVE_PU20K(1), /* 66 HV_DDI0_DDC_SDA */
	GPIO_NC, /* 67 HV_DDI2_DDC_SCL */
	NATIVE_TX_RX_EN, /* 68 HV_DDI2_HPD */
	GPIO_NC, /* 69 PANEL1_VDDEN */
	GPIO_NC, /* 70 PANEL1_BKLTEN */
	NATIVE_PU20K(1), /* 71 HV_DDI0_DDC_SCL */
	GPIO_NC, /* 72 PANEL0_VDDEN */
	GPIO_END
};

/* East Community */
static const struct soc_gpio_map  gpe_gpio_map[] = {
	Native_M1, /* 00 PMU_SLP_S3_B */
	GPIO_NC, /* 01 PMU_BATLOW_B */
	Native_M1, /* 02 SUS_STAT_B */
	Native_M1, /* 03 PMU_SLP_S0IX_B */
	Native_M1, /* 04 PMU_AC_PRESENT */
	Native_M1, /* 05 PMU_PLTRST_B */
	Native_M1, /* 06 PMU_SUSCLK */
	GPIO_NC, /* 07 PMU_SLP_LAN_B */
	Native_M1, /* 08 PMU_PWRBTN_B */
	Native_M1, /* 09 PMU_SLP_S4_B */
	NATIVE_FUNC(M1, P_1K_H, NA), /* 10 PMU_WAKE_B */
	GPIO_NC, /* 11 PMU_WAKE_LAN_B */
	GPIO_NC, /* 15 MF_GPIO_3 */
	GPIO_NC, /* 16 MF_GPIO_7 */
	GPIO_NC, /* 17 MF_I2C1_SCL */
	GPIO_NC, /* 18 MF_GPIO_1 */
	GPIO_NC, /* 19 MF_GPIO_5 */
	GPIO_NC, /* 20 MF_GPIO_9 */
	GPIO_NC, /* 21 MF_GPIO_0 */
	GPIO_INPUT_PU_20K, /* 22 MF_GPIO_4 */
	GPIO_NC, /* 23 MF_GPIO_8 */
	GPIO_NC, /* 24 MF_GPIO_2 */
	GPIO_NC, /* 25 MF_GPIO_6 */
	GPIO_NC, /* 26 MF_I2C1_SDA */
	GPIO_END
};

static struct soc_gpio_config gpio_config = {
	/* BSW */
	.north = gpn_gpio_map,
	.southeast = gpse_gpio_map,
	.southwest  = gpsw_gpio_map,
	.east = gpe_gpio_map
};

struct soc_gpio_config *mainboard_get_gpios(void)
{
	return &gpio_config;
}
