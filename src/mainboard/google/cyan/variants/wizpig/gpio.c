/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <mainboard/google/cyan/irqroute.h>
#include <soc/gpio.h>
#include <stdlib.h>

/* South East Community */
static const struct soc_gpio_map gpse_gpio_map[] = {
	Native_M1,/* MF_PLT_CLK0 */
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
	Native_M1, /* 46 LPC_CLKRUNB */
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
	GPIO_NC, /* SPI1_CS1_B  66 */
	NATIVE_PU20K(1), /* 67 MMC1_D4_SD_WE */
	NATIVE_PU20K(1), /* 68 MMC1_D7 */
	GPIO_NC, /* 69 MMC1_RCLK */
	Native_M1, /* 75  GPO USB_OC1_B */
	Native_M1, /* 76  PMU_RESETBUTTON_B */
	GPI(trig_edge_both, L0, NA, non_maskable, en_edge_detect, NA , NA),
	/* GPIO_ALERT 77   */
	Native_M1, /* 78  SDMMC3_PWR_EN_B */
	GPIO_NC, /* 79  GPI ILB_SERIRQ */
	Native_M1, /* 80  USB_OC0_B */
	NATIVE_INT_PU20K(1, L1), /* 81  SDMMC3_CD_B */
	GPIO_NC,  /* 82  spkr	 assumed gpio number */
	Native_M1, /* 83 SUSPWRDNACK */
	SPARE_PIN,/* 84 spare pin */
	Native_M1, /* 85 SDMMC3_1P8_EN */
	GPIO_END
};


/* South West Community */
static const struct soc_gpio_map  gpsw_gpio_map[] = {
	GPIO_NC, /* 00 FST_SPI_D2 */
	Native_M1, /* 01 FST_SPI_D0 */
	Native_M1, /* 02 FST_SPI_CLK */
	GPIO_NC, /* 03 FST_SPI_D3 */
	GPIO_NC, /* GPO FST_SPI_CS1_B */
	Native_M1, /* 05 FST_SPI_D1 */
	Native_M1, /* 06 FST_SPI_CS0_B */
	GPIO_INPUT_NO_PULL, /* 07 FST_SPI_CS2_B */
	GPIO_NC, /* 15 UART1_RTS_B */
	Native_M2, /* 16 UART1_RXD */
	GPIO_NC, /* 17 UART2_RXD */
	GPIO_NC, /* 18 UART1_CTS_B */
	GPIO_NC, /* 19 UART2_RTS_B */
	Native_M2, /* 20 UART1_TXD */
	GPIO_NC, /* 21 UART2_TXD */
	GPIO_NC, /* 22 UART2_CTS_B */
	GPIO_NC, /* 30 MF_HDA_CLK */
	GPIO_NC, /* 31 GPIO_SW31/MF_HDA_RSTB */
	GPIO_NC, /* 32 GPIO_SW32 /MF_HDA_SDI0 */
	GPIO_NC, /* 33 MF_HDA_SDO */
	GPI(trig_edge_both, L3, P_1K_H, non_maskable, en_edge_detect, NA, NA),
		/* 34 MF_HDA_DOCKRSTB */
	GPIO_NC, /* 35 MF_HDA_SYNC */
	GPIO_NC, /* 36 GPIO_SW36 MF_HDA_SDI1 */
	GPI(trig_edge_both, L2, P_1K_H, non_maskable, en_edge_detect, NA, NA),
		/* 37 MF_HDA_DOCKENB */
	NATIVE_PU1K_CSEN_INVTX(1), /* 45 I2C5_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 46 I2C4_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 47 I2C6_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 48 I2C5_SCL */
	GPIO_NC, /* 49 I2C_NFC_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 50 I2C4_SCL */
	NATIVE_PU1K_CSEN_INVTX(1), /* 51 I2C6_SCL */
	GPIO_NC, /* 52 I2C_NFC_SCL */
	NATIVE_PU1K_CSEN_INVTX(1), /* 60 I2C1_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 61 I2C0_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 62 I2C2_SDA */
	NATIVE_PU1K_CSEN_INVTX(1), /* 63 I2C1_SCL */
	GPIO_INPUT_NO_PULL, /* 64 I2C3_SDA RAMID3*/
	NATIVE_PU1K_CSEN_INVTX(1), /* 65 I2C0_SCL */
	NATIVE_PU1K_CSEN_INVTX(1), /* 66  I2C2_SCL */
	GPIO_INPUT_NO_PULL,/* 67  I2C3_SCL,RAMID1 */
	GPIO_OUT_HIGH, /* 75 SATA_GP0 */
	GPIO_NC, /* 76 GPI SATA_GP1 */
	GPIO_INPUT_PU_20K, /* 77 SATA_LEDN */
	GPIO_NC, /* 78 SATA_GP2 */
	Native_M1, /* 79 NFC_DEV_WAKE , MF_SMB_CLK */
	GPIO_INPUT_NO_PULL, /* 80 SATA_GP3,RAMID0 */
	Native_M1, /* 81 NFC_DEV_WAKE , MF_SMB_CLK */
	Native_M1, /* 82 NFC_FW_DOWNLOAD, MF_SMB_DATA */
	/* Per DE request, change PCIE_CLKREQ0123B to GPIO_INPUT */
	GPIO_NC, /* 90 PCIE_CLKREQ0B */
	GPIO_NC, /* 91 GPI PCIE_CLKREQ1B/LTE_WAKE# */
	Native_M1, /* 92 GP_SSP_2_CLK */
	NATIVE_PU20K(1), /* 93 PCIE_CLKREQ2B/PCIE_CLKREQ_WLAN# */
	Native_M1, /* 94 GP_SSP_2_RXD */
	GPI(trig_edge_both, L1, P_5K_H, 0, en_edge_detect, NA, NA),
		/* 95 PCIE_CLKREQ3B/AUDIO_CODEC_IRQ */
	Native_M1, /* 96 GP_SSP_2_FS */
	NATIVE_FUNC(1, 0, inv_tx_enable), /* 97 GP_SSP_2f_TXD */
	GPIO_END
};


/* North Community */
static const struct soc_gpio_map  gpn_gpio_map[] = {
	GPIO_NC, /* 00 GPIO_DFX0 */
	GPIO_NC, /* 01 GPIO_DFX3 */
	GPIO_NC, /* 02 GPIO_DFX7 */
	GPIO_NC, /* 03 GPIO_DFX1 */
	GPIO_NC, /* 04 GPIO_DFX5 */
	GPIO_NC, /* 05 GPIO_DFX4 */
	GPIO_NC, /* 06 GPIO_DFX8 */
	GPIO_NC, /* 07 GPIO_DFX2 */
	GPIO_NC, /* 08 GPIO_DFX6 */
	GPI(trig_edge_low, L8, NA, non_maskable, en_edge_rx_data ,
	UNMASK_WAKE, SCI), /* 15 GPIO_SUS0 */
	GPO_FUNC(NA, NA), /* 16 SEC_GPIO_SUS10 */
	GPI(trig_edge_low, L0, P_1K_H, non_maskable, NA, NA, NA),
	/* 17 GPIO_SUS3 */
	GPI(trig_level_low, L1, P_1K_H, 0, NA, UNMASK_WAKE, NA),
	/* 18 GPIO_SUS7 */
	GPI(trig_level_low, L1, P_1K_H, 0, NA, UNMASK_WAKE, NA),
	/* 19 GPIO_SUS1 */
	GPIO_NC, /* 20 GPIO_SUS5 */
	GPIO_INPUT_NO_PULL, /* 21 SEC_GPIO_SUS11 */
	GPIO_NC, /* 22 GPIO_SUS4 */
	GPIO_NC,
	/* 23 SEC_GPIO_SUS8 */
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
	Native_M2, /* 45 GP_CAMERASB05 */
	Native_M2, /* 46 GP_CAMERASB02 */
	Native_M2, /* 47 GP_CAMERASB08 */
	Native_M2, /* 48 GP_CAMERASB00 */
	Native_M2, /* 49 GP_CAMERASBO6 */
	GPIO_NC, /* 50 GP_CAMERASB10 */
	Native_M2, /* 51 GP_CAMERASB03 */
	GPIO_NC, /* 52 GP_CAMERASB09 */
	Native_M2, /* 53 GP_CAMERASB01 */
	Native_M2, /* 54 GP_CAMERASB07 */
	GPIO_NC, /* 55 GP_CAMERASB11 */
	Native_M2, /* 56 GP_CAMERASB04 */
	GPIO_NC, /* 60 PANEL0_BKLTEN */
	GPIO_NC, /* 61 HV_DDI0_HPD */
	NATIVE_PU1K_M1, /* 62 HV_DDI2_DDC_SDA */
	Native_M1, /* 63 PANEL1_BKLTCTL */
	NATIVE_TX_RX_EN, /* 64 HV_DDI1_HPD */
	GPIO_NC, /* 65 PANEL0_BKLTCTL */
	GPIO_NC, /* 66 HV_DDI0_DDC_SDA */
	NATIVE_PU1K_M1, /* 67 HV_DDI2_DDC_SCL */
	NATIVE_TX_RX_EN, /* 68 HV_DDI2_HPD */
	Native_M1, /* 69 PANEL1_VDDEN */
	Native_M1, /* 70 PANEL1_BKLTEN */
	GPIO_NC, /* 71 HV_DDI0_DDC_SCL */
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
