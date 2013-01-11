/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef INTEL_WTM1_GPIO_H
#define INTEL_WTM1_GPIO_H

#include "southbridge/intel/lynxpoint/lp_gpio.h"

const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	/* UART1_RXD */
	{ .gpio  = 0,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART1_TXD */
	{ .gpio  = 1,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART1_RTSD */
	{ .gpio  = 2,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART1_CTSB */
	{ .gpio  = 3,
	  .conf0 = GPIO_MODE_NATIVE },
	/* I2C0_SDA */
	{ .gpio  = 4,
	  .conf0 = GPIO_MODE_NATIVE },
	/* I2C0_SCL */
	{ .gpio  = 5,
	  .conf0 = GPIO_MODE_NATIVE },
	/* I2C1_SDA */
	{ .gpio  = 6,
	  .conf0 = GPIO_MODE_NATIVE },
	/* I2C1_SCL */
	{ .gpio  = 7,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PCIE_SLOT3_WAKE_R_N / ICC_EN_N */
	{ .gpio  = 8,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCH_GPIO9 / XDP_FN5 */
	{ .gpio  = 9,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCH_GPIO10 / XDP_FN6 */
	{ .gpio  = 10,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SMBALERTB / PCH_GPIO11 / AMB_THRM_R_N */
	{ .gpio  = 11,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PM_LANPHY_ENABLE */
	{ .gpio  = 12,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB30_P0_PWREN */
	{ .gpio  = 13,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SENSOR_HUB_INT_R_N */
	{ .gpio  = 14,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_EDGE,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* HOST_ALERT1_R_N */
	{ .gpio  = 15,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* LAN_RST_N */
	{ .gpio  = 16,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* CRIT_TEMP_REP_N */
	{ .gpio  = 17,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW },
	/* SRC0CLKRQB */
	{ .gpio  = 18,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SRC1CLKRQB */
	{ .gpio  = 19,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SRC2CLKRQB */
	{ .gpio  = 20,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SRC3CLKRQB */
	{ .gpio  = 21,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SRC4CLKRQB_TRST2 */
	{ .gpio  = 22,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SRC5CLKRQB_TDI2 */
	{ .gpio  = 23,
	  .conf0 = GPIO_MODE_NATIVE },
	/* BT_ALERT_ME_LED */
	{ .gpio  = 24,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* USB_WAKEOUT_N / USB2_4_5_PWREN */
	{ .gpio  = 25,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* NFC_IRQ_MGP5 */
	{ .gpio  = 26,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_EDGE,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* SMC_WAKE_SCI_N */
	{ .gpio  = 27,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE },
	/* PCH_NFC_RESET_CMNHDR */
	{ .gpio  = 28,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SLP_WLANB */
	{ .gpio  = 29,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SUSWARNB_SUSPWRDNACK_MGPIO1 */
	{ .gpio  = 30,
	  .conf0 = GPIO_MODE_NATIVE },
	/* ACPRESENT_MGPIO2 */
	{ .gpio  = 31,
	  .conf0 = GPIO_MODE_NATIVE },
	/* CLKRUNB */
	{ .gpio  = 32,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SATA0_PHYSLP */
	{ .gpio  = 33,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SMC_EXTSMI_R_N */
	{ .gpio  = 34,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SATA_DIRECT_ODD_PRSNT_R_N */
	{ .gpio  = 35,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* CMNHDR_PEDET2 */
	{ .gpio  = 36,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* MSATA_MPCIE_DET_N */
	{ .gpio  = 37,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SATA1_PHYSLP_DIRECT */
	{ .gpio  = 38,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SATA2_PHYSLP_CMNHDR */
	{ .gpio  = 39,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB_OC_0_1_R_N */
	{ .gpio  = 40,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB_OC_6_7_R_N */
	{ .gpio  = 41,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB_OC_4_5_R_N */
	{ .gpio  = 42,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB32_P1_PWREN */
	{ .gpio  = 43,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SNSR_HUB_RST_N */
	{ .gpio  = 44,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCIE_SLOT4_WAKE_N */
	{ .gpio  = 45,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SNSR_HUB_PWREN_N */
	{ .gpio  = 46,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SPI_TPM_HDR_IRQ_N */
	{ .gpio  = 47,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* SLOT5_RST_GYRO_INT2_N */
	{ .gpio  = 48,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* GPS_EN_NGFF_ALS_INT_N */
	{ .gpio  = 49,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* TOUCH_PANEL_INTR_N */
	{ .gpio  = 50,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* PCIE_SLOT5_WAKE_R_N */
	{ .gpio  = 51,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* GYRO_INT1_CLV / TOUCH_RST_N */
	{ .gpio  = 52,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* CODEC_IRQ */
	{ .gpio  = 53,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* PCH_MC_WAKE_R_N */
	{ .gpio  = 54,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* THCP_INTR_N */
	{ .gpio  = 55,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* PCIE_SLOT4_RST_N */
	{ .gpio  = 56,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCIE_SLOT4_PWREN_N */
	{ .gpio  = 57,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SPI_TPM_HDR_PWREN_N */
	{ .gpio  = 58,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCIE_SLOT3_RST_N */
	{ .gpio  = 59,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCIE_SLOT3_PWREN_N */
	{ .gpio  = 60,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SUS_STATB */
	{ .gpio  = 61,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SUSCLK */
	{ .gpio  = 62,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SLP_S5B */
	{ .gpio  = 63,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_CLK */
	{ .gpio  = 64,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_CMD */
	{ .gpio  = 65,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_D0 */
	{ .gpio  = 66,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_D1 */
	{ .gpio  = 67,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_D2 */
	{ .gpio  = 68,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SDIO_D3 */
	{ .gpio  = 69,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_VDD_EN_CMNHDR_R */
	{ .gpio  = 70,
	  .conf0 = GPIO_MODE_NATIVE },
	/* MPHY_PWREN */
	{ .gpio  = 71,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PM_BATLOW_R_N */
	{ .gpio  = 72,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PCH_GP73_MGP8 / PCH_HOT_N */
	{ .gpio  = 73,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SML1_DATA */
	{ .gpio  = 74,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SML1_DATA */
	{ .gpio  = 75,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PCH_AUDIO_PWR_N */
	{ .gpio  = 76,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* MPCIE_RST_N */
	{ .gpio  = 77,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* EXTTS_SNI_DRV0_PCH */
	{ .gpio  = 78,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SIO1007_IRQ_N */
	{ .gpio  = 79,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_EDGE,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_ACPI,
	  .route = GPIO_ROUTE_SCI,
	  .irqen = GPIO_IRQ_ENABLE },
	/* EXTTS_SNI_DRV1_PCH */
	{ .gpio  = 80,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SPKR */
	{ .gpio  = 81,
	  .conf0 = GPIO_MODE_NATIVE },
	/* RCINB */
	{ .gpio  = 82,
	  .conf0 = GPIO_MODE_NATIVE },
	/* MC_3.3_PWREN_N */
	{ .gpio  = 83,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCIE_SLOT5_PWREN_N */
	{ .gpio  = 84,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* MC_1.5_PWREN_N */
	{ .gpio  = 85,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SATA_ODD_PWRGT_R */
	{ .gpio  = 86,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PECI_HDR / PCH_RF_KILL_N */
	{ .gpio  = 87,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* TCHP_PWREN_N */
	{ .gpio  = 88,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* TOUCH_PWREN_N */
	{ .gpio  = 89,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SATA1_PWR_EN_N  */
	{ .gpio  = 90,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* UART0_RXD */
	{ .gpio  = 91,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART0_TXD */
	{ .gpio  = 92,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART0_RTSB */
	{ .gpio  = 93,
	  .conf0 = GPIO_MODE_NATIVE },
	/* UART0_CTSB */
	{ .gpio  = 94,
	  .conf0 = GPIO_MODE_NATIVE },
	/* END */
	{ .gpio  = GPIO_LIST_END }
};

#endif
