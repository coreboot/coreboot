/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#ifndef INTEL_WTM2_GPIO_H
#define INTEL_WTM2_GPIO_H

#include "southbridge/intel/lynxpoint/lp_gpio.h"

const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	/* LPSS_UART1_RXD */
	{ .gpio  = 0,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART1_TXD */
	{ .gpio  = 1,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART1_RTS_N_R */
	{ .gpio  = 2,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART1_CTS_N */
	{ .gpio  = 3,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_I2C0_SDA_R */
	{ .gpio  = 4,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_I2C0_SCL */
	{ .gpio  = 5,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_I2C1_SDA */
	{ .gpio  = 6,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_I2C1_SCL */
	{ .gpio  = 7,
	  .conf0 = GPIO_MODE_NATIVE },
	/* NGFF_SLTA_WIFI_WAKE_N */
	{ .gpio  = 8,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* ACCEL_INT2_MCP (NGFF_SLTA_UART_WAKE_N) */
	{ .gpio  = 9,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SMC_RUNTIME_SCI_N */
	{ .gpio  = 10,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_INVERT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_ACPI,
	  .route = GPIO_ROUTE_SCI },
	/* AMB_THRM_R_N */
	{ .gpio  = 11,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PM_LANPHY_ENABLE */
	{ .gpio  = 12,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB32_P0_PWREN */
	{ .gpio  = 13,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SH_INT_ACCEL_DRDY_USB_INT_N */
	{ .gpio  = 14,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT | GPIO_IRQ_EDGE,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* LAN_PWREN_N */
	{ .gpio  = 15,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* LAN_RST_N */
	{ .gpio  = 16,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* CRIT_TEMP_REP_R_N (NGFF_SLTB_CARD_PRESENT_SSD_N) */
	{ .gpio  = 17,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW },
	/* TBT_FORCE_PWR */
	{ .gpio  = 18,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* EC_IN_RW */
	{ .gpio  = 19,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE,
	  .owner = GPIO_OWNER_GPIO },
	/* CK_REQ_P2_NGFFSLTA_N_R */
	{ .gpio  = 20,
	  .conf0 = GPIO_MODE_NATIVE },
	/* CK_PCIE_LAN_REQ_N */
	{ .gpio  = 21,
	  .conf0 = GPIO_MODE_NATIVE },
	/* CK_REQ_P4_TBT_N */
	{ .gpio  = 22,
	  .conf0 = GPIO_MODE_NATIVE },
	/* CK_REQ_P5_N */
	{ .gpio  = 23,
	  .conf0 = GPIO_MODE_NATIVE },
	/* ME_PG_LED (NGFF_SLTB_WWN_CRD_PRSNT) */
	{ .gpio  = 24,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* USB_WAKEOUT_N */
	{ .gpio  = 25,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE },
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
	/* PCH_NFC_RESET */
	{ .gpio  = 28,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PCH_SLP_WLAN_N */
	{ .gpio  = 29,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SUS_PWR_ACK_R */
	{ .gpio  = 30,
	  .conf0 = GPIO_MODE_NATIVE },
	/* AC_PRESENT_R */
	{ .gpio  = 31,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PM_CKRUN_N */
	{ .gpio  = 32,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SATA0_PHYSLP */
	{ .gpio  = 33,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* ESATA_DET_N */
	{ .gpio  = 34,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SATA_DIRECT_PRSNT_R_N */
	{ .gpio  = 35,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* NGFF_SSD_SATA2_PCIE1_DET_N */
	{ .gpio  = 36,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* NGFF_SSD_SATA3_PCIE0_DET_N */
	{ .gpio  = 37,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SATA1_PHYSLP_DIRECT */
	{ .gpio  = 38,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SMC_EXTSMI_N_R */
	{ .gpio  = 39,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_ACPI,
	  .route = GPIO_ROUTE_SMI },
	/* USB_OC_0_1_R_N */
	{ .gpio  = 40,
	  .conf0 = GPIO_MODE_NATIVE },
	/* USB_OC_2_6_R_N */
	{ .gpio  = 41,
	  .conf0 = GPIO_MODE_NATIVE },
	/* TBT_CIO_PLUG_SMI_N_R */
	{ .gpio  = 42,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* USB32_P1_PWREN */
	{ .gpio  = 43,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SENSOR_HUB_RST_N */
	{ .gpio  = 44,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* GYRO_INT2_MCP_R (TBT_WAKE_Q_N) */
	{ .gpio  = 45,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SNSR_HUB_PWREN */
	{ .gpio  = 46,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SPI_TPM_HDR_IRQ_N */
	{ .gpio  = 47,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* PCIE_TBT_RST_N */
	{ .gpio  = 48,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* COMBO_JD */
	{ .gpio  = 49,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* TOUCH_PANEL_INTR_N */
	{ .gpio  = 50,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* PCH_WIFI_RF_KILL_N */
	{ .gpio  = 51,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* TOUCH_PNL_RST_N_R */
	{ .gpio  = 52,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SNSR_HUB_I2C_WAKE / ALS_INT_MCP (NGFF_SLTB_WWAN_SSD_DET2) */
	{ .gpio  = 53,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO },
	/* NGFF_SLTB_SSD_MC_WAKE_N */
	{ .gpio  = 54,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO },
	/* TOUCHPAD_INTR_N */
	{ .gpio  = 55,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT,
	  .conf1 = GPIO_SENSE_ENABLE,
	  .owner = GPIO_OWNER_GPIO,
	  .irqen = GPIO_IRQ_ENABLE },
	/* NGFF_SLTB_WWAN_SSD_DET1 */
	{ .gpio  = 56,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* NGFF_SLTB_WWAN_PWREN */
	{ .gpio  = 57,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* SLATEMODE_HALLOUT_R */
	{ .gpio  = 58,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* USB2_CAM_PWREN */
	{ .gpio  = 59,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* USB_CR_PWREN_N */
	{ .gpio  = 60,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PM_SUS_STAT_N */
	{ .gpio  = 61,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SUS_CK */
	{ .gpio  = 62,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SLP_S5_R_N */
	{ .gpio  = 63,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_CLK_CMNHDR_R */
	{ .gpio  = 64,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_CMD_CMNHDR_R */
	{ .gpio  = 65,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_D0_CMNHDR_R */
	{ .gpio  = 66,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_D1_CMNHDR_R */
	{ .gpio  = 67,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_D2_CMNHDR_R */
	{ .gpio  = 68,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_SDIO_D3_CMNHDR_R1 */
	{ .gpio  = 69,
	  .conf0 = GPIO_MODE_NATIVE },
	/* NGFF_SLTA_WIFI_PWREN_N_R (SDIO_POWER_EN) */
	{ .gpio  = 70,
	  .conf0 = GPIO_MODE_NATIVE },
	/* MPHY_PWREN */
	{ .gpio  = 71,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PM_BATLOW_R_N */
	{ .gpio  = 72,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PCH_NOT_N */
	{ .gpio  = 73,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SML1_DATA */
	{ .gpio  = 74,
	  .conf0 = GPIO_MODE_NATIVE },
	/* SML1_CK */
	{ .gpio  = 75,
	  .conf0 = GPIO_MODE_NATIVE },
	/* PCH_AUDIO_PWR_R */
	{ .gpio  = 76,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_HIGH,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PC_SLTB_SSD_RST_N_R */
	{ .gpio  = 77,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* PM_EXTTS0_EC_N */
	{ .gpio  = 78,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* SIO1007_IRQ_N */
	{ .gpio  = 79,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* PM_EXTTS1_R_N */
	{ .gpio  = 80,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_INPUT },
	/* PCH_HDA_SPKR */
	{ .gpio  = 81,
	  .conf0 = GPIO_MODE_NATIVE },
	/* H_RCIN_N */
	{ .gpio  = 82,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI0_CS_R_N */
	{ .gpio  = 83,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI0_CLK_R */
	{ .gpio  = 84,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI0_MISO_R */
	{ .gpio  = 85,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI0_MOSI_BBS0_R */
	{ .gpio  = 86,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI1_CS_R_N */
	{ .gpio  = 87,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI1_CLK_R */
	{ .gpio  = 88,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_GSPI1_MISO_R */
	{ .gpio  = 89,
	  .conf0 = GPIO_MODE_NATIVE },
	/* NGFF_SLTA_WIFI_RST_N */
	{ .gpio  = 90,
	  .conf0 = GPIO_MODE_GPIO | GPIO_DIR_OUTPUT | GPO_LEVEL_LOW,
	  .conf1 = GPIO_SENSE_DISABLE },
	/* LPSS_UART0_RXD */
	{ .gpio  = 91,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART0_TXD */
	{ .gpio  = 92,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART0_RTS_N */
	{ .gpio  = 93,
	  .conf0 = GPIO_MODE_NATIVE },
	/* LPSS_UART0_CTS_N */
	{ .gpio  = 94,
	  .conf0 = GPIO_MODE_NATIVE },
	/* END */
	{ .gpio  = GPIO_LIST_END }
};

#endif
