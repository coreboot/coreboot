/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <compiler.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
/* RCIN# */		PAD_CFG_GPI(GPP_A0, NONE, DEEP), /* PCH_CSI_GPIO1 */
/* ESPI_IO0 */
/* ESPI_IO1 */
/* ESPI_IO2 */
/* ESPI_IO3 */
/* ESPI_CS# */
/* SERIRQ */		PAD_CFG_GPI(GPP_A6, NONE, DEEP), /* PCH_CSI_GPIO2 */
/* PIRQA# */		PAD_NC(GPP_A7, NONE), /* PCH_CSI_GPIO1 */
/* CLKRUN# */		PAD_CFG_GPI(GPP_A8, NONE, DEEP), /* EC_IN_RW_OD */
/* ESPI_CLK */
/* CLKOUT_LPC1 */	PAD_CFG_GPO(GPP_A10, 0, DEEP), /* PEN_RESET_ODL */
/* PME# */		PAD_NC(GPP_A11, NONE),
/* BM_BUSY# */		PAD_CFG_GPI(GPP_A12, UP_20K, DEEP), /* FPMCU_INT */
/* SUSWARN# */
/* ESPI_RESET# */
/* SUSACK# */
/* SD_1P8_SEL */	PAD_NC(GPP_A16, NONE),
/* SD_PWR_EN# */
/* ISH_GP0 */		PAD_CFG_GPO(GPP_A18, 0, DEEP),
					/* GNSS_DISABLE_1V8_ODL */
/* ISH_GP1 */		PAD_CFG_GPO(GPP_A19, 0, DEEP),
					/* WWAN_RADIO_DISABLE_1V8_ODL */
/* ISH_GP2 */		PAD_CFG_GPI(GPP_A20, NONE, DEEP), /* GP_INT_ODL */
/* ISH_GP3 */		PAD_CFG_GPI(GPP_A21, NONE, DEEP), /* FPMCU_PCH_BOOT0 */
/* ISH_GP4 */		PAD_CFG_GPI(GPP_A22, UP_20K, DEEP), /* FPMCU_INT */
/* ISH_GP5 */		PAD_CFG_GPO(GPP_A23, 0, DEEP), /* FPMCU_RST_ODL */
/* CORE_VID0 */		PAD_NC(GPP_B0, NONE),
/* CORE_VID1 */		PAD_NC(GPP_B1, NONE),
/* VRALERT# */		PAD_NC(GPP_B2, NONE),
/* CPU_GP2 */		PAD_NC(GPP_B3, NONE),
/* CPU_GP3 */		PAD_NC(GPP_B4, NONE),
/* SRCCLKREQ0# */	PAD_NC(GPP_B5, NONE),
/* SRCCLKREQ1# */
/* SRCCLKREQ2# */
/* SRCCLKREQ3# */
/* SRCCLKREQ4# */	PAD_NC(GPP_B9, NONE),
/* SRCCLKREQ5# */	PAD_NC(GPP_B10, NONE),
/* EXT_PWR_GATE# */
/* SLP_S0# */
/* PLTRST# */
/* SPKR */		PAD_NC(GPP_B14, DN_20K), /* GPP_B14_STRAP */
#if IS_ENABLED(CONFIG_ZOOMBINI_USE_SPI_TPM)
/* GSPI0_CS# */		/* H1_SLAVE_SPI_CS_L */
/* GSPI0_CLK */		/* H1_SLAVE_SPI_CLK_R */
/* GSPI0_MISO */	/* H1_SLAVE_SPI_MISO_R */
/* GSPI0_MOSI */	/* H1_SLAVE_SPI_MOSI_R */
#else
/* GSPI0_CS# */		PAD_NC(GPP_B15, NONE),
/* GSPI0_CLK */		PAD_NC(GPP_B16, NONE),
/* GSPI0_MISO */	PAD_NC(GPP_B17, NONE),
/* GSPI0_MOSI */	PAD_NC(GPP_B18, NONE),
#endif
/* GSPI1_CS# */
/* GSPI1_CLK */
/* GSPI1_MISO */
/* GSPI1_MOSI */
/* SML1ALERT# */	PAD_NC(GPP_B23, DN_20K), /* GPP_B23_STRAP */
/* SMBCLK */		PAD_CFG_GPO(GPP_C0, 0, DEEP), /* SOC_EDP_CABC_EN */
/* SMBDATA */		PAD_CFG_GPI(GPP_C1, NONE, DEEP),
					/* PCIE_8_WLAN_WAKE_ODL */
/* SMBALERT# */		PAD_NC(GPP_C2, DN_20K), /* GPP_C2_STRAP - 20K_PD */
/* SML0CLK */		PAD_CFG_GPO(GPP_C3, 0, DEEP),
					/* EN_PP3300_TRACKPAD */
/* SML0DATA */		PAD_NC(GPP_C4, NONE),
/* SML0ALERT# */	PAD_NC(GPP_C5, DN_20K), /* GPP_C5_STRAP - 20K_PD */
/* SM1CLK */		PAD_CFG_GPI(GPP_C6, NONE, DEEP), /* PEN_PDCT_ODL */
/* SM1DATA */		PAD_CFG_GPI(GPP_C7, NONE, DEEP), /* PEN_INT_ODL */
/* UART0_RXD */
/* UART0_TXD */
/* UART0_RTS# */	PAD_NC(GPP_C10, NONE),
/* UART0_CTS# */	PAD_NC(GPP_C11, NONE),
/* UART1_RXD */         PAD_CFG_GPI_APIC(GPP_C12, NONE, DEEP, EDGE_SINGLE,
					 INVERT), /* H1_PCH_INT_ODL */
/* UART1_TXD */		PAD_CFG_GPI(GPP_C13, NONE, DEEP), /* TRACKPAD_INT_ODL */
/* UART1_RTS# */	PAD_CFG_GPI(GPP_C14, NONE, DEEP),
					/* TOUCHSCREEN_INT_ODL */
/* UART1_CTS# */	PAD_CFG_GPI(GPP_C15, NONE, DEEP), /* TRACKPAD_INT_ODL */
/* I2C0_SDA */
/* I2C0_SCL */
/* I2C1_SDA */
/* I2C1_SCL */
/* UART2_RXD */		PAD_NC(GPP_C20, NONE),
/* UART2_TXD */		PAD_NC(GPP_C21, NONE),
/* UART2_RTS# */	PAD_NC(GPP_C22, NONE),
/* UART2_CTS# */	PAD_NC(GPP_C23, NONE),
/* SPI1_CS# */		PAD_CFG_GPI(GPP_D0, NONE, DEEP), /* PCH_MEM_STRAP3 */
/* SPI1_CLK */		PAD_NC(GPP_D1, NONE),
/* SPI1_MISO */		PAD_NC(GPP_D2, NONE),
/* SPI1_MOSI */		PAD_CFG_GPI(GPP_D3, NONE, DEEP), /* PCH_MEM_STRAP0 */
/* FASHTRIG */
/* ISH_I2C0_SDA */	PAD_CFG_NF(GPP_D5, NONE, DEEP, NF1),
					/* EC_I2C_SENSOR_SDA */
/* ISH_I2C0_SCL */	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
					/* EC_I2C_SENSOR_SCL */
/* ISH_I2C1_SDA */	PAD_CFG_GPI(GPP_D7, NONE, DEEP), /* WWAN_SAR_INT_ODL */
/* ISH_I2C1_SCL */	PAD_NC(GPP_D8, NONE),
/* ISH_SPI_CS# */	PAD_CFG_GPO(GPP_D9, 0, DEEP),
					/* EN_PP3300_TOUCHSCREEN */
/* ISH_SPI_CLK */	PAD_CFG_GPO(GPP_D10, 0, DEEP), /* EN_PP3300_WLAN */
/* ISH_SPI_MISO */	PAD_CFG_GPO(GPP_D11, 0, DEEP), /* EN_PP3300_WWAN */
/* ISH_SPI_MOSI */	PAD_CFG_GPI(GPP_D12, NONE, DEEP), /* GPP_D12_STRAP */
/* ISH_UART0_RXD */	PAD_NC(GPP_D13, NONE), /* ISH_UART_RX */
/* ISH_UART0_TXD */	PAD_NC(GPP_D14, NONE), /* ISH_UART_TX */
/* ISH_UART0_RTS# */	PAD_CFG_GPO(GPP_D15, 0, DEEP),
					/* TOUCHSCREEN_RST_ODL */
/* ISH_UART0_CTS# */	PAD_CFG_GPO(GPP_D16, 0, DEEP), /* SPK_PA_EN */
/* DMIC_CLK1 */		PAD_CFG_NF(GPP_D17, NONE, DEEP, NF1),
					/* BASE_CAM_DMIC_CLK */
/* DMIC_DATA1 */	PAD_CFG_NF(GPP_D18, NONE, DEEP, NF1),
					/* BASE_CAM_DMIC_DATA */
/* DMIC_CLK0 */		PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
					/* LID_CAM_DMIC_CLK */
/* DMIC_DATA0 */	PAD_CFG_NF(GPP_D20, NONE, DEEP, NF1),
					/* LID_CAM_DMIC_DATA */
/* SPI1_IO2 */		PAD_CFG_GPI(GPP_D21, NONE, DEEP), /* PCH_MEM_STRAP1 */
/* SPI1_IO3 */		PAD_CFG_GPI(GPP_D22, NONE, DEEP), /* PCH_MEM_STRAP2 */
/* I2S_MCLK */
/* SATAXPCI0 */		PAD_NC(GPP_E0, NONE), /* PCH_TP_1 */
/* SATAXPCIE1 */	PAD_NC(GPP_E1, NONE), /* PCH_TP_2 */
/* SATAXPCIE2 */	PAD_NC(GPP_E2, NONE), /* PCH_TP_3 */
/* CPU_GP0 */		PAD_NC(GPP_E3, NONE),
/* SATA_DEVSLP0 */	PAD_CFG_GPI(GPP_E4, NONE, DEEP),
					/* PCIE_7_WWAN_WAKE_ODL */
/* SATA_DEVSLP1 */	PAD_NC(GPP_E5, NONE),
/* SATA_DEVSLP2 */	PAD_NC(GPP_E6, NONE),
/* CPU_GP1 */		PAD_NC(GPP_E7, NONE),
/* SATALED# */		PAD_NC(GPP_E8, NONE), /* PCH_TP_4 */
/* USB2_OCO# */
/* USB2_OC1# */
/* USB2_OC2# */
/* USB2_OC3# */
/* DDPB_HPD0 */
/* DDPC_HPD1 */		PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1), /* USB_C1_HPD */
/* DDPD_HPD2 */
/* DDPE_HPD3 */		PAD_NC(GPP_E16, NONE),
/* EDP_HPD */
/* DDPB_CTRLCLK */	PAD_NC(GPP_E18, NONE),
/* DDPB_CTRLDATA */	PAD_CFG_GPI(GPP_E19, DN_20K, DEEP), /* GPP_E19_STRAP */
/* DDPC_CTRLCLK */	PAD_NC(GPP_E20, NONE),
/* DDPC_CTRLDATA */	PAD_CFG_GPI(GPP_E21, DN_20K, DEEP), /* GPP_E21_STRAP */
/* DDPD_CTRLCLK */	PAD_NC(GPP_E22, NONE),
/* DDPD_CTRLDATA */	PAD_CFG_GPI(GPP_E23, DN_20K, DEEP), /* I2S2_SCLK */
/* I2S2_SFRM */		PAD_CFG_GPO(GPP_F1, 0, DEEP),
					/* WWAN_RESET_1V8_ODL */
/* I2S2_TXD */		PAD_NC(GPP_F2, NONE),
/* I2S2_RXD */		PAD_CFG_GPO(GPP_F3, 0, DEEP), /* CNV_RF_RESET_L */
/* I2C2_SDA */
/* I2C2_SCL */
/* I2C3_SDA */
/* I2C3_SCL */
/* I2C4_SDA */
/* I2C4_SCL */
/* I2C5_SDA */
/* I2C5_SCL */
/* EMMC_CMD */
/* EMMC_DATA0 */
/* EMMC_DATA1 */
/* EMMC_DATA2 */
/* EMMC_DATA3 */
/* EMMC_DATA4 */
/* EMMC_DATA5 */
/* EMMC_DATA6 */
/* EMMC_DATA7 */
/* EMMC_RCLK */
/* EMMC_CLK */
/* RSVD */		PAD_NC(GPP_F23, NONE),
/* SD_CMD */
/* SD_DATA0 */
/* SD_DATA1 */
/* SD_DATA2 */
/* SD_DATA3 */
/* SD_CD# */
/* SD_CLK */
/* SD_WP */		PAD_CFG_NF(GPP_G7, NONE, DEEP, NF1), /* SD_CD_ODL */
/* I2S2_SCLK */		PAD_NC(GPP_H0, NONE), /* I2S2_SCLK - TP75 */
/* I2S2_SFRM */		PAD_NC(GPP_H1, NONE), /* I2S2_SFRM - TP74 */
/* I2S2_TXD */
/* I2S2_RXD */		PAD_NC(GPP_H3, NONE), /* I2S2_PCH_RX TP76 */
/* I2C2_SDA */
/* I2C2_SCL */
/* I2C3_SDA */
/* I2C3_SCL */
/* I2C4_SDA */
/* I2C4_SCL */
/* I2C5_SDA */
/* I2C5_SCL */
/* M2_SKT2_CFG0 */	PAD_NC(GPP_H12, NONE), /* PCH_TP_5 - TP60 */
/* M2_SKT2_CFG1 */	PAD_NC(GPP_H13, NONE), /* PCH_TP_6 - TP61 */
/* M2_SKT2_CFG2 */	PAD_NC(GPP_H14, NONE), /* PCH_TP_7 - TP62 */
/* M2_SKT2_CFG3 */	PAD_NC(GPP_H15, NONE), /* PCH_TP_8 - TP63 */
/* DDPF_CTRLCLK */	PAD_NC(GPP_H16, NONE),
/* DPPF_CTRLDATA */	PAD_NC(GPP_H17, NONE),
/* CPU_C10_GATE# */	PAD_CFG_NF(GPP_H18, NONE, DEEP, NF1),
					/* CPU_C10_GATE_PCH_L */
/* TIMESYNC0 */		PAD_NC(GPP_H19, NONE),
/* IMGCLKOUT1 */	PAD_NC(GPP_H20, NONE),
/* GPP_H21 */		PAD_CFG_GPI(GPP_H21, DN_20K, DEEP), /* H21_STRAP */
/* GPP_H22 */
/* GPP_H23 */		PAD_NC(GPP_H23, DN_20K), /* H23_STRAP */

/* BATLOW# */
/* ACPRESENT */		PAD_NC(GPD1, NONE),
/* LAN_WAKE# */		PAD_CFG_NF(GPD2, NONE, DEEP, NF1), /* EC_PCH_WAKE_ODL */
/* PWRBTN# */
/* SLP_S3# */
/* SLP_S4# */
/* SLP_A# */		PAD_NC(GPD6, NONE),
/* RSVD */
/* SUSCLK */
/* SLP_WLAN# */		PAD_NC(GPD9, NONE),
/* SLP_S5# */		PAD_NC(GPD10, NONE),
/* LANPHYC */		PAD_NC(GPD11, NONE),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
#if IS_ENABLED(CONFIG_ZOOMBINI_USE_SPI_TPM)
/* GSPI0_CS# */		PAD_CFG_NF(GPP_B15, NONE, DEEP,
				   NF1), /* PCH_SPI_H1_3V3_CS_L */
/* GSPI0_CLK */		PAD_CFG_NF(GPP_B16, NONE, DEEP,
				   NF1), /* PCH_SPI_H1_3V3_CLK */
/* GSPI0_MISO */	PAD_CFG_NF(GPP_B17, NONE, DEEP,
				   NF1), /* PCH_SPI_H1_3V3_MISO */
/* GSPI0_MOSI */	PAD_CFG_NF(GPP_B18, NONE, DEEP,
				   NF1), /* PCH_SPI_H1_3V3_MOSI */
#endif
#if IS_ENABLED(CONFIG_ZOOMBINI_USE_I2C_TPM)
/* I2C5_SDA */
/* I2C5_SCL */
#endif
/* Ensure UART pins are in native mode for H1. */
/* UART0_RXD */		/* UART_PCH_RX_DEBUG_TX */
/* UART0_TXD */		/* UART_PCH_RX_DEBUG_RX */
/* UART1_RXD */         PAD_CFG_GPI_APIC(GPP_C12, NONE, DEEP, EDGE_SINGLE,
					 INVERT), /* H1_PCH_INT_ODL */
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak
	variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
};

const struct cros_gpio *__weak variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
